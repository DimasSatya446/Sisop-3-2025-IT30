#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

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

void download_csv_if_needed() {
    if (access("delivery_order.csv", F_OK) == -1) {
        printf("Downloading delivery_order.csv...\n");
        system("wget -q -O delivery_order.csv \"https://drive.google.com/uc?export=download&id=1OJfRuLgsBnIBWtdRXbRsD2sG6NhMKOg9\"");
        printf("Download completed.\n");
    }
}

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

        if (name && addr && type) {
            Order *o = &data->orders[data->count++];
            strncpy(o->name, name, NAME_LENGTH);
            strncpy(o->address, addr, ADDR_LENGTH);
            o->type = (strstr(type, "Express") != NULL) ? EXPRESS : REGULER;
            o->status = PENDING;
            strcpy(o->agent, "-");
        }
    }
    fclose(fp);
}

void log_delivery(const char *agent, const Order *o, const char *type) {
    FILE *log = fopen("delivery.log", "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d/%02d/%d %02d:%02d:%02d] [AGENT %s] %s package delivered to %s in %s\n",
        t->tm_mday, t->tm_mon+1, t->tm_year+1900,
        t->tm_hour, t->tm_min, t->tm_sec,
        agent, type, o->name, o->address);
    fclose(log);
}

void deliver_reguler(SharedData *data, const char *target_name) {
    for (int i = 0; i < data->count; ++i) {
        if (data->orders[i].type == REGULER &&
            data->orders[i].status == PENDING &&
            strcmp(data->orders[i].name, target_name) == 0) {

            data->orders[i].status = DELIVERED;
            strcpy(data->orders[i].agent, target_name);
            log_delivery(target_name, &data->orders[i], "Reguler");
            printf("Order untuk %s berhasil dikirim.\n", target_name);
            return;
        }
    }
    printf("Order tidak ditemukan atau telah dikirim\n");
}

void check_status(SharedData *data, const char *target_name) {
    for (int i = 0; i < data->count; ++i) {
        if (strcmp(data->orders[i].name, target_name) == 0) {
            if (data->orders[i].status == DELIVERED)
                printf("Status for %s: Delivered by Agent %s\n", target_name, data->orders[i].agent);
            else
                printf("Status for %s: Pending\n", target_name);
            return;
        }
    }
    printf("Order not found.\n");
}

void list_all(SharedData *data) {
    for (int i = 0; i < data->count; ++i) {
        printf("Order %d: %s - %s\n", i+1, data->orders[i].name,
            data->orders[i].status == DELIVERED ? "Delivered" : "Pending");
    }
}

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

    download_csv_if_needed();

    if (data->count == 0)
        load_orders_from_csv(data);

    if (argc >= 3 && strcmp(argv[1], "-deliver") == 0)
        deliver_reguler(data, argv[2]);
    else if (argc >= 3 && strcmp(argv[1], "-status") == 0)
        check_status(data, argv[2]);
    else if (argc == 2 && strcmp(argv[1], "-list") == 0)
        list_all(data);
    else
        printf("Usage:\n ./dispatcher -deliver [Nama]\n ./dispatcher -status [Nama]\n ./dispatcher -list\n");

    shmdt(data);
    return 0;
}
