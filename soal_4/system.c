// system.c
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

const char *dungeon_names[] = {
    "Double Dungeon", "Demon Castle", "Pyramid Dungeon", "Red Gate Dungeon",
    "Hunters Guild Dungeon", "Busan A-Rank Dungeon", "Insects Dungeon", "Goblins Dungeon",
    "D-Rank Dungeon", "Gwanak Mountain Dungeon", "Hapjeong Subway Station Dungeon"
};

void display_all_hunters(struct SystemData *data) {
    printf("=== LIST HUNTERS ===\n");
    for (int i = 0; i < data->num_hunters; ) {
        key_t h_key = data->hunters[i].shm_key;
        int h_shmid = shmget(h_key, sizeof(struct Hunter), 0666);
        if (h_shmid == -1) {
            // Bersihkan entry yang invalid
            for (int j = i; j < data->num_hunters - 1; j++) {
                data->hunters[j] = data->hunters[j + 1];
            }
            data->num_hunters--;
            continue; // lanjut tanpa menaikkan i
        }
    
        struct Hunter *h_ptr = (struct Hunter *)shmat(h_shmid, NULL, 0);
        if (h_ptr == (void *)-1) {
            i++;
            continue;
        }
    
        printf("%d. %s | Lv:%d | EXP:%d | ATK:%d | HP:%d | DEF:%d | %s\n",
               i + 1,
               h_ptr->username,
               h_ptr->level,
               h_ptr->exp,
               h_ptr->atk,
               h_ptr->hp,
               h_ptr->def,
               h_ptr->banned ? "BANNED" : "OK");
    
        shmdt(h_ptr);
        i++;
    }    
}

void display_all_dungeons(struct SystemData *data) {
    printf("=== LIST DUNGEONS ===\n");
    for (int i = 0; i < data->num_dungeons; i++) {
        struct Dungeon *d = &data->dungeons[i];
        int d_shmid = shmget(d->shm_key, sizeof(struct Dungeon), 0666);
        if (d_shmid < 0) continue;
        struct Dungeon *d_ptr = shmat(d_shmid, NULL, 0);
        if (d_ptr == (void *) -1) continue;

        printf("%d. %s | Min Lv: %d | EXP: %d | ATK: %d | HP: %d | DEF: %d | Key: %d\n",
            i + 1, d_ptr->name, d_ptr->min_level, d_ptr->exp,
            d_ptr->atk, d_ptr->hp, d_ptr->def, d_ptr->shm_key);

        shmdt(d_ptr);
    }
}

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

    // Generate unique shm_key
    key_t candidate_key;
    int exists;
    do {
        candidate_key = ftok("/tmp", rand() % 255 + 1); // range: 1 - 255
        exists = 0;
        for (int i = 0; i < data->num_dungeons; i++) {
            if (data->dungeons[i].shm_key == candidate_key) {
                exists = 1;
                break;
            }
        }
    } while (exists);
    new_dungeon.shm_key = candidate_key;

    // Alokasikan shared memory untuk dungeon baru
    int d_shmid = shmget(new_dungeon.shm_key, sizeof(struct Dungeon), IPC_CREAT | 0666);
    if (d_shmid < 0) {
        perror("shmget failed");
        return;
    }

    struct Dungeon *d_ptr = shmat(d_shmid, NULL, 0);
    if (d_ptr == (void *) -1) {
        perror("shmat failed");
        return;
    }

    *d_ptr = new_dungeon;
    shmdt(d_ptr);

    // Simpan dungeon ke system data
    data->dungeons[data->num_dungeons++] = new_dungeon;

    // Output informasi dungeon yang dibuat
    printf("Dungeon '%s' created! (Min Level: %d)\n", new_dungeon.name, new_dungeon.min_level);
}


void ban_hunter(struct SystemData *data) {
    char name[50];
    printf("Masukkan username hunter yang ingin di-ban/unban: ");
    scanf("%s", name);
    getchar();

    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, name) == 0) {
            // Update di struct SystemData
            data->hunters[i].banned = !data->hunters[i].banned;

            // Juga update di shared memory hunter
            int h_shmid = shmget(data->hunters[i].shm_key, sizeof(struct Hunter), 0666);
            if (h_shmid != -1) {
                struct Hunter *h_ptr = shmat(h_shmid, NULL, 0);
                if (h_ptr != (void *)-1) {
                    h_ptr->banned = data->hunters[i].banned;
                    shmdt(h_ptr);
                }
            }

            printf("Hunter %s berhasil di-%s.\n", name, data->hunters[i].banned ? "ban" : "unban");
            return;
        }
    }

    printf("Hunter tidak ditemukan.\n");
}

void reset_hunter(struct SystemData *data) {
    char name[50];
    printf("Masukkan username hunter yang ingin di-reset: ");
    scanf("%s", name);
    getchar();

    for (int i = 0; i < data->num_hunters; i++) {
        if (strcmp(data->hunters[i].username, name) == 0) {
            // Reset struct SystemData
            data->hunters[i].level = 1;
            data->hunters[i].exp = 0;
            data->hunters[i].atk = 10;
            data->hunters[i].hp = 100;
            data->hunters[i].def = 5;
            data->hunters[i].banned = 0;

            // Reset shared memory hunter
            int h_shmid = shmget(data->hunters[i].shm_key, sizeof(struct Hunter), 0666);
            if (h_shmid != -1) {
                struct Hunter *h_ptr = shmat(h_shmid, NULL, 0);
                if (h_ptr != (void *)-1) {
                    h_ptr->level = 1;
                    h_ptr->exp = 0;
                    h_ptr->atk = 10;
                    h_ptr->hp = 100;
                    h_ptr->def = 5;
                    h_ptr->banned = 0;
                    shmdt(h_ptr);
                }
            }

            printf("Hunter %s telah di-reset ke stats awal.\n", name);
            return;
        }
    }

    printf("Hunter tidak ditemukan.\n");
}

int shmid = -1;
struct SystemData *system_data = NULL;

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

        switch (choice) {
            case 1: display_all_hunters(system_data); break;
            case 2: display_all_dungeons(system_data); break;
            case 3: generate_dungeon(system_data); break;
            case 4: ban_hunter(system_data); break;
            case 5: reset_hunter(system_data); break;
            case 6:
    		printf("Menghapus semua shared memory...\n");

    		// Hapus shared memory hunter
    		for (int i = 0; i < system_data->num_hunters; i++) {
        	     int h_shmid = shmget(system_data->hunters[i].shm_key, sizeof(struct Hunter), 0666);
        	     if (h_shmid != -1) shmctl(h_shmid, IPC_RMID, NULL);
    		}

    		// Hapus shared memory dungeon
    		for (int i = 0; i < system_data->num_dungeons; i++) {
        	     int d_shmid = shmget(system_data->dungeons[i].shm_key, sizeof(struct Dungeon), 0666);
        	     if (d_shmid != -1) shmctl(d_shmid, IPC_RMID, NULL);
    		}

    		// Hapus shared memory utama system
    		shmdt(system_data);
    		shmctl(shmid, IPC_RMID, NULL);
    		printf("Semua shared memory berhasil dihapus. Keluar.\n");
    		break;

            default: printf("Invalid choice.\n");
        }
    } while (choice != 6);

    shmdt(system_data);
    return 0;
}
