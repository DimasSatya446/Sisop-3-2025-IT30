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

---

> **Sebagai efisiensi line, pada soal ini penjelasan `source code` pada beberapa *line code* disingkat dengan `[...]`, sehingga tidak memakan banyak line yang ada. Kejelasan line yang sudah pasti / FIX telah di bentuk dan dirancang pada folder soal yang sudah disediakan / ada. Terimakasih 🙇‍♂️🙏**

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
- `#define SERVER_IP "127.0.0.1"`
Mendefinisikan alamat IP server sebagai localhost (komputer sendiri).
- `#define SERVER_PORT 8888`
Menetapkan port server tempat client akan terhubung, dalam hal ini port 8888
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
- `PORT: `Port koneksi ke server.
- `BUFFER_SIZE: `Buffer untuk komunikasi.
- `int sock;
struct sockaddr_in server;
char buffer[BUFFER_SIZE];
sock = socket(AF_INET, SOCK_STREAM, 0);` Membuat socket TCP.
- `server.sin_family = AF_INET;
server.sin_port = htons(PORT);
server.sin_addr.s_addr = inet_addr("127.0.0.1");` Konfigurasi alamat server: localhost, port 8888.
- `connect(sock, (struct sockaddr*)&server, sizeof(server));` Menghubungkan socket client ke server dungeon.

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
- `int sock = socket(AF_INET, SOCK_STREAM, 0);`
Membuat socket TCP (SOCK_STREAM) untuk koneksi client-server.

- `AF_INET: menggunakan protokol IPv4.
if (sock < 0) { ... }`
Mengecek apakah socket berhasil dibuat. Jika gagal, tampilkan error dan keluar.
- `struct sockaddr_in server_addr;`
Struktur alamat untuk koneksi ke server.

- `server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(SERVER_PORT);
inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);`
Mengatur alamat IP dan port server (127.0.0.1:8888) dalam bentuk yang dimengerti oleh socket.

- `if (connect(...) < 0) { ... }`
Melakukan koneksi ke server. Jika gagal, socket ditutup dan program keluar.

- `while (1) {
    print_main_menu();
    scanf("%d", &choice);
    getchar(); // menyerap newline`
Menampilkan menu dan menerima input angka dari user.

- `getchar() `menyerap karakter newline (\n) setelah scanf.
- `case 1:
    const char* cmd = "SHOW_STATS";
    send(sock, cmd, strlen(cmd), 0);
    recv(sock, buffer, MAX_DATA, 0);
    printf("%s\n", buffer);
Kirim perintah "SHOW_STATS" ke server.`

Server membalas dengan status pemain yang kemudian ditampilkan.
- `case 2:
    const char* cmd = "SHOP";
    send(sock, cmd, strlen(cmd), 0);
    ...
    recv(sock, buffer, MAX_DATA, 0);
    printf("%s\n", buffer);`
Kirim perintah "SHOP" ke server.

- Menerima dan menampilkan respon dari server (misalnya daftar senjata atau toko).
- `case 3:
    const char* cmd = "INVENTORY";
    send(sock, cmd, strlen(cmd), 0);
    ...`
Kirim perintah "INVENTORY" ke server.

- Kode recv() dan tampilan hasil kemungkinan akan ditambahkan kemudian.
- `case 4:
    const char* cmd = "BATTLE";
    send(sock, cmd, strlen(cmd), 0);

    while (1) {
        ...
    }`
Kirim perintah "BATTLE" ke server. Masuk loop battle mode

- `case 5:
    printf("Goodbye !\n");
    close(sock);
    exit(0);`
Menutup socket dan keluar dari game.
`default:
    printf("Invalid choice. Please try again.\n");`
Menangani input selain 1–5. error handling

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

- `const char* cmd = "SHOW_STATS";`Client menyiapkan string perintah "SHOW_STATS".

- `send(sock, cmd, strlen(cmd), 0);`Mengirimkan string tersebut ke server melalui koneksi socket (sock).

- `memset(buffer, 0, MAX_DATA);`Mengosongkan buffer untuk menghindari data sisa dari komunikasi sebelumnya.

- `recv(sock, buffer, MAX_DATA, 0);`Menerima balasan dari server dan menyimpannya ke dalam buffer.

- `printf("\n%s\n", buffer);`Menampilkan isi balasan tersebut ke layar terminal user (berisi status player).
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
- Sebelum server dapat merespons perintah SHOW_STATS, ia menggunakan struktur data berikut untuk menyimpan status pemain:
`typedef struct {
    int gold;                          // Jumlah emas yang dimiliki pemain
    Weapon inventory[MAX_INVENTORY];   // Daftar senjata milik pemain
    int inventory_size;                // Jumlah senjata yang dimiliki
    Weapon equipped;                   // Senjata yang sedang digunakan
    int base_damage;                   // Damage dasar pemain (belum termasuk senjata)
    int kills;                         // Jumlah musuh yang dikalahkan
} PlayerStats;`
- Inisialisasi Default di handle_client; 
Setiap client yang terhubung akan memiliki instance PlayerStats sendiri, diinisialisasi dengan nilai default:
`Weapon fists = {
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
player.equipped = fists; `
- Pemain baru dimulai dengan 500 gold, satu senjata awal: Fists, dan belum punya kill.

- Saat server menerima string "SHOW_STATS" dari client, ia menjalankan blok ini:
`if (strcmp(buffer, "SHOW_STATS") == 0) {
    char stats_response[BUFFER_SIZE];
    snprintf(stats_response, BUFFER_SIZE,
             "=== PLAYER STATS ===\nGold: %d | Equipped Weapon: %s | Base Damage: %d | Kills: %d\n",
             player.gold, player.equipped.name, player.base_damage, player.kills);`
Langkah-langkahnya:
- Validasi Perintah

- Server membandingkan string buffer dengan "SHOW_STATS".

- Server membentuk string berisi:

Gold pemain, 

Nama senjata yang sedang digunakan, 

Base damage pemain, 

Jumlah kills.

- Jika senjata yang digunakan memiliki efek pasif, maka informasi tambahan juga dikirim:
` if (strlen(player.equipped.passive[0]) > 0 || strlen(player.equipped.passive[1]) > 0) {
    strcat(stats_response, "\nPassive: ");
    strcat(stats_response, player.equipped.passive[0]);
    strcat(stats_response, ", ");
    strcat(stats_response, player.equipped.passive[1]);
}`
Server akan menambahkan bagian "Passive: ..." ke respons jika ada efek pasif yang terisi.
- Pengiriman Balasan
`send(new_socket, stats_response, strlen(stats_response), 0);
Server mengirim string stats_response ke client melalui new_socket.`

