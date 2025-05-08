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
