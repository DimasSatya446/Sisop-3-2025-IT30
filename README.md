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
## soal_2

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