d. Weapon Shop
> Ternyata anda memiliki sisa uang dan langsung pergi ke toko senjata tersebut untuk membeli senjata. Terdapat 5 pilihan senjata di toko tersebut dan beberapa dari mereka memiliki passive yang unik. Disaat opsi Shop dipilih, program akan menunjukan senjata apa saja yang dapat dibeli beserta harga, damage, dan juga passive (jika ada). List senjata yang ada dan dapat dibeli beserta logic/command untuk membeli senjata tersebut diletakan di code shop.c/shop.h yang nanti akan dipakai oleh dungeon.c.
- Import shop.c sebagai bagian dari kompilasi di server
`#include "shop.c"`
- `#define MAX_PASSIVE 2       // Jumlah maksimum efek pasif senjata
#define SHOP_SIZE 5         // Total senjata tersedia di toko
#define BUFFER_SIZE 2048    // Ukuran buffer untuk output string`
- `typedef struct {
    int id;
    char name[32];
    int price;
    int damage;
    char passive[MAX_PASSIVE][64];
} Weapon;` untuk menyimpan data weapon
- `void init_shop()`
Menginisialisasi daftar senjata di weapon_shop dengan 5 jenis senjata:
1. Rusty Sword – Bleed, Low durability
2. Battle Axe – Bleed, Slow attack
3. Fire Wand – Burn, Mana drain
4. Twin Daggers – Double hit, Poison
5. Staff of Homa – Double hit, Burn

- `void display_weapon_shop(char* buffer)`
Menghasilkan tampilan tabel senjata dalam bentuk string berwarna yang dapat dikirim ke client atau dicetak ke terminal.
- `void format_weapon_display(const Weapon* w, char* output, int with_passive)`
Menghasilkan representasi string dari senjata.
-- Jika with_passive == 1, maka string akan menyertakan passive effect.
-- Jika with_passive == 0, hanya nama senjata yang ditampilkan.
- `int get_weapon_by_id(int id, Weapon* result)`
Mencari senjata berdasarkan ID.
Return 1 jika senjata ditemukan dan data disalin ke result.
Return 0 jika tidak ditemukan.

e. Handy Inventory
> Setelah membeli senjata di toko tadi, anda membuka ransel anda untuk memakai senjata tersebut. Jika opsi View Inventory dipilih, program akan menunjukan senjata apa saja yang dimiliki dan dapat dipakai (jika senjata memiliki passive, tunjukan juga passive tersebut). Lalu apabila opsi Show Player Stats dipilih saat menggunakan weapon maka Base Damage player akan berubah dan jika memiliki passive, maka akan ada status tambahan yaitu Passive.

**Server**
```else if (strcmp(buffer, "INVENTORY") == 0) {
            char inv[BUFFER_SIZE] = "=== YOUR INVENTORY ===\n";
            char temp[256];
            for (int i = 0; i < player.inventory_size; i++) {
                format_weapon_display(&player.inventory[i], temp, is_same_weapon(&player.equipped, &player.inventory[i]));
                strcat(inv, "[");
                char idx[4];
                sprintf(idx, "%d", i);
                strcat(inv, idx);
                strcat(inv, "] ");
                strcat(inv, temp);
                strcat(inv, "\n");
	}
            send(new_socket, inv, strlen(inv), 0);
            memset(buffer, 0, BUFFER_SIZE);
            recv(new_socket, buffer, BUFFER_SIZE, 0);
            int idx = atoi(buffer);
            if (idx >= 0 && idx < player.inventory_size) {
                player.equipped = player.inventory[idx];
                player.base_damage = player.equipped.damage;
                send(new_socket, "Weapon equipped!", 17, 0);
            } else {
                send(new_socket, "Invalid index!", 15, 0);
            }
        }
```
- `else if (strcmp(buffer, "INVENTORY") == 0) {`
Server memeriksa apakah perintah yang diterima dari client adalah "INVENTORY". Jika ya, server akan menampilkan daftar inventaris pemain.

Format Tampilan Inventaris:
```
char inv[BUFFER_SIZE] = "=== YOUR INVENTORY ===\n";
char temp[256];
for (int i = 0; i < player.inventory_size; i++) {
    format_weapon_display(&player.inventory[i], temp, is_same_weapon(&player.equipped, &player.inventory[i]));
    strcat(inv, "[");
    char idx[4];
    sprintf(idx, "%d", i);
    strcat(inv, idx);
    strcat(inv, "] ");
    strcat(inv, temp);
    strcat(inv, "\n");
}
```
- Server memformat tampilan inventaris pemain. Dimulai dengan string header "=== YOUR INVENTORY ===", kemudian untuk setiap senjata dalam inventaris pemain, informasi tentang senjata tersebut diformat menggunakan fungsi format_weapon_display dan ditambahkan ke dalam string inv.

- Setiap senjata akan ditampilkan dengan indeks (misalnya, [0], [1], dll.) dan deskripsi senjata.

- Mengirim Inventaris ke Client:
`send(new_socket, inv, strlen(inv), 0);`
Setelah format inventaris selesai, server mengirimkan informasi inventaris tersebut ke client menggunakan send().

- Mengambil Pilihan Senjata dari Client:
`memset(buffer, 0, BUFFER_SIZE);
recv(new_socket, buffer, BUFFER_SIZE, 0);
int idx = atoi(buffer);`
Server kemudian menunggu untuk menerima input dari client yang berisi indeks senjata yang ingin dipilih untuk dipasang (equip).

- Validasi dan Penambahan Senjata:

`if (idx >= 0 && idx < player.inventory_size) {
    player.equipped = player.inventory[idx];
    player.base_damage = player.equipped.damage;
    send(new_socket, "Weapon equipped!", 17, 0);
} else {
    send(new_socket, "Invalid index!", 15, 0);
}`
- Jika indeks yang dimasukkan valid (berada dalam rentang inventaris), senjata tersebut dipasang (equip) oleh server, dan informasi senjata yang dipasang dikirimkan kembali ke client.
- Jika indeks tidak valid, server mengirimkan pesan kesalahan "Invalid index!".
**Client**
```
case 3: {
                // Send command to view inventory
                const char* cmd = "INVENTORY";
                send(sock, cmd, strlen(cmd), 0);

                // Receive inventory list
                memset(buffer, 0, MAX_DATA);
                recv(sock, buffer, MAX_DATA, 0);
                printf("\n%s", buffer);

                // Read weapon index input
                int weapon_idx;
                printf("Enter weapon index to equip (or invalid number to cancel): ");
                if (scanf("%d", &weapon_idx) == 1) {
                    getchar(); // consume newline
                    char idx_str[8];
                    snprintf(idx_str, sizeof(idx_str), "%d", weapon_idx);
                    send(sock, idx_str, strlen(idx_str), 0);

                    // Receive equip confirmation
                    memset(buffer, 0, MAX_DATA);
                    recv(sock, buffer, MAX_DATA, 0);
                    printf("%s\n", buffer);
                } else {
                    getchar(); // consume invalid input
                    printf("Invalid input.\n");
                }
                break;
            }
```
- Mengirim Perintah untuk Melihat Inventaris:
`const char* cmd = "INVENTORY";
send(sock, cmd, strlen(cmd), 0);`
Client mengirimkan perintah "INVENTORY" ke server untuk meminta daftar inventaris.

