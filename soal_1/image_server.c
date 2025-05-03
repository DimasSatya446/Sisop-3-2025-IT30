#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define LOG_FILE "/home/seribu_man/SISOP_3/soal_1/server/server.log"
#define DATABASE_DIR "/home/seribu_man/SISOP_3/soal_1/server/database"

// Fungsi untuk logging server
void log_server(const char *type, const char *message) {
    FILE *log = fopen(LOG_FILE, "a");
    if (!log) {
        fprintf(stderr, "FATAL: Gagal membuka file log %s: %s\n", LOG_FILE, strerror(errno));
        return;
    }
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char stamp[32];
    strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", t);
    fprintf(log, "[Server][%s]: [%s] %s\n", stamp, type, message);
    fflush(log);
    fclose(log);
}

// Fungsi untuk daemonize proses server
void daemonize() {
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) { log_server("FATAL", "Fork pertama gagal"); exit(EXIT_FAILURE); }
    if (pid > 0) { exit(EXIT_SUCCESS); }

    sid = setsid();
    if (sid < 0) { log_server("FATAL", "setsid gagal"); exit(EXIT_FAILURE); }

    if ((chdir("/")) < 0) { log_server("FATAL", "chdir gagal"); exit(EXIT_FAILURE); }
    umask(0);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    pid = fork();
    if (pid < 0) { log_server("FATAL", "Fork kedua gagal"); exit(EXIT_FAILURE); }
    if (pid > 0) { exit(EXIT_SUCCESS); }
}

// Fungsi baru: membalik string
void reverse(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}

// Fungsi baru: mengkonversi hex ke bytes
int hex_to_bytes(const char *hex, unsigned char *out) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;
    for (int i = 0; i < len / 2; i++) {
        if (sscanf(hex + 2 * i, "%2hhx", &out[i]) != 1)
            return -1;
    }
    return len / 2;
}

