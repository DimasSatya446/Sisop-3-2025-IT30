#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <time.h>
#include <errno.h>

// Import shop.c sebagai bagian dari kompilasi
#include "shop.c"

#define PORT 8888
#define BUFFER_SIZE 2048
#define MAX_INVENTORY 10
#define MAX_PASSIVE 2

typedef struct {
    int gold;
    Weapon inventory[MAX_INVENTORY];
    int inventory_size;
    Weapon equipped;
    int base_damage;
    int kills;
} PlayerStats;

// Deklarasi fungsi eksternal dari shop.c
extern void init_shop();
extern int get_weapon_by_id(int id, Weapon* result);
extern void display_weapon_shop(char* buffer);
extern void format_weapon_display(const Weapon* w, char* output, int with_passive);

// Mutex untuk proteksi akses ke toko senjata
pthread_mutex_t shop_mutex = PTHREAD_MUTEX_INITIALIZER;

int is_same_weapon(Weapon* a, Weapon* b) {
    return strcmp(a->name, b->name) == 0;
}

void display_health_bar(char* bar, int current, int max) {
    int width = 20;
    int fill = (current * width) / max;
    char filled[21], empty[21];
    memset(filled, '=', fill);
    filled[fill] = '\0';
    memset(empty, ' ', width - fill);
    empty[width - fill] = '\0';
    sprintf(bar, "[%s%s] %d/%d HP", filled, empty, current, max);
}

int has_passive(const Weapon* w, const char* effect_name);

int has_passive(const Weapon* w, const char* effect_name) {
    return strstr(w->passive[0], effect_name) || strstr(w->passive[1], effect_name);
}

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

        } else if (strcmp(buffer, "SHOP") == 0) {
            char shop_msg[BUFFER_SIZE];
            display_weapon_shop(shop_msg);
            strcat(shop_msg, "Enter weapon ID to buy (0 to cancel): ");
            send(new_socket, shop_msg, strlen(shop_msg), 0);

            // Baca ID senjata yang dipilih
            memset(buffer, 0, BUFFER_SIZE);
            valread = recv(new_socket, buffer, BUFFER_SIZE - 1, 0);
            if (valread <= 0) {
                send(new_socket, "Connection lost!", 17, 0);
                break;
            }
            buffer[valread] = '\0';

            // Validasi input
            char* endptr;
            int id = strtol(buffer, &endptr, 10);
            if (endptr == buffer || *endptr != '\0') {
                send(new_socket, "Invalid input!", 15, 0);
                continue;
            }

            if (id == 0) {
                send(new_socket, "Cancelled.", 10, 0);
            } else {
                Weapon chosen;
                if (get_weapon_by_id(id, &chosen)) {
                    if (player.gold >= chosen.price) {
                        if (player.inventory_size >= MAX_INVENTORY) {
                            send(new_socket, "Inventory full!", 17, 0);
                            continue;
                        }

                        player.gold -= chosen.price;
                        player.inventory[player.inventory_size++] = chosen;

                        char confirm[128];
                        snprintf(confirm, sizeof(confirm),
                                 "You bought %s! Gold left: %d",
                                 chosen.name, player.gold);
                        send(new_socket, confirm, strlen(confirm), 0);
                    } else {
                        send(new_socket, "Not enough gold!", 17, 0);
                    }
                } else {
                    send(new_socket, "Invalid weapon ID!", 19, 0);
                }
            }
        } else if (strcmp(buffer, "INVENTORY") == 0) {
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

        } else if (strcmp(buffer, "BATTLE") == 0) {
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
        } else if (strcmp(buffer, "EXIT") == 0) {
            break;
        } else {
            send(new_socket, "Unknown command", 17, 0);
        }
    }

    close(new_socket);
    pthread_exit(NULL);
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