- Menerima dan Menampilkan Inventaris:
`memset(buffer, 0, MAX_DATA);
recv(sock, buffer, MAX_DATA, 0);
printf("\n%s", buffer);`
Setelah mengirim perintah, client menerima data inventaris dari server dan menampilkannya di layar.

- Memasukkan Indeks Senjata yang Akan Dipasang:
`int weapon_idx;
printf("Enter weapon index to equip (or invalid number to cancel): ");
if (scanf("%d", &weapon_idx) == 1) {
    getchar(); // consume newline
    char idx_str[8];
    snprintf(idx_str, sizeof(idx_str), "%d", weapon_idx);
    send(sock, idx_str, strlen(idx_str), 0);
}`
Client meminta input dari pengguna untuk memilih senjata yang ingin dipasang dari inventaris. Pengguna diminta untuk memasukkan indeks senjata (misalnya, 0, 1, dll.).

- Input tersebut kemudian dikirimkan ke server sebagai string.

- Menerima Konfirmasi atau Pesan Kesalahan:
`memset(buffer, 0, MAX_DATA);
recv(sock, buffer, MAX_DATA, 0);
printf("%s\n", buffer);`
Client menerima konfirmasi atau pesan kesalahan dari server, yang memberi tahu apakah senjata berhasil dipasang atau jika ada masalah (misalnya, indeks tidak valid).

f. Enemy Encounter
> Anda sekarang sudah siap untuk melewati pintu yang seram tadi, disaat anda memasuki pintu tersebut, anda langsung ditemui oleh sebuah musuh yang bukan sebuah manusia. Dengan tekad yang bulat, anda melawan musuh tersebut. Saat opsi Battle Mode dipilih, program akan menunjukan health-bar musuh serta angka yang menunjukan berapa darah musuh tersebut dan menunggu input dengan opsi attack untuk melakukan sebuah serangan dan juga exit untuk keluar dari Battle Mode. Apabila darah musuh berkurang, maka health-bar musuh akan berkurang juga.
> Jika darah musuh sudah 0, maka program akan menunjukan rewards berupa berapa banyak gold yang didapatkan lalu akan muncul musuh lagi.
> Other Battle Logic :
- Health & Rewards
Untuk darah musuh, seberapa banyak darah yang mereka punya dibuat secara random, contoh: 50-200 HP. Lakukan hal yang sama untuk rewards. 
- Damage Equation
Untuk damage, gunakan base damage sebagai kerangka awal dan tambahkan rumus damage apapun (dibebaskan, yang pasti perlu random number agar hasil damage bervariasi). Lalu buatlah logic agar setiap serangan memiliki kesempatan untuk Critical yang membuat damage anda 2x lebih besar.
- Passive
Jika senjata yang dipakai memiliki Passive setiap kali passive tersebut menyala, maka tunjukan bahwa passive tersebut aktif.

**Client**
```
case 4: {
                const char* cmd = "BATTLE";
                send(sock, cmd, strlen(cmd), 0);
            
                while (1) {
                    // tunggu battle message server
                    memset(buffer, 0, MAX_DATA);
                    int valread = recv(sock, buffer, MAX_DATA - 1, 0);
                    if (valread <= 0) {
                        printf("Connection lost!\n");
                        break;
                    }
                    buffer[valread] = '\0'; // Null-terminate
            
                    printf("\n%s\n", buffer);
            
                    // Wait for input
                    printf("> ");
                    char battle_cmd[MAX_DATA];
                    fgets(battle_cmd, sizeof(battle_cmd), stdin);
                    battle_cmd[strcspn(battle_cmd, "\n")] = '\0'; // Remove newline
            
                    // Send input to server
                    send(sock, battle_cmd, strlen(battle_cmd), 0);
            
                    // Exit battle loop kalau ketik 'exit'
                    if (strcmp(battle_cmd, "exit") == 0) {
                        break;
                    }
                }
            
                break;
            }
```
- Mengirim Perintah "BATTLE" ke Server:
`const char* cmd = "BATTLE";
send(sock, cmd, strlen(cmd), 0);`
Client mengirimkan perintah "BATTLE" ke server untuk memulai pertempuran. Server akan merespons dengan informasi terkait pertempuran, seperti musuh yang muncul.
- Menerima Pesan Pertempuran dari Server:
`while (1) {
    memset(buffer, 0, MAX_DATA);
    int valread = recv(sock, buffer, MAX_DATA - 1, 0);
    if (valread <= 0) {
        printf("Connection lost!\n");
        break;
    }
    buffer[valread] = '\0'; // Null-terminate
    printf("\n%s\n", buffer);`
Client menunggu pesan dari server yang berisi informasi tentang pertempuran. Misalnya, server memberi tahu bahwa pertempuran dimulai dan musuh muncul. Jika koneksi terputus, client akan menampilkan pesan "Connection lost!" dan keluar dari pertempuran.

- Menunggu Input dari Pengguna:
`printf("> ");
char battle_cmd[MAX_DATA];
fgets(battle_cmd, sizeof(battle_cmd), stdin);
battle_cmd[strcspn(battle_cmd, "\n")] = '\0'; // Remove newline`
Setelah menerima pesan dari server, client menunggu input dari pengguna untuk memilih aksi dalam pertempuran, seperti menyerang musuh atau keluar dari pertempuran.

- Mengirim Perintah ke Server:
`send(sock, battle_cmd, strlen(battle_cmd), 0);`
Setelah pengguna memasukkan perintah (misalnya "attack" atau "exit"), perintah tersebut dikirimkan ke server.

