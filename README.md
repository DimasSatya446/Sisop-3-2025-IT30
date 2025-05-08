# Sisop-3-2025-IT30
### by IT30


### Anggota Kelompok

| No | Nama                              | NRP         |
|----|-----------------------------------|------------|
| 1  | Adiwidya Budi Pratama            | 5027241012 |
| 2  | Ahmad Rafi Fadhillah Dwiputra     | 5027241068 |
| 3  | Dimas Satya Andhika              | 5027241032 |

---

## soal_1

# “The Legend of Rootkids👑”

## Image Client


**Fungsi reverse**
```c
void reverse(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}
```
`strlen(str)` → menghitung panjang string.

`for (int i = 0; i < len / 2; i++)` → iterasi setengah dari panjang string (karena kita tukar dari dua sisi).

`char tmp = str[i];` → simpan sementara karakter saat ini.

`str[i] = str[len - i - 1];` → tukar karakter di posisi i dengan karakter di posisi dari ujung (len - i - 1).

`str[len - i - 1] = tmp;` → letakkan karakter sementara ke posisi ujung itu.

---

**Fungsi bytes_to_hex()**
```c
void bytes_to_hex(const unsigned char *data, size_t len, char *hex_out) {
    for (size_t i = 0; i < len; i++) {
        sprintf(hex_out + (i * 2), "%02x", data[i]);
    }
    hex_out[len * 2] = '\0';
}
```
`data` adalah array biner (byte).

`hex_out` adalah output berupa string hex.

`sprintf(hex_out + (i * 2), "%02x", data[i]);`
Ini menulis 2 karakter hex untuk tiap byte.

`hex_out[len * 2] = '\0';` → null terminator untuk mengakhiri string.

---

**Fungsi upload_file()**
```c
void upload_file() {
```
Langkah-langkah sintaks di sini:

a) Input nama file
```c
char filename[256];
printf("Masukkan nama file TXT: ");
scanf("%s", filename);
```
→ Baca nama file yang akan di-upload.

b) Gabungkan path folder:
```c
char path[300];
snprintf(path, sizeof(path), "secrets/%s", filename);
```
→ Menyusun path ke folder `secrets/` + nama file.

c) Buka file biner:
```c
FILE *f = fopen(path, "rb");
```
→ `rb` artinya baca file dalam mode biner.

d) Ambil ukuran file:
```c
fseek(f, 0, SEEK_END);
long fsize = ftell(f);
fseek(f, 0, SEEK_SET);
```
- `fseek()` ke akhir file.

- `ftell()` mengambil ukuran file (posisi akhir).

- `fseek()` kembali ke awal file.

e) Alokasikan memori:
```c
unsigned char *binary_data = malloc(fsize);
```
→ Siapkan memori sebesar ukuran file untuk menyimpan data biner.

f) Baca file:
```c
fread(binary_data, 1, fsize, f);
```
→ Baca seluruh file ke binary_data.

g) Convert ke hex:
```c
char *hex_data = malloc(fsize * 2 + 1);
bytes_to_hex(binary_data, fsize, hex_data);
```
→ Gunakan fungsi `bytes_to_hex()` untuk ubah biner ke hex.

h) Balik hex:
```c
reverse(hex_data);
```
→ Membalik string hex tersebut.

i) Socket setup:
```c
int sock = socket(AF_INET, SOCK_STREAM, 0);
```
→ Membuat TCP socket.

j) Siapkan alamat server:
```c
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(PORT);
inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);
```
→ Konfigurasi server: IP + port dalam format `sockaddr_in`.

k) Connect ke server:
```c
connect(sock, (struct sockaddr*)&addr, sizeof(addr));
```
→ Lakukan koneksi TCP ke server.

l) Susun request:
```c
snprintf(request, req_len, "UPLOAD %s\n%s", filename, hex_data);
```
→ Formatnya: `UPLOAD <filename>\n<hex_data>.`

m) Kirim request:
```c
send(sock, request, strlen(request), 0);
```
→ Kirim data ke server lewat socket.

n) Terima respon:
```c
int r = recv(sock, response, sizeof(response) - 1, 0);
```
→ Terima balasan dari server.

o) Tutup socket:
```c
close(sock);
```

---

**Fungsi download_file()**
```c
void download_file() {
```
Langkah-langkahnya:

a) Input nama file JPEG:
```c
char filename[256];
printf("Masukkan nama file JPEG: ");
scanf("%s", filename);
```
b) Buat socket:
```c
int sock = socket(AF_INET, SOCK_STREAM, 0);
```
c) Siapkan alamat server dan connect:
```c
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(PORT);
inet_pton(AF_INET, SERVER_IP, &addr.sin_addr);
connect(sock, (struct sockaddr*)&addr, sizeof(addr));
```
d) Susun request:
```c
snprintf(request, sizeof(request), "DOWNLOAD %s\n", filename);
send(sock, request, strlen(request), 0);
```
→ Format requestnya: `DOWNLOAD <filename>\n`

e) Terima respon awal:
```c
int ir = recv(sock, initial_response, sizeof(initial_response) - 1, 0);
```
Kalau respon bukan `"SUCCESS"`, download dibatalkan.

f) Buka file lokal untuk tulis biner:
```c
FILE *out = fopen(filename, "wb");
```
`wb` = write biner.

g) Loop untuk menerima data file:
```c
while ((bytes_received = recv(sock, data, sizeof(data), 0)) > 0) {
    fwrite(data, 1, bytes_received, out);
    total_received += bytes_received;
}
```
- Terus menerima data sampai `recv()` berhenti.
- Tulis data langsung ke file JPEG lokal.

h) Cek error & tutup file/socket:
```c
fclose(out);
close(sock);
```

---

**Fungsi main()**
```c
int main() {
```
Isi utamanya:

- Menampilkan menu secara loop.

- scanf() baca pilihan user.

- switch memilih aksi:

    1 → jalankan `upload_file()`

    2 → jalankan `download_file()`

    3 → keluar program dengan `exit(0)`
NOTE:

`Socket(), connect(), send(), recv()` → fungsi socket programming khas C.

`malloc(), free()` → alokasi memori manual.

`fread(), fwrite()` → operasi file secara biner.

`snprintf()` → string formatting aman (anti buffer overflow).

`inet_pton()` → mengubah IP string ke format network.

---

## Image Server


**Fungsi log**
```c
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
```
`FILE *log = fopen(LOG_FILE, "a");`
➔ Membuka file log untuk append (`a`), artinya setiap log baru akan ditambahkan di akhir file.

`time_t now = time(NULL);`
➔ Ambil waktu saat ini (epoch time).

`struct tm *t = localtime(&now);`
➔ Konversi waktu epoch menjadi format lokal (`struct tm`).

`strftime(stamp, sizeof(stamp), "%Y-%m-%d %H:%M:%S", t);`
➔ Format waktu menjadi string `YYYY-MM-DD HH:MM:SS`.

fprintf(log, "[Server][%s]: [%s] %s\n", stamp, type, message);
➔ Tulis ke file log dengan format `[Server][waktu]: [tipe] pesan`.

---

**Fungsi daemonize**
```c
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
```
`pid = fork();`
➔ Membuat proses child. Jika parent (pid > 0), parent exit → child lanjut (detach dari terminal).

`sid = setsid();`
➔ Child jadi session leader → benar-benar independen dari terminal.

`chdir("/");`
➔ Ubah direktori kerja jadi root (`/`).

`umask(0);`
➔ Set hak akses file default agar tidak dibatasi.

`close(STDIN_FILENO); close(STDOUT_FILENO); close(STDERR_FILENO);`
➔ Tutup input-output terminal (daemon tidak punya console).

Fork kedua: Supaya tidak bisa mendapatkan terminal kembali.

---

**Fungsi reverse**
```c
void reverse(char *str) {
    int len = strlen(str);
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = tmp;
    }
}
```
`int len = strlen(str);`
➔ Hitung panjang string.

Loop `for (int i = 0; i < len / 2; i++)`
➔ Tukar karakter di awal dan akhir (`str[i]` dan `str[len - i - 1]`).

---

**Hex to byte**
```c
int hex_to_bytes(const char *hex, unsigned char *out) {
    int len = strlen(hex);
    if (len % 2 != 0) return -1;
    for (int i = 0; i < len / 2; i++) {
        if (sscanf(hex + 2 * i, "%2hhx", &out[i]) != 1)
            return -1;
    }
    return len / 2;
}
```
`if (len % 2 != 0) return -1;`
➔ Hex string harus genap (setiap 2 char mewakili 1 byte).

