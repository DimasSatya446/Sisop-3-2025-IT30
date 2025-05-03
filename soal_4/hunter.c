// hunter.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
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

int notification_on = 0;
pthread_t notif_thread;

struct Hunter* register_hunter(struct SystemData *data, char *username) {
    if (data->num_hunters >= MAX_HUNTERS) return NULL;

    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, username) == 0) return NULL;
    }

    struct Hunter new_hunter;
    strcpy(new_hunter.username, username);
    new_hunter.level = 1;
    new_hunter.exp = 0;
    new_hunter.atk = 10;
    new_hunter.hp = 100;
    new_hunter.def = 5;
    new_hunter.banned = 0;

    key_t temp_key;
    int temp_shmid;
    do {
        temp_key = ftok("/tmp", rand() % 100 + 101);
        temp_shmid = shmget(temp_key, sizeof(struct Hunter), IPC_CREAT | IPC_EXCL | 0666);
    } while (temp_shmid == -1);

    new_hunter.shm_key = temp_key;

    struct Hunter *h_ptr = shmat(temp_shmid, NULL, 0);
    if (h_ptr == (void *)-1) {
        perror("shmat failed");
        return NULL;
    }

    *h_ptr = new_hunter;
    shmdt(h_ptr);

    data->hunters[data->num_hunters++] = new_hunter;
    printf("Hunter %s registered.\n", username);
    return (struct Hunter *)shmat(temp_shmid, NULL, 0);
}

struct Hunter* login(struct SystemData *data, char *username) {
    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, username) == 0) {
            int h_shmid = shmget(data->hunters[i].shm_key, sizeof(struct Hunter), 0666);
            if (h_shmid == -1) return NULL;

            struct Hunter *h_ptr = shmat(h_shmid, NULL, 0);
            if (h_ptr == (void *)-1) {
                perror("shmat failed");
                return NULL;
            }
            return h_ptr;
        }
    }
    return NULL;
}

void list_available_dungeons(struct SystemData *data, struct Hunter *hunter) {
    if (hunter->banned) {
        printf("You are banned from performing this action.\n");
        return;
    }

    printf("=== DUNGEONS AVAILABLE FOR YOU ===\n");
    for (int i = 0; i < data->num_dungeons; i++) {
        struct Dungeon *d = &data->dungeons[i];
        int d_shmid = shmget(d->shm_key, sizeof(struct Dungeon), 0666);
        if (d_shmid < 0) continue;
        struct Dungeon *d_ptr = shmat(d_shmid, NULL, 0);
        if (d_ptr == (void *)-1) continue;

        if (d_ptr->min_level <= hunter->level) {
            printf("%d. %s (Lv%d+) EXP:%d ATK:%d HP:%d DEF:%d\n",
                i + 1, d_ptr->name, d_ptr->min_level,
                d_ptr->exp, d_ptr->atk, d_ptr->hp, d_ptr->def);
        }
        shmdt(d_ptr);
    }
}

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
        if (d_shmid < 0) continue;
        struct Dungeon *d_ptr = shmat(d_shmid, NULL, 0);
        if (d_ptr == (void *)-1) continue;

        if (d_ptr->min_level <= hunter->level) {
            available[count++] = i;
            printf("%d. %s (Level %d+)\n", count, d_ptr->name, d_ptr->min_level);
        }
        shmdt(d_ptr);
    }

    if (count == 0) {
        printf("No available dungeons for your level.\n");
        return;
    }

    int choice;
    printf("Choose Dungeon: ");
    scanf("%d", &choice);
    getchar();

    if (choice < 1 || choice > count) {
        printf("Invalid dungeon.\n");
        return;
    }

    int index = available[choice - 1];
    struct Dungeon *d = &data->dungeons[index];
    int d_shmid = shmget(d->shm_key, sizeof(struct Dungeon), 0666);
    struct Dungeon *d_ptr = shmat(d_shmid, NULL, 0);
    if (d_ptr == (void *)-1) return;

    hunter->atk += d_ptr->atk;
    hunter->hp += d_ptr->hp;
    hunter->def += d_ptr->def;
    hunter->exp += d_ptr->exp;

    printf("\nRaid success! Gained:\n");
    printf("ATK: %d\n", d_ptr->atk);
    printf("HP: %d\n", d_ptr->hp);
    printf("DEF: %d\n", d_ptr->def);
    printf("EXP: %d\n", d_ptr->exp);

    while (hunter->exp >= 500) {
        hunter->exp -= 500;
        hunter->level++;
    }

    shmdt(d_ptr);
    shmctl(d_shmid, IPC_RMID, NULL);
    for (int i = index; i < data->num_dungeons - 1; i++) {
        data->dungeons[i] = data->dungeons[i + 1];
    }
    data->num_dungeons--;

    printf("\nPress enter to continue...");
    getchar();
}