- Mengakhiri Pertempuran jika Menggunakan Perintah "exit":
`if (strcmp(battle_cmd, "exit") == 0) {
    break;
}`
Jika pengguna mengetikkan perintah "exit", maka client akan keluar dari loop pertempuran dan menyelesaikan sesi pertempuran.
**Server**
```
else if (strcmp(buffer, "BATTLE") == 0) {
            while (1) {
                // Membuat musuh baru setiap kali loop dimulai
                int enemy_max_hp = rand() % 30 + 40;
                int enemy_hp = enemy_max_hp;
                char msg[BUFFER_SIZE];
                snprintf(msg, BUFFER_SIZE,
                         "=== BATTLE STARTED ===\nEnemy appeared with: ");
                char bar[64];
                display_health_bar(bar, enemy_hp, enemy_max_hp);
                strcat(msg, bar);
                strcat(msg, "\nType 'attack' to attack or 'exit' to leave battle.");
                send(new_socket, msg, strlen(msg), 0);
        
                while (enemy_hp > 0) {
                    memset(buffer, 0, BUFFER_SIZE);
                    int recv_len = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
                    if (recv_len <= 0) {
                        printf("[DEBUG] Client disconnected or recv error during battle\n");
                        goto exit_battle;
                    }
                    buffer[recv_len] = '\0';
        
                    if (strcmp(buffer, "attack") == 0) {
                        int base = player.base_damage;
                        int variance = (base / 2 > 0) ? base / 2 : 1;
                        int damage = base + rand() % (variance + 1);
                        int is_crit = rand() % 100 < 20;
                        if (is_crit) damage *= 2;
        
                        char passive_effect[256] = "";
                        int passive_triggered = 0;
        
                        // Bleed (25% chance)
                        if (has_passive(&player.equipped, "Bleed chance")) {
                            if (rand() % 100 < 25) {
                                int bleed_damage = 5;
                                enemy_hp -= bleed_damage;
                                snprintf(passive_effect, sizeof(passive_effect),
                                        "Your weapon bleeds the enemy! Dealt %d extra bleed damage.\n", bleed_damage);
                                passive_triggered = 1;
                            }
                        }
        
                        // Burn (20% chance)
                        if (has_passive(&player.equipped, "Burn effect")) {
                            if (rand() % 100 < 20) {
                                int burn_damage = 7;
                                enemy_hp -= burn_damage;
                                snprintf(passive_effect, sizeof(passive_effect),
                                        "Enemy is burning! Dealt %d extra burn damage.\n", burn_damage);
                                passive_triggered = 1;
                            }
                        }
        
                        // Double Hit (30% chance)
                        if (has_passive(&player.equipped, "Double Hit")) {
                            if (rand() % 100 < 30) {
                                int extra_damage = base + rand() % (variance + 1);
                                enemy_hp -= extra_damage;
                                snprintf(passive_effect, sizeof(passive_effect),
                                        "Double hit activated! Dealt %d extra damage.\n", extra_damage);
                                passive_triggered = 1;
                            }
                        }
        
                        // Poison (20% chance)
                        if (has_passive(&player.equipped, "Poison")) {
                            if (rand() % 100 < 20) {
                                int poison_damage = 3;
                                enemy_hp -= poison_damage;
                                snprintf(passive_effect, sizeof(passive_effect),
                                        "Enemy is poisoned! Dealt %d extra poison damage.\n", poison_damage);
                                passive_triggered = 1;
                            }
                        }
        
                        enemy_hp -= damage;
                        if (enemy_hp < 0) enemy_hp = 0;
        
                        char attack_msg[BUFFER_SIZE];
                        if (is_crit) {
                            snprintf(attack_msg, BUFFER_SIZE,
                                    "=== CRITICAL HIT! ===\nYou dealt %d damage!\n%s",
                                    damage, passive_triggered ? passive_effect : "");
                        } else {
                            snprintf(attack_msg, BUFFER_SIZE,
                                    "You dealt %d damage!\n%s",
                                    damage, passive_triggered ? passive_effect : "");
                        }
        
                        send(new_socket, attack_msg, strlen(attack_msg), 0);
        
                        if (enemy_hp == 0) {
                            int reward = rand() % 100 + 50;
                            player.gold += reward;
                            player.kills++;
                            snprintf(attack_msg, BUFFER_SIZE,
                                    "You defeated the enemy!\n=== REWARD ===\nYou earned %d gold!\nTotal kills: %d\n\n",
                                    reward, player.kills);
                            send(new_socket, attack_msg, strlen(attack_msg), 0);
                            break; // Keluar dari loop musuh saat ini
                        } else {
                            snprintf(attack_msg, BUFFER_SIZE, "=== ENEMY STATUS ===\nEnemy health: ");
                            display_health_bar(bar, enemy_hp, enemy_max_hp);
                            strcat(attack_msg, bar);
                            send(new_socket, attack_msg, strlen(attack_msg), 0);
                        }
                    } else if (strcmp(buffer, "exit") == 0) {
                        send(new_socket, "You fled the battle.", 22, 0);
                        goto exit_battle;
                    } else {
                        send(new_socket, "Unknown battle command.", 25, 0);
                    }
                }
            }
            exit_battle:
                continue;
```
- Memulai Pertempuran:
`else if (strcmp(buffer, "BATTLE") == 0) {
    while (1) {
        int enemy_max_hp = rand() % 30 + 40;
        int enemy_hp = enemy_max_hp;
        char msg[BUFFER_SIZE];
        snprintf(msg, BUFFER_SIZE, "=== BATTLE STARTED ===\nEnemy appeared with: ");
        char bar[64];
        display_health_bar(bar, enemy_hp, enemy_max_hp);
        strcat(msg, bar);
        strcat(msg, "\nType 'attack' to attack or 'exit' to leave battle.");
        send(new_socket, msg, strlen(msg), 0);`
Server memulai pertempuran dengan mengirimkan pesan kepada client bahwa musuh baru telah muncul. Musuh ini memiliki HP acak antara 40 hingga 70.

- Menunggu Perintah dari Client:
`while (enemy_hp > 0) {
    memset(buffer, 0, BUFFER_SIZE);
    int recv_len = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
    if (recv_len <= 0) {
        printf("[DEBUG] Client disconnected or recv error during battle\n");
        goto exit_battle;
    }
    buffer[recv_len] = '\0';`
Setelah memulai pertempuran, server menunggu perintah dari client. Jika client mengirimkan perintah "attack", server akan menghitung kerusakan yang diberikan pada musuh.

- Menangani Perintah "attack":
`if (strcmp(buffer, "attack") == 0) {
    int base = player.base_damage;
    int variance = (base / 2 > 0) ? base / 2 : 1;
    int damage = base + rand() % (variance + 1);
    int is_crit = rand() % 100 < 20;
    if (is_crit) damage *= 2;`
Server memproses perintah "attack" dengan menghitung kerusakan yang diterima oleh musuh. Kerusakan didasarkan pada damage dasar pemain dan mungkin dikalikan dua jika terjadi critical hit (20% peluang).