`sscanf(hex + 2 * i, "%2hhx", &out[i]);`
➔ Baca 2 char hex ➔ konversi ke byte.

---

**Fungsi handle client**
```c
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
```
a) Baca perintah dari klien:
```c
int n = read(sock, buf, sizeof(buf) - 1);
```
➔ Baca data dari socket ke buffer.
```c
char *newline = strstr(buf, "\n");
*newline = '\0';
char *command = buf;
char *data = newline + 1;
```
➔ Ambil perintah di baris pertama dan data hex di bawahnya.

b) Upload:

`reverse(data);` ➔ Balik string hex yang dikirim klien.

`hex_to_bytes(data, bin);` ➔ Konversi hex ke byte.

`mkdir(DATABASE_DIR, 0777);` ➔ Pastikan folder database ada.

`strftime(filename, ...)` ➔ Nama file otomatis `YYYYMMDD_HHMMSS.jpeg`.

`fwrite(bin, 1, blen, out);` ➔ Simpan hasil decode ke file.
```c
send(sock, "SUCCESS: filename\n", ...);
```
➔ Balas ke klien bahwa upload sukses.

c) Download
- Validasi nama file
```c
if (strstr(fn, "..") != NULL || strstr(fn, "/") != NULL)
```
➔ Cegah serangan path traversal.

- Buka file
```c
FILE *f = fopen(path, "rb");
```
➔ Baca file dalam mode biner.

- Kirim isi file
```c
while ((bytes_read = fread(file_data, 1, sizeof(file_data), f)) > 0)
    send(sock, file_data, bytes_read, 0);
```
➔ Loop baca file ➔ langsung kirim ke klien.

---

**Fungsi sigchld_handler()**
```c
void sigchld_handler(int sig) {
    while (waitpid(-1, NULL, WNOHANG | WUNTRACED) > 0);
}
```
`waitpid(-1, NULL, WNOHANG | WUNTRACED`
➔ Tunggu semua proses anak yang sudah selesai ➔ bersihkan zombie process.

---

**Main**
```c
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
```
`daemonize();` ➔ Proses server jadi background daemon.

- Pasang sinyal handler:

```c
sigaction(SIGCHLD, &sa, NULL);
```
➔ Tangani child process yang selesai.

Setup socket:

`socket(AF_INET, SOCK_STREAM, 0);` ➔ Buat socket TCP IPv4.

`setsockopt(..., SO_REUSEADDR | SO_REUSEPORT, ...)` ➔ Biar socket bisa dipakai ulang cepat.

`bind()` ➔ Ikat socket ke port 8080.

`listen()` ➔ Mulai dengarkan koneksi masuk.

Loop utama server:
```c
while (1) {
    int client_fd = accept(...);
    pid_t pid = fork();
    if (pid == 0) handle_client(client_fd);
}
```
➔ Untuk setiap klien:

- Buat child process (`fork()`).

- Jalankan `handle_client()` untuk klien tsb.

- Parent lanjut menunggu klien berikutnya.

---

## soal_2
### Perusahaan Ekspedisi *Rush Go*
---
### Problem:

> **RushGo ingin memberikan layanan ekspedisi terbaik dengan 2 pilihan, Express (super cepat) dan Reguler (standar). Namun, pesanan yang masuk sangat banyak! Mereka butuh sebuah sistem otomatisasi pengiriman, agar agen-agen mereka tidak kewalahan menangani pesanan yang terus berdatangan.**

**Terdiri dari 2 sistem utama:**
	**>** `delivery_agent.c` untuk agen otomatis pengantar express.
	**>** `dispatcher.c` untuk pengiriman dan monitoring pesanan oleh user.