void battle_hunter(struct SystemData *data, struct Hunter *myself) {
    if (myself->banned) {
        printf("You are banned from performing this action.\n");
        return;
    }

    printf("\n=== PVP LIST ===\n");

    int valid_index[MAX_HUNTERS];
    int count = 0;

    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, myself->username) != 0 && data->hunters[i].banned == 0) {
            int power = data->hunters[i].atk + data->hunters[i].hp + data->hunters[i].def;
            printf("%s - Total Power: %d\n", data->hunters[i].username, power);
            valid_index[count++] = i;
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

    struct Hunter *opponent = &data->hunters[found];
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

        int shmid = shmget(opponent->shm_key, sizeof(struct Hunter), 0666);
        if (shmid >= 0) {
            shmctl(shmid, IPC_RMID, NULL);
            printf("Deleting defender's shared memory (shmid: %d)\n", shmid);
        }

        for (int i = found; i < data->num_hunters - 1; i++) {
            data->hunters[i] = data->hunters[i + 1];
        }
        data->num_hunters--;

        printf("Battle won! You acquired %s's stats\n", target);
    } else {
        opponent->atk += myself->atk;
        opponent->hp += myself->hp;
        opponent->def += myself->def;
        opponent->exp += myself->exp;
        opponent->level += myself->level;

        int shmid = shmget(myself->shm_key, sizeof(struct Hunter), 0666);
        if (shmid >= 0) {
            shmctl(shmid, IPC_RMID, NULL);
            printf("You lost! Deleting your shared memory (shmid: %d)\n", shmid);
        }

        for (int i = 0; i < data->num_hunters; i++) {
            if (strcmp(data->hunters[i].username, myself->username) == 0) {
                for (int j = i; j < data->num_hunters - 1; j++) {
                    data->hunters[j] = data->hunters[j + 1];
                }
                data->num_hunters--;
                break;
            }
        }

        printf("You lost the battle. %s took your stats.\n", opponent->username);
        exit(0);
    }

    printf("\nPress enter to continue...");
    getchar();
}

void *notification_loop(void *arg) {
    struct SystemData *data = (struct SystemData *)arg;
    int index = 0;

    while (notification_on) {
        if (data->num_dungeons == 0) {
            printf("\n[NOTIF] No dungeons available.\n");
        } else {
            struct Dungeon d = data->dungeons[index % data->num_dungeons];
            printf("\n[NOTIF] Dungeon: %s | Min Lv: %d |\n", d.name, d.min_level);
            index++;
        }

        sleep(3);
    }

    return NULL;
}

pthread_t notif_thread;
int notif_enabled = 0;
int notif_started = 0;

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

        // Pindah ke baris 2 kolom 1 dan tulis notifikasi
        fprintf(stdout, "\033[s");                      // Simpan posisi kursor
        fprintf(stdout, "\033[2;1H");                   // Pindah ke baris notifikasi
        fprintf(stdout, "\r%-100s", latest_notif);      // Timpa baris notifikasi
        fprintf(stdout, "\033[u");                      // Kembali ke posisi input
        fflush(stdout);

        sys_data->current_notification_index =
            (sys_data->current_notification_index + 1) % sys_data->num_dungeons;

        sleep(3);
    }

    return NULL;
}

void print_hunter_menu_with_notif(const char* username) {
    printf("\033[2J\033[H"); // Clear screen

    printf("=== HUNTER SYSTEM ===\n");

    if (notif_enabled && strlen(latest_notif) > 0) {
        printf("%s\n", latest_notif);  // Tampilkan notifikasi di sini
    } else {
        printf("\n"); // Spasi kosong kalau belum ada notif
    }

    printf("\n--- %s's MENU ---\n", username);
    printf("1. List Dungeon\n");
    printf("2. Raid\n");
    printf("3. Battle\n");
    printf("4. Toggle Notification\n");
    printf("5. Exit\n");
    printf("Choice: ");
    fflush(stdout);
}

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

        if (choice == 1) {
            printf("Username: ");
            scanf("%s", username);
            getchar();
            hunter = register_hunter(data, username);
            if (hunter) {
                printf("Registration success!\n");
                logged_in = 1;
            }
        } else if (choice == 2) {
            printf("Username: ");
            scanf("%s", username);
            getchar();
            hunter = login(data, username);
            if (hunter) {
                printf("Login success!\n");
                logged_in = 1;
            } else {
                printf("Login failed! User not found.\n");
            }
        } else if (choice == 3) {
            shmdt(data);
            return 0;
        } else {
            printf("Invalid choice!\n");
        }
    }

    int ch;
    do {
        print_hunter_menu_with_notif(hunter->username);
        scanf("%d", &ch);
        getchar();

        switch (ch) {
            case 1:
                list_available_dungeons(data, hunter);
		printf("\nPress ENTER to return...");
    		getchar();
                break;
            case 2:
                raid_dungeon(data, hunter);
                break;
            case 3:
                battle_hunter(data, hunter);
                break;
            case 4:  // Toggle Notification
                notif_enabled = !notif_enabled;

                if (notif_enabled && !notif_started) {
                    pthread_create(&notif_thread, NULL, notif_loop, data);
                    notif_started = 1;
                }
                break;
            case 5:
                printf("Exiting...\n");
                break;
            default:
                printf("Invalid choice!\n");
        }
    } while (ch != 5);

    if (notif_started) {
        pthread_cancel(notif_thread);
        pthread_join(notif_thread, NULL);
    }

    shmdt(data);
    return 0;
}