// Fungsi untuk menangani koneksi klien
void handle_client(int sock) {
    log_server("INFO", "Client connected");
    char buf[BUFFER_SIZE] = {0};
    int n = read(sock, buf, sizeof(buf) - 1);
    if (n <= 0) {
        if (n == 0) log_server("INFO", "Client disconnected");
        else log_server("ERROR", strerror(errno));
        close(sock);
        return;
    }
    buf[n] = '\0';

    char *newline = strstr(buf, "\n");
    if (!newline) {
        log_server("ERROR", "Format perintah tidak valid");
        send(sock, "ERROR: Format perintah tidak valid\n", 35, 0);
        close(sock);
        return;
    }
    *newline = '\0';

    char *command = buf;
    char *data = newline + 1;
    char msg[BUFFER_SIZE + 64];

    if (strncmp(command, "UPLOAD ", 7) == 0) {
        log_server("UPLOAD", "Menerima perintah UPLOAD");
        char *filename_arg = command + 7;
        char original_filename[256];
        strncpy(original_filename, filename_arg, sizeof(original_filename) - 1);
        original_filename[sizeof(original_filename) - 1] = '\0';
        original_filename[strcspn(original_filename, " \t\r\n")] = 0;

        reverse(data);
        size_t hex_len = strlen(data);

        if (hex_len % 2 != 0) {
            log_server("ERROR", "UPLOAD: Panjang hex tidak genap");
            send(sock, "ERROR: Panjang hex tidak genap\n", 31, 0);
            close(sock);
            return;
        }

        unsigned char bin[BUFFER_SIZE / 2];
        int blen = hex_to_bytes(data, bin);
        if (blen < 0) {
            log_server("ERROR", "UPLOAD: Hex decode gagal atau format salah");
            send(sock, "ERROR: Hex decode gagal atau format salah\n", 42, 0);
            close(sock);
            return;
        }

        if (mkdir(DATABASE_DIR, 0777) == -1 && errno != EEXIST) {
            snprintf(msg, sizeof(msg), "UPLOAD: Gagal membuat direktori database: %s", strerror(errno));
            log_server("FATAL", msg);
            send(sock, "ERROR: Gagal membuat direktori di server\n", 41, 0);
            close(sock);
            return;
        }

        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        char filename[256];
        strftime(filename, sizeof(filename), DATABASE_DIR "/%Y%m%d_%H%M%S.jpeg", tm_info);

        FILE *out = fopen(filename, "wb");
        if (!out) {
            snprintf(msg, sizeof(msg), "UPLOAD: Gagal simpan file %s: %s", filename, strerror(errno));
            log_server("ERROR", msg);
            send(sock, "ERROR: Gagal simpan file di server\n", 35, 0);
            close(sock);
            return;
        }
        fwrite(bin, 1, blen, out);
        fclose(out);

        char resp[128];
        strftime(resp, sizeof(resp), "SUCCESS: %Y%m%d_%H%M%S.jpeg\n", tm_info);
        send(sock, resp, strlen(resp), 0);

        snprintf(msg, sizeof(msg), "UPLOAD: File %s (original: %s) disimpan", filename, original_filename);
        log_server("UPLOAD", msg);

    } else if (strncmp(command, "DOWNLOAD ", 9) == 0) {
        log_server("DOWNLOAD", "Menerima perintah DOWNLOAD");
        char *fn = command + 9;
        fn[strcspn(fn, " \t\r\n")] = 0;

        if (strstr(fn, "..") != NULL || strstr(fn, "/") != NULL) {
            log_server("ERROR", "DOWNLOAD: Percobaan Path Traversal terdeteksi");
            send(sock, "ERROR: Nama file tidak valid\n", 29, 0);
            close(sock);
            return;
        }

        char path[256];
        snprintf(path, sizeof(path), DATABASE_DIR "/%s", fn);

        FILE *f = fopen(path, "rb");
        if (!f) {
            snprintf(msg, sizeof(msg), "DOWNLOAD: File %s tidak ditemukan", fn);
            log_server("ERROR", msg);
            send(sock, "ERROR: File tidak ditemukan\n", 29, 0);
            close(sock);
            return;
        }

        send(sock, "SUCCESS\n", 8, 0);

        unsigned char file_data[BUFFER_SIZE];
        size_t bytes_read;
        ssize_t bytes_sent = 0;
        while ((bytes_read = fread(file_data, 1, sizeof(file_data), f)) > 0) {
            bytes_sent = send(sock, file_data, bytes_read, 0);
            if (bytes_sent < 0) {
                snprintf(msg, sizeof(msg), "DOWNLOAD: Gagal mengirim data file %s: %s", fn, strerror(errno));
                log_server("ERROR", msg);
                break;
            }
        }
        fclose(f);

        if (bytes_read < 0 && errno != 0) {
            snprintf(msg, sizeof(msg), "DOWNLOAD: Error membaca file %s: %s", fn, strerror(errno));
            log_server("ERROR", msg);
        } else if (bytes_sent >= 0) {
            snprintf(msg, sizeof(msg), "DOWNLOAD: File %s berhasil dikirim", fn);
            log_server("DOWNLOAD", msg);
        }

    } else {
        snprintf(msg, sizeof(msg), "Perintah tidak dikenali: %s", command);
        log_server("ERROR", msg);
        send(sock, "ERROR: Perintah tidak valid\n", 28, 0);
    }

    close(sock);
}

// Signal handler untuk SIGCHLD
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG | WUNTRACED) > 0);
}

// Fungsi utama server
int main() {
    daemonize();

    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) < 0) {
        log_server("FATAL", "Gagal memasang signal handler SIGCHLD");
        exit(EXIT_FAILURE);
    }

    log_server("INFO", "Server started (daemon mode)");

    if (mkdir(DATABASE_DIR, 0777) == -1 && errno != EEXIST) {
        log_server("FATAL", strerror(errno));
        exit(EXIT_FAILURE);
    }

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { log_server("FATAL", strerror(errno)); exit(EXIT_FAILURE); }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        log_server("WARNING", strerror(errno));
    }

    struct sockaddr_in addr = { .sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(PORT) };

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) { log_server("FATAL", strerror(errno)); exit(EXIT_FAILURE); }
    if (listen(server_fd, 10) < 0) { log_server("FATAL", strerror(errno)); exit(EXIT_FAILURE); }

    char log_msg_listen[64];
    snprintf(log_msg_listen, sizeof(log_msg_listen), "Listening on port %d...", PORT);
    log_server("INFO", log_msg_listen);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            log_server("ERROR", strerror(errno));
            continue;
        }

        pid_t pid = fork();
        if (pid < 0) {
            log_server("ERROR", "Fork failed for client handler");
            close(client_fd);
        } else if (pid == 0) {
            close(server_fd);
            handle_client(client_fd);
            exit(EXIT_SUCCESS);
        } else {
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}