- Efek Pasif Senjata (Contoh: Bleed, Burn, Poison, dll.):
`if (has_passive(&player.equipped, "Bleed chance")) {
    if (rand() % 100 < 25) {
        int bleed_damage = 5;
        enemy_hp -= bleed_damage;
        snprintf(passive_effect, sizeof(passive_effect), "Your weapon bleeds the enemy! Dealt %d extra bleed damage.\n", bleed_damage);
        passive_triggered = 1;
    }
}
- Server juga memeriksa apakah senjata pemain memiliki efek pasif, seperti peluang untuk menyebabkan kerusakan tambahan pada musuh (contoh: "Bleed", "Burn", "Poison", dll.). Jika efek pasif terpicu, kerusakan ekstra diterima oleh musuh.

- Mengirimkan Pesan Serangan:
`char attack_msg[BUFFER_SIZE];
if (is_crit) {
    snprintf(attack_msg, BUFFER_SIZE, "=== CRITICAL HIT! ===\nYou dealt %d damage!\n%s", damage, passive_triggered ? passive_effect : "");
} else {
    snprintf(attack_msg, BUFFER_SIZE, "You dealt %d damage!\n%s", damage, passive_triggered ? passive_effect : "");
}
send(new_socket, attack_msg, strlen(attack_msg), 0);`
Setelah perhitungan damage selesai, server mengirimkan pesan kepada client tentang berapa banyak kerusakan yang berhasil dilakukan pada musuh dan apakah ada efek pasif yang terjadi.

- Musuh Kalah atau Masih Bertahan:
`if (enemy_hp == 0) {
    int reward = rand() % 100 + 50;
    player.gold += reward;
    player.kills++;
    snprintf(attack_msg, BUFFER_SIZE, "You defeated the enemy!\n=== REWARD ===\nYou earned %d gold!\nTotal kills: %d\n\n", reward, player.kills);
    send(new_socket, attack_msg, strlen(attack_msg), 0);
    break; // Keluar dari loop musuh saat ini
} else {
    snprintf(attack_msg, BUFFER_SIZE, "=== ENEMY STATUS ===\nEnemy health: ");
    display_health_bar(bar, enemy_hp, enemy_max_hp);
    strcat(attack_msg, bar);
    send(new_socket, attack_msg, strlen(attack_msg), 0);
}`
Jika musuh kalah, server memberikan hadiah berupa emas kepada pemain dan menambah jumlah kill pemain. Server juga menampilkan status musuh yang tersisa jika musuh masih hidup.

- Menangani Perintah "exit":
`else if (strcmp(buffer, "exit") == 0) {
    send(new_socket, "You fled the battle.", 22, 0);
    goto exit_battle;
}`
Jika pemain memilih untuk keluar dari pertempuran dengan mengetik "exit", server akan mengirim pesan "You fled the battle" dan keluar dari loop pertempuran.
---
## soal_4
### Tiba-Tiba Jadi Admin Sistem `(Warnet)` Setelah Reinkarnasi?! 📶🖥️ Dari Shadow Monarch ke Admin Sistem Hunter Dunia Lain!
---
### Problem:

> **[...] . Beruntungnya, Sung Jin Woo pun mengalami reinkarnasi dan sekarang bekerja sebagai seorang admin. Uniknya, pekerjaan ini mempunyai sebuah sistem yang bisa melakukan tracking pada seluruh aktivitas dan keadaan seseorang. Sayangnya, model yang diberikan oleh Bos-nya sudah sangat tua sehingga program tersebut harus dimodifikasi agar tidak ketinggalan zaman, dengan spesifikasi..**

---
> **Sebagai efisiensi line, pada soal ini penjelasan `source code` pada beberapa *line code* disingkat dengan `[...]`, sehingga tidak memakan banyak line yang ada. Kejelasan line yang sudah pasti / FIX telah di bentuk dan dirancang pada folder soal yang sudah disediakan / ada. Terimakasih 🙇‍♂️🙏**
---

### Code's Key Components
> **`system.c`**
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#define MAX_HUNTERS 50
#define MAX_DUNGEONS 50

struct Hunter {
    char username[50];
    int level;
    int exp;
    int atk;
    int hp;
    int def;
    int banned;
    key_t shm_key;
};

struct Dungeon {
    char name[50];
    int min_level;
    int exp;
    int atk;
    int hp;
    int def;
    key_t shm_key;
};

struct SystemData {
    struct Hunter hunters[MAX_HUNTERS];
    int num_hunters;
    struct Dungeon dungeons[MAX_DUNGEONS];
    int num_dungeons;
    int current_notification_index;
};

key_t get_system_key() {
    return ftok("/tmp", 'S');
}
```
Bekerja sebagai struktur data dan konstanta pada data dalam sistem, dengan struktur data utama yaitu: 
-   `Hunter`: data pemain seperti `username`, `level`, `statistik`, dan `shm_key` (key shared memory hunter).
    
-   `Dungeon`: informasi dungeon seperti `nama`, level minimum, statistik hadiah, dan `shm_key` (key shared memory dungeon).
    
-   `SystemData`: shared memory pusat untuk daftar semua hunter dan dungeon.

Serta konfigurasi *shared memory* dengan key shared memory utama sistem.

```
const char *dungeon_names[] = {
    "Double Dungeon", "Demon Castle", "Pyramid Dungeon", "Red Gate Dungeon",
    [...]
    "D-Rank Dungeon", "Gwanak Mountain Dungeon", "Hapjeong Subway Station Dungeon"
};
```
Berfungsi sebagai *Array string* sebagai pilihan nama dungeon random.

```
void display_all_hunters(struct SystemData *data) {
    printf("=== LIST HUNTERS ===\n");
    for (int i = 0; i < data->num_hunters; ) {
        key_t h_key = data->hunters[i].shm_key;
        int h_shmid = shmget(h_key, sizeof(struct Hunter), 0666);
        if (h_shmid == -1) {
        [...]
        
        shmdt(h_ptr);
        i++;
    }    
}
```
Berfungsi untuk menampilkan seluruh hunter di sistem, dengan mekanisme utama sebagai berikut:
-   *Looping* dan membaca shared memory seluruh hunter melalui `shmget()` dan `shmat()`.
    
-   Menampilkan data statistik dan status setiap hunter.

```
void display_all_dungeons(struct SystemData *data) {
    printf("=== LIST DUNGEONS ===\n");
    for (int i = 0; i < data->num_dungeons; i++) {
        struct Dungeon *d = &data->dungeons[i];
        int d_shmid = shmget(d->shm_key, sizeof(struct Dungeon), 0666);
        [...]
        
        shmdt(d_ptr);
    }
}
```
Berfungsi untuk menampilkan seluruh dungeon yang ada di sistem, dengan mekanisme utama yang mirip dengan `display_all_hunters` yaitu sebagai berikut:
Menampilkan semua dungeon yang aktif:

-   Buka shared memory dungeon satu per satu melalui `shmget()` dan `shmat()`.
    
-   Tampilkan informasi dungeon.

```
void generate_dungeon(struct SystemData *data) {
    if (data->num_dungeons >= MAX_DUNGEONS) {
        printf("Dungeon limit reached.\n");
        return;
    }

    struct Dungeon new_dungeon;
    srand(time(NULL) + rand()); // randomisasi lebih baik

    // Set nama dan statistik dungeon
    strcpy(new_dungeon.name, dungeon_names[rand() % 11]);
    new_dungeon.min_level = 1 + rand() % 5;
    new_dungeon.exp = 150 + rand() % 151;
    new_dungeon.atk = 100 + rand() % 51;
    new_dungeon.hp  = 50 + rand() % 51;
    new_dungeon.def = 25 + rand() % 26;
    [...]

    // Output informasi dungeon yang dibuat
    printf("Dungeon '%s' created! (Min Level: %d)\n", new_dungeon.name, new_dungeon.min_level);
}
```
Berfungsi dalam membuat dungeon baru, dengan mekanisme sebagai berikut:
-   Randomisasi nama dan statistik dungeon dari array `dungeon_names[]`.
    
-   Membuat `shm_key` unik dengan `ftok`.
    
-   Buat *shared memory* pada *shared memory dungeon* baru dan simpan objek `Dungeon` ke sana.
    
-   Tambahkan data dungeon *shm* ke `SystemData`.

```
void ban_hunter(struct SystemData *data) {
    char name[50];
    printf("Masukkan username hunter yang ingin di-ban/unban: ");
    scanf("%s", name);
    getchar();

    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, name) == 0) {
        [...]

    printf("Hunter tidak ditemukan.\n");
}
```
Berfungsi dalam menjadi fitur untuk ban/un-ban hunter tertentu, dengan mekanisme utama sebagai berikut:
-   Menemukan hunter dengan `strcmp(username)`.
    
-   Toggle status `banned` begitupun sebaliknya untuk `un-banned`.
    
-   Update ke `SystemData` (utama) dan Shared memory hunter (akses langsung via `shmat()`).

```
void reset_hunter(struct SystemData *data) {
    char name[50];
    printf("Masukkan username hunter yang ingin di-reset: ");
    scanf("%s", name);
    getchar();

    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, name) == 0) {
        [...]

    printf("Hunter tidak ditemukan.\n");
}
```
Berfungsi dalam menjadi fitur untuk Mengembalikan hunter ke status awal, dengan mekanisme sebagai berikut:

-   Mencari nama Hunter
-   Mengatur semua stat hunter kembali ke default:
    
    -   Level = 1, EXP = 0, ATK = 10, HP = 100, DEF = 5.
        
-   Update ke `SystemData` dan *shared memory hunter*.

```
int main() {
    key_t sys_key = get_system_key();
    shmid = shmget(sys_key, sizeof(struct SystemData), IPC_CREAT | 0666);
    system_data = (struct SystemData *)shmat(shmid, NULL, 0);

    if (system_data->num_hunters == 0 && system_data->num_dungeons == 0) {
        system_data->num_hunters = 0;
        system_data->num_dungeons = 0;
    }

    int choice;
    do {
        printf("\n=== SYSTEM MENU ===\n");
        printf("1. Lihat semua Hunter\n");
        printf("2. Lihat semua Dungeon\n");
        printf("3. Generate Dungeon\n");
        printf("4. Ban Hunter\n");
        printf("5. Reset Hunter\n");
        printf("6. Exit\n");
        printf(">> ");
        scanf("%d", &choice);
        getchar();
        [...]

    shmdt(system_data);
    return 0;
}
```
Berfungsi sebagai program utama pada kode `system.c` yang mengatur keseluruhan fitur dan lainnya pada sistem untuk admin, dengan mekanisme utama sebagai berikut:

- Inisialisasi Shared Memory dan alokasi memori data-datanya serta menginisialisasi data-data kosong (hunter dan dungeon)

- Membuat menu admin pada `./system` dengan opsi:
	-    Lihat semua hunter
	-   Lihat semua dungeon
	-   Generate dungeon baru
	-   Ban/unban hunter
	-   Reset hunter
	-   Exit & bersihkan semua shared memory

Serta berfungsi sebagai dalam menyediakan antarmuka admin (melalui terminal) hingga Mengelola validitas data melalui `shmget`, `shmdt`, dan `shmctl`.

> **`hunter.c`**
```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
[...]

#define MAX_HUNTERS 50
#define MAX_DUNGEONS 50

struct Hunter {
	[...]
};

struct Dungeon {
	[...]
};

struct SystemData {
	[...]
};

key_t get_system_key() {
    return ftok("/tmp", 'S');
```
Sama seperti program `system.c`, `hunter.c` juga menginisiasi struktur data dan konstanta pada data dalam sistem, dengan struktur data utama yang sama seperti `system.c` yaitu: 
-   `Hunter`: data pemain seperti `username`, `level`, `statistik`, dan `shm_key` (key shared memory hunter).
    
-   `Dungeon`: informasi dungeon seperti `nama`, level minimum, statistik hadiah, dan `shm_key` (key shared memory dungeon).
    
-   `SystemData`: shared memory pusat untuk daftar semua hunter dan dungeon.

Serta konfigurasi *shared memory* pada `hunter,c` dengan key shared memory utama sistem didalam `system.c`.

```
struct Hunter* register_hunter(struct SystemData *data, char *username) {
    if (data->num_hunters >= MAX_HUNTERS) return NULL;

    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, username) == 0) return NULL;
    }
	[...]

    printf("Hunter %s registered.\n", username);
    return (struct Hunter *)shmat(temp_shmid, NULL, 0);
}
```
Berfungsi sebagai fitur *Register* pada sistem `hunter.c` untuk mendaftarkan dan menginsiasi *key shm*untuk *hunter*, dengan mekanisme utama sebagai berikut:

-   Memvalidasi apakah jumlah hunter belum penuh.
    
-   Mengecek apakah username sudah digunakan.
    
-   Menginisialisasi data hunter baru dengan [*default stat*].
    
-   Generate shm_key unik dan mengalokasikan shared memory serta key-nya untuk hunter baru (`shm_key` dan ukuran `struct Hunter`).
    
-   Simpan data hunter baru ke shared memory.
    
-   Menambahkan data hunter baru ke `SystemData`.

```
struct Hunter* login(struct SystemData *data, char *username) {
    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, username) == 0) {
            int h_shmid = shmget(data->hunters[i].shm_key, sizeof(struct Hunter), 0666);
            [...]

    }
    return NULL;
}
```
Berfungsi sebagai fitur *login* untuk para *hunter* yang sudah memiliki akun / key / shm nama *hunter* yang sudah dinisiasi sebelumnya, dengan mekanisme utama sebagai berikut:

-   Mencari username di `data->hunters`.

-   Mengaambil pointer ke shared memory hunter dan return-nya sebagai akun hunter-nya.

Setelah akun telah diinisiasi melalui *register* / *login*, hunter dapt mengakses menu utama hunter yang dapat mengatur fitur-fitur utama hunter, yaitu sebagai berikut.
```
void list_available_dungeons(struct SystemData *data, struct Hunter *hunter) {
    if (hunter->banned) {
        printf("You are banned from performing this action.\n");
        return;
    }

    printf("=== DUNGEONS AVAILABLE FOR YOU ===\n");
    for (int i = 0; i < data->num_dungeons; i++) {
        struct Dungeon *d = &data->dungeons[i];
	    [...]

        }
        shmdt(d_ptr);
    }
}
```
Berfungsi sebagai salah satu fitur utama hunter yang memperlihatkan keseluruhan dungeon yang ada bagi hunter bisa *raid* sesuai dengan level-nya, dengan mekanisme utama sebagai berikut:

-   Cek apakah hunter sedang _banned_.
    
-   Menampilkan dungeon yang sesuai `min_level` pemain.
    
-   Membuka shared memory tiap dungeon satu-satu dan hanya  dungeon yang sesuai level yang ditampilkan dengan status dungeonnya.

```
void raid_dungeon(struct SystemData *data, struct Hunter *hunter) {
    if (hunter->banned) {
        printf("You are banned from performing this action.\n");
        return;
    }

    printf("\n=== RAIDABLE DUNGEONS ===\n");
    int available[MAX_DUNGEONS];
    int count = 0;

    for (int i = 0; i < data->num_dungeons; i++) {
        struct Dungeon *d = &data->dungeons[i];
        int d_shmid = shmget(d->shm_key, sizeof(struct Dungeon), 0666);
        [...]

    printf("\nPress enter to continue...");
    getchar();
}
```
Berfungsi sebagai salah satu fitur utama hunter yang memperlihatkan keseluruhan dungeon yang ada bagi hunter bisa *raid* sesuai dengan level-nya dan memperbolehkan hunter untuk *raid* suatu dungeon sesuai dengan level-nya, dengan mekanisme utama sebagai berikut:

-   Cek apakah hunter sedang _banned_.
    
-   Tampilkan dungeon yang bisa di*raid* dan pilih dungeon yang akan di*raid*.
    
-   Tambahkan semua stat dan EXP ke hunter dari stat dungeon yang telah di*raid*.
    
-   Jika EXP ≥ 500, hunter akan level up dan kembali ke EXP = 0 di level baru.
    
-   Menghapus *shared memory* dungeon setelah di*raid* oleh hunter.
    
-   Dungeon dihapus dari array `SystemData`.

```
void battle_hunter(struct SystemData *data, struct Hunter *myself) {
    if (myself->banned) {
        printf("You are banned from performing this action.\n");
        return;
    }

    printf("\n=== PVP LIST ===\n");

    int valid_index[MAX_HUNTERS];
    int count = 0;

    // Tampilkan daftar hunter lain dari shared memory
    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, myself->username) != 0 && data->hunters[i].banned == 0) {
            int shmid = shmget(data->hunters[i].shm_key, sizeof(struct Hunter), 0666);
            if (shmid == -1) continue;
            [...]
```
Berfungsi sebagai salah satu fitur utama hunter yang memperbolehkan sesama hunter untuk menginisiasi PvP / battle antara hunter dengan hunter lainnya dengan berbagai ketentuan yang telah ditentukan, dengan mekanisme utama sebagai berikut:

1.   Cek hunter *banned / un-banned*.
    
2.   Tampilkan daftar lawan yang valid.
    
3.   Input username lawan.
    
4.   Buka shared memory lawan.
    
5.   Hitung power kita dan lawan:  `power = atk + hp + def` 
    
6.   Kondisi Battle:
- Jika menang:
	-   Tambahkan semua stat lawan ke kita.
    -   Hapus data lawan dari *shared memory hunter*

-   Jika kalah:
    -   Stat kita ditambahkan ke lawan.
    -   Hapus data shared memory kita dari *shared memory hunter*
    -   `exit(0)` karena kita kalah dan keluar dari permainan.

```
pthread_t notif_thread;
int notif_enabled = 0;
int notif_started = 0;
int notification_on = 0;

#define MAX_NOTIF_LEN 100
char latest_notif[256] = "";

void* notif_loop(void* arg) {
    struct SystemData* sys_data = (struct SystemData*) arg;

    while (1) {
        if (!notif_enabled || sys_data->num_dungeons == 0) {
            sleep(1);
            continue;
        }

        struct Dungeon* d = &sys_data->dungeons[sys_data->current_notification_index];

        snprintf(latest_notif, sizeof(latest_notif),
                 "[NOTIF] Dungeon: %s with minimum Lv: %d", d->name, d->min_level);
                 [...]

    return NULL;
}
```
Berfungsi sebagai salah satu fitur utama dalam program `hunter.c` yan menjadi *toggle notification* untuk memperlihatkan seluruh dungeon yang terbuka dan diperbarui setiap 3 detik, dengan mekanisme utama sebagai berikut:

-   Loop selamanya.
    
-   Setiap 3 detik tampilkan notifikasi dungeon ke layar:
    
    -   Dungeon ditampilkan di baris ke-2 terminal (`\033[2;1H`).
        
    -   Posisi kursor disimpan dan dikembalikan (`\033[s`, `\033[u`).
        
    -   Menampilkan nama dungeon dan level minimal.
        
-   Notifikasi berputar dari `current_notification_index`.

```
void print_hunter_menu_with_notif(const char* username) {
    printf("\033[2J\033[H"); // Clear screen

    printf("=== HUNTER SYSTEM ===\n");

    if (notif_enabled && strlen(latest_notif) > 0) {
        printf("%s\n", latest_notif);  // Tampilkan notifikasi di sini
    } else {
        printf("\n"); // Spasi kosong kalau belum ada notif
    }
    [...]

    printf("Choice: ");
    fflush(stdout);
}
```
Berfungsi sebagai format utama pada menu utama hunter secara rapih, dengan mekanisme utama sebagai berikut:

-   Membersihkan layar (`\033[2J\033[H`).
-   Menampilkan menu utama hunter.
-   Menampilkan notifikasi di atas menu jika `notif_enabled`.