- **Mengunduh dan menyimpan *[delivery order](https://drive.google.com/file/d/1OJfRuLgsBnIBWtdRXbRsD2sG6NhMKOg9/view)* ke dalam *Shared Memory*, serta keseluruhan pengelolaan *delivery order* dengan penggunaan *Shared Memory* secara bersama.**
- **Pengiriman tipe *express* dengan 3 agen pengiriman utama: A, B, dan C, yang berjalan di tiap *thread* terpisah dan secara otomatis tanpa intervensi *user* pada datanya pada *Shared Memory* dalam `./delivery_agent`.**
- **Pengiriman tipe *reguler* dengan pengiriman secara manual dilakukan oleh user  dari `./dispatcher`, serta pengiriman dilaksanakan oleh `Agent[User]`.**
- **Program akan mencatat *log* dalam `~/delivery.log` pada kedua tipe pengiriman (*express* dan *reguler*) dengan format yang telah ditentukan setelah pengiriman sukses dilakukan.**
- **Program `./dispatcher` dapat mengecek status setiap pengiriman.**
- **Program `./dispatcher` dapat memperlihatkan keseluruhan pengiriman dalam format list disertai nama dan statusnya.**

**Sebagai efisiensi line, pada soal ini penjelasan `source code` pada beberapa *line code* disingkat dengan `[...]`, sehingga tidak memakan banyak line yang ada. Kejelasan line yang sudah pasti / FIX telah di bentuk dan dirancang pada folder soal yang sudah disediakan / ada. Terimakasih 🙇‍♂️🙏**

---

### Code's Key Components
> **`delivery_agent.c`**

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <time.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
    
    #define SHM_KEY 1234
    #define MAX_ORDERS 100
    #define NAME_LENGTH 64
    #define ADDR_LENGTH 128
    
    typedef enum { PENDING, DELIVERED } Status;
    typedef enum { EXPRESS, REGULER } Type;
    
    typedef struct {
        char name[NAME_LENGTH];
        char address[ADDR_LENGTH];
        Type type;
        Status status;
        char agent[NAME_LENGTH];
    } Order;
    
    typedef struct {
        Order orders[MAX_ORDERS];
        int count;
    } SharedData;

Menjadi *library*,*struct*, *define*, dan konfigurasi *shared memory* yang akan digunakan sebagai struktur data dalam data *delivery_order* pada sistem.

        void log_delivery(const char *agent, const Order *o) {
            FILE *log = fopen("delivery.log", "a");
            time_t now = time(NULL);
            [...]
                fclose(log);
	    }
Berfungsi dalam *logging* pada setiap pengiriman, khususnya yang bertipe *express* (pada program `delivery_agent.c`) dengan:

-    Membuka file `delivery.log` (mode append).
    
-   Mendapatkan waktu lokal saat ini.
    
-   Menuliskan log dalam format:  
    `[dd/mm/yyyy hh:mm:ss] [AGENT X (pada kasus ini A/B/C)] Express package delivered to [Nama] in [Alamat]`

```
    void *agent_thread(void *arg) {
        char *agent_name = (char *)arg;
    
        while (1) {
            for (int i = 0; i < data->count; ++i) {
                if (data->orders[i].type == EXPRESS && data->orders[i].status == PENDING) {
                [...]
        return NULL;
    }
```
Berfungsi sebagai *thread* masing-masing agent (A/B/C) dengan mekanisme sebagai berikut:
-   `arg` adalah pointer ke nama agen (`A`, `B`, `C`).
    
-   Infinite loop:
    
    -   Iterasi semua order (`data->orders`).
        
    -   Jika ketemu order dengan:
        
        -   `type == EXPRESS`
            
        -   `status == PENDING`
            
    -   Maka:
        
        -   ubah status menjadi `DELIVERED`
            
        -   simpan nama agen
            
        -   log ke file
            
        -   `sleep(1)` untuk simulasi antar
            
    -   `sleep(3)` setelah 1 iterasi penuh untuk efisiensi CPU.

```
int main() {
    int shmid = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    data = (SharedData *)shmat(shmid, NULL, 0);
    if (data == (void *) -1) {
        perror("shmat");
        return 1;
    }
    [...]
        shmdt(data);
    return 0;
}
```
Berfungsi sebagai fungsi utama dalam code yang secara singkat bekerja dengan mekanisme sebagai berikut:

-   *Shared memory* yang berisi data *delivery_order* diakses dan dibaca.
    
-   3 thread agen dibuat: `"A"`, `"B"`, `"C"`.
    
-   Tiap agen:
    -   Scan daftar order.
    -   Kirim semua pengiriman bertipe `[EXPRESS-PENDING]`.
    -   Mencatat Log hasil pengiriman yang sukses dilaksanakan.
    -   Ulangi terus selamanya.

> **`dispatcher.c`**
```
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <time.h>
    #include <sys/ipc.h>
    #include <sys/shm.h>
    
    #define SHM_KEY 1234
    #define MAX_ORDERS 100
	
	[...]
    
    typedef struct {
        Order orders[MAX_ORDERS];
        int count;
    } SharedData;
```
Sama seperti program sebelumnya, yaitu menjadi *library*,*struct*, *define*, dan konfigurasi *shared memory* yang akan digunakan sebagai struktur data dalam data *delivery_order* pada sistem.

```
void download_csv_if_needed() {
    if (access("delivery_order.csv", F_OK) == -1) {
        printf("Downloading delivery_order.csv...\n");
        system("wget -q -O delivery_order.csv \"https://drive.google.com/uc?export=download&id=1OJfRuLgsBnIBWtdRXbRsD2sG6NhMKOg9\"");
        printf("Download completed.\n");
    }
}
```
Berfungsi dalam akan mengecek jika file *delivery_order.csv* sudah ada pada progam dan mendownload file *delivery_order.csv* jika belum ada sebagai source data untuk setiap pengiriman.

```
void load_orders_from_csv(SharedData *data) {
    FILE *fp = fopen("delivery_order.csv", "r");
    if (!fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        char *name = strtok(line, ",");
        char *addr = strtok(NULL, ",");
        char *type = strtok(NULL, "\n");
    [...]
    
        }
    fclose(fp);
}
```
Berfungsi dalam mengakses data *delivery_order* dari *shared memory* dengan mekanisme sebagai berikut:
-   Membuka `delivery_order.csv`.
    
-   Membaca file baris per baris dengan batasan;
```
char *name = strtok(line, ","); 
char *addr = strtok(NULL, ","); 
char *type = strtok(NULL, "\n");
```
-   Mengisi elemen `Order` berdasarkan data yang dibaca.
    
-   Jika `type` mengandung "Express", maka `o->type = EXPRESS`.

```
void log_delivery(const char *agent, const Order *o, const char *type) {
    FILE *log = fopen("delivery.log", "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d/%02d/%d %02d:%02d:%02d] [AGENT %s] %s package delivered to %s in %s\n",
    [...]
    
    fclose(log);
}
```
Sama dengan fungsi pada program sebelumnya dengan fungsi dalam *logging* pada setiap pengiriman, khususnya yang bertipe *reguler* (pada program `dispatcher.c`) dengan:

-    Membuka file `delivery.log` (mode append).
    
-   Mendapatkan waktu lokal saat ini.
    
-   Menuliskan log dalam format:  
    `[dd/mm/yyyy hh:mm:ss] [AGENT X (pada kasus ini A/B/C)] Express package delivered to [Nama] in [Alamat]`

```
void deliver_reguler(SharedData *data, const char *target_name) {
    for (int i = 0; i < data->count; ++i) {
        if (data->orders[i].type == REGULER &&
            data->orders[i].status == PENDING &&
            strcmp(data->orders[i].name, target_name) == 0) {
            [...]
            
    printf("Order tidak ditemukan atau telah dikirim\n");
}
```
Berfungsi dalam mengirim pengiriman bertipe *Reguler* secara manual, dengan mekanisme sebagai berikut:
-   Mencari order dengan:
    
    -   Bertipe `REGULER` dengan status `PENDING`
        
    -   Nama pelanggan sesuai `target_name`
        
-   Jika ketemu:
    
    -   Ubah status jadi `DELIVERED`
        
    -   Isi field `agent` dengan `target_name` (simbolis sebagai pengantar)
        
    -   Mencatat Log pengiriman setelah sukses dikirim
        
-   Jika tidak:
    
    -   Cetak pesan error

```
void check_status(SharedData *data, const char *target_name) {
    for (int i = 0; i < data->count; ++i) {
        if (strcmp(data->orders[i].name, target_name) == 0) {
            if (data->orders[i].status == DELIVERED)
            [...]
            
    }
    printf("Order not found.\n");
}
```
Berfungsi dalam mengecek *status* setiap pengiriman, dengan mekanisme sebagai berikut:

-   Mencari pesanan dengan nama `target_name`.
    
-   Jika ketemu, dengan format yang telah diberikan:
    
    -   Status `DELIVERED` → tampilkan nama agen
        
    -   Status `PENDING` → tampilkan belum terkirim
        
-   Jika tidak ada → tampilkan pengiriman tidak ditemukan

```
void list_all(SharedData *data) {
    for (int i = 0; i < data->count; ++i) {
        printf("Order %d: %s - %s\n", i+1, data->orders[i].name,
            data->orders[i].status == DELIVERED ? "Delivered" : "Pending");
    }
}
```
Berfungsi untuk menampilkan keseluruhan pesanan dengan statusnya dengan melalui keseluruhan data pada *shared memory*.

```
int main(int argc, char *argv[]) {
    int shmid = shmget(SHM_KEY, sizeof(SharedData), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    SharedData *data = (SharedData *)shmat(shmid, NULL, 0);
    if (data == (void *) -1) {
        perror("shmat");
        return 1;
    }
    [...]

    shmdt(data);
    return 0;
}
```
Berfungsi sebagai fungsi/program utama dalam `dispatcher.c`, dengan mekanisme sebagai berikut:

-   Akses shared memory → `shmget` & `shmat`
-   Panggil `download_csv_if_needed()` → Mengunduh file CSV jika belum ada.
-   Jika `data->count == 0` → muat data dari CSV untuk data *`delivery_order`*.
-   Proses argumen:
    -   `-deliver [nama]` → Mengantar order bertipe *Reguler*.
 
    -   `-status [nama]` → Mengecek status suatu pengiriman.

    -   `-list` → Menampilkan semua order pengiriman yang ada.
-   Jika salah argumen → tampilkan format usage.
    
-   Detach shared memory (`shmdt`).

---
## soal_3

### The Lost Dungeon
[Author: Fico / purofuro]

> Suatu pagi, anda menemukan jalan setapak yang ditumbuhi lumut dan hampir tertutup semak. Rasa penasaran membawamu mengikuti jalur itu, hingga akhirnya anda melihatnya: sebuah kastil tua, tertutup akar dan hampir runtuh, tersembunyi di tengah hutan. Gerbangnya terbuka seolah memanggilmu masuk.
> Di dalam, anda menemukan pintu batu besar dengan simbol-simbol aneh yang terasa… hidup. Setelah mendorongnya dengan susah payah, anda pun menuruni tangga batu spiral yang dalam dan gelap. Di ujungnya, anda menemukan sebuah dunia baru: dungeon bawah tanah yang sudah tertinggal sangat lama.
> Anda tidak tahu bagaimana anda dapat berada di situasi ini, tetapi didorong oleh rasa ingin tahu dan semangat, apa pun yang menunggumu di bawah sana, anda akan melawan.

a. **Entering the dungeon** 
> dungeon.c akan bekerja sebagai server yang dimana client (player.c) dapat terhubung melalui RPC. dungeon.c akan memproses segala perintah yang dikirim oleh player.c. Lebih dari 1 client dapat mengakses server.
**Client**
```bash
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define MAX_DATA 2048

void print_main_menu() {
    printf("\n");
    ...
    printf(CYAN ">> Choose your fate, adventurer: " RESET);
}


int main() {
    ...
 return 0;
}
```
**Server**
```bash
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

#define PORT 8888
#define BUFFER_SIZE 2048
#define MAX_INVENTORY 10
#define MAX_PASSIVE 2

typedef struct {
    ...
} Playerstats

...

void* handle_client(void* arg) {
    int new_socket = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];
    ...
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    printf("[DUNGEON SERVER] Listening on port %d...\n", PORT);

    init_shop(); // Inisialisasi toko senjata

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        int* pclient = malloc(sizeof(int));
        *pclient = new_socket;
        pthread_t t;
        pthread_create(&t, NULL, handle_client, pclient);
        pthread_detach(t);
    }

    return 0;
}
```

b. **Sightseeing**
> Anda melihat disekitar dungeon dan menemukan beberapa hal yang menarik seperti toko senjata dan pintu dengan aura yang cukup seram. Ketika player.c dijalankan, ia akan terhubung ke dungeon.c dan menampilkan sebuah main menu seperti yang dicontohkan di bawah ini (tidak harus mirip, dikreasikan sesuai kreatifitas masing-masing praktikan).

![image](https://github.com/user-attachments/assets/a66fbf14-d529-462f-bb3d-00ff86127b0a)

```
void print_main_menu() {
    printf("\n");
    printf(WHITE "╔══════════════════════════════════════╗\n" RESET);
    printf(WHITE "║" CYAN "      🕯️  DUNGEON MAIN MENU   🕯️      " WHITE "║\n" RESET);
    printf(WHITE "╠══════════════════════════════════════╣\n" RESET);
    printf(WHITE "║" YELLOW " 1. 📖 Examine Hero Status            " WHITE "║\n");
    printf(WHITE "║" GREEN  " 2. 🪙 Visit the Blacksmith           " WHITE "║\n");
    printf(WHITE "║" CYAN   " 3. 🎒 Manage Inventory & Gear        " WHITE "║\n");
    printf(WHITE "║" RED    " 4. ⚔️  Enter Battle Arena            " WHITE "║\n");
    printf(WHITE "║" MAGENTA " 5. 🚪 Leave the Dungeon              " WHITE "║\n");
    printf(WHITE "╚══════════════════════════════════════╝\n" RESET);
    printf(CYAN ">> Choose your fate, adventurer: " RESET);
}


int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_DATA];
    int choice;

    while (1) {
        print_main_menu();
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice) {
            case 1: {
                const char* cmd = "SHOW_STATS";
                send(sock, cmd, strlen(cmd), 0);

                // Receive response
                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("\n%s\n", buffer);
                break;
            }

            case 2: {
                // Send command to enter shop
                const char* cmd = "SHOP";
                send(sock, cmd, strlen(cmd), 0);

                ...

                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("%s\n", buffer);
                break;
            }

            case 3: {
                // Send command to view inventory
                const char* cmd = "INVENTORY";
                send(sock, cmd, strlen(cmd), 0);

                ...
                break;
            }

            case 4: {
                const char* cmd = "BATTLE";
                send(sock, cmd, strlen(cmd), 0);
            
                while (1) {
                    ...
                    }
                }
            
                break;
            }

            case 5:
                printf("Goodbye !\n");
                close(sock);
                exit(0);

            default:
                printf("Invalid choice. Please try again.\n");
        }
    }
```
c. **Status Check**
> Melihat bahwa terdapat sebuah toko senjata, anda mengecek status diri anda dengan harapan anda masih memiliki sisa uang untuk membeli senjata. Jika opsi Show Player Stats dipilih, maka program akan menunjukan Uang yang dimiliki (Jumlah dibebaskan), senjata yang sedang digunakan, Base Damage, dan jumlah musuh yang telah dimusnahkan.

![image](https://github.com/user-attachments/assets/e8d63cc6-f990-4187-9380-cf1526c384c7)
**Client**
```
case 1: {
                // Send command
                const char* cmd = "SHOW_STATS";
                send(sock, cmd, strlen(cmd), 0);

                // Receive response
                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("\n%s\n", buffer);
                break;
            }
```
**Server**
```
typedef struct {
    int gold;
    Weapon inventory[MAX_INVENTORY];
    int inventory_size;
    Weapon equipped;
    int base_damage;
    int kills;
} PlayerStats;
...

void* handle_client(void* arg) {
    int new_socket = *(int*)arg;
    free(arg);
    char buffer[BUFFER_SIZE];

    Weapon fists = {
        .id = 0,
        .damage = 5,
        .price = 0,
    };
    strcpy(fists.name, "Fists");

    PlayerStats player = {
        .gold = 500,
        .inventory_size = 1,
        .base_damage = 5,
        .kills = 0
    };
    player.inventory[0] = fists;
    player.equipped = fists;

    srand(time(NULL));

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int valread = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
        if (valread <= 0) {
            printf("[DEBUG] Client disconnected or recv error\n");
            break;
        }
        buffer[valread] = '\0'; // Null-terminate buffer

        if (strcmp(buffer, "SHOW_STATS") == 0) {
            char stats_response[BUFFER_SIZE];
            snprintf(stats_response, BUFFER_SIZE,
                     "=== PLAYER STATS ===\nGold: %d | Equipped Weapon: %s | Base Damage: %d | Kills: %d\n",
                     player.gold, player.equipped.name, player.base_damage, player.kills);

            if (strlen(player.equipped.passive[0]) > 0 || strlen(player.equipped.passive[1]) > 0) {
                strcat(stats_response, "\nPassive: ");
                strcat(stats_response, player.equipped.passive[0]);
                strcat(stats_response, ", ");
                strcat(stats_response, player.equipped.passive[1]);
            }

            send(new_socket, stats_response, strlen(stats_response), 0);

        } else if ...
```
---
## soal_4
a


---
## Revisi
### soal_4
a