```
int main() {
    key_t key = get_system_key();
    int shmid = shmget(key, sizeof(struct SystemData), 0666);
    if (shmid == -1) {
        perror("Failed to access system shared memory");
        return 1;
    }

    struct SystemData *data = (struct SystemData *)shmat(shmid, NULL, 0);

    char username[50];
    int logged_in = 0;
    struct Hunter *hunter = NULL;

    while (!logged_in) {
        int choice;
        printf("\n=== HUNTER MENU ===\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();
        [...]

    shmdt(data);
    return 0;
}
```
Berfungsi sebagai program utama dalam program `hunter.c` yang mengatur keseluruhan fitur-fitur utama, sekunder, dan menu utama interaktif hunter pada program `hunter.c`, yaitu sebagai berikut:
-   **Koneksi dan mengakses ke shared memory system untuk data**:
 ```
    key_t key = get_system_key(); int shmid =shmget(key,sizeof(SystemData), 0666);
    data = shmat(shmid, NULL, 0);
```
    
-   **Loop pada login/register**:
    
    -   Pilihan 1: Register → `register_hunter`
        
    -   Pilihan 2: Login → `login`
        
    -   Pilihan 3: Exit → detach & return
        
-   **Setelah login, tampilkan menu interaktif hunter :**
```
1. List Dungeon 
2. Raid 
3. Battle 
4. Toggle Notification 
5. Exit
```
   -   **Case 1**: Menampilkan dungeon available (fungsi #5)
        
   -   **Case 2**: Melakukan raid dungeon (fungsi #6)
        
   -   **Case 3**: PvP dengan hunter lain (fungsi #7)
        
   -   **Case 4**: Mengaktifkan thread notifikasi jika belum berjalan.
        `if (notif_enabled && !notif_started) pthread_create(...)` 
        
   -   **Case 5**: Keluar dari game / menu hunter.


---
## Revisi
### soal_4

![image](https://github.com/user-attachments/assets/f4807ba8-5137-4e20-bbb5-f3086ac0c931)

Seharusnya saat battle, keseluruhan power hunter yang telah diinisiasi pada data {atk + hp + def} sehingga data hunter pada *shared memory* selalu ter-update dan tidak terinisiasi data awal. Pada data battle, bisa terlihat seluruh power hunter yang telah terinisiasi dengan stats yang ada.

```
void battle_hunter(struct SystemData *data, struct Hunter *myself) {
    if (myself->banned) {
        printf("You are banned from performing this action.\n");
        return;
    }

    printf("\n=== PVP LIST ===\n");

    int valid_index[MAX_HUNTERS];
    int count = 0;

    // Tampilkan daftar hunter lain dari shared memory
    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, myself->username) != 0 && data->hunters[i].banned == 0) {
            int shmid = shmget(data->hunters[i].shm_key, sizeof(struct Hunter), 0666);
            if (shmid == -1) continue;

            struct Hunter *h_ptr = (struct Hunter *)shmat(shmid, NULL, 0);
            if (h_ptr == (void *)-1) continue;

            int power = h_ptr->atk + h_ptr->hp + h_ptr->def;
            printf("%s - Total Power: %d\n", h_ptr->username, power);
            valid_index[count++] = i;

            shmdt(h_ptr);
        }
    }

    if (count == 0) {
        printf("No available opponents.\n");
        return;
    }

    char target[50];
    printf("\nTarget: ");
    scanf("%s", target);
    getchar();

    int found = -1;
    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, target) == 0 &&
            strcmp(myself->username, target) != 0 &&
            data->hunters[i].banned == 0) {
            found = i;
            break;
        }
    }

    if (found == -1) {
        printf("Invalid target.\n");
        return;
    }

    // Ambil hunter lawan dari shared memory
    int shmid = shmget(data->hunters[found].shm_key, sizeof(struct Hunter), 0666);
    if (shmid == -1) {
        printf("Failed to access opponent's data.\n");
        return;
    }

    struct Hunter *opponent = (struct Hunter *)shmat(shmid, NULL, 0);
    if (opponent == (void *)-1) {
        perror("shmat failed");
        return;
    }

    int my_power = myself->atk + myself->hp + myself->def;
    int op_power = opponent->atk + opponent->hp + opponent->def;

    printf("You chose to battle %s\n", opponent->username);
    printf("Your Power: %d\n", my_power);
    printf("Opponent's Power: %d\n", op_power);

    if (my_power >= op_power) {
        myself->atk += opponent->atk;
        myself->hp += opponent->hp;
        myself->def += opponent->def;
        myself->exp += opponent->exp;
        myself->level += opponent->level;

        shmctl(shmid, IPC_RMID, NULL); // Hapus shared memory lawan
        printf("Deleting defender's shared memory (shmid: %d)\n", shmid);

        // Hapus hunter dari data->hunters
        for (int i = found; i < data->num_hunters - 1; i++) {
            data->hunters[i] = data->hunters[i + 1];
        }
        data->num_hunters--;

        printf("Battle won! You acquired %s's stats\n", target);
    } else {
        // Update lawan dengan stat kita
        opponent->atk += myself->atk;
        opponent->hp += myself->hp;
        opponent->def += myself->def;
        opponent->exp += myself->exp;
        opponent->level += myself->level;

        // Hapus shared memory kita sendiri
        int my_shmid = shmget(myself->shm_key, sizeof(struct Hunter), 0666);
        if (my_shmid >= 0) {
            shmctl(my_shmid, IPC_RMID, NULL);
            printf("You lost! Deleting your shared memory (shmid: %d)\n", my_shmid);
        }

        // Hapus kita dari data->hunters
        for (int i = 0; i < data->num_hunters; i++) {
            if (strcmp(data->hunters[i].username, myself->username) == 0) {
                for (int j = i; j < data->num_hunters - 1; j++) {
                    data->hunters[j] = data->hunters[j + 1];
                }
                data->num_hunters--;
                break;
            }
        }

        shmdt(opponent);
        printf("You lost the battle. %s took your stats.\n", opponent->username);
        exit(0);
    }

    shmdt(opponent);
    printf("\nPress enter to continue...");
    getchar();
}
```

|After fix|

![image](https://github.com/user-attachments/assets/84090b1b-4f17-4e6d-9bb6-5742eabb3434)

Setelah fix, data *shared memory hunter* telah ter-update dengan yang paling baru pada fitur *battle*.

Tak hanya itu, di beberapa *source code* ada beberapa fix untuk code yang lebih bersih dan tertata pada data *shared memory* yang tertuju, seperti:
- ban/unban hunter
- reset hunter
- pembersihan code cleanup() dengan yang lebih ditujukan pada fungsi yang lebih benar (penghapusan shm pada exit).
(Namun, fix *source code* ini tidak memiliki bukti output karena terindikasi setalah revisi sebelumnya, sehingga proble langsung diperbaki)
