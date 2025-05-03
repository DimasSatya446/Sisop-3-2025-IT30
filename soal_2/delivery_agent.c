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

SharedData *data;

void log_delivery(const char *agent, const Order *o) {
    FILE *log = fopen("delivery.log", "a");
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(log, "[%02d/%02d/%d %02d:%02d:%02d] [AGENT %s] Express package delivered to %s in %s\n",
        t->tm_mday, t->tm_mon+1, t->tm_year+1900,
        t->tm_hour, t->tm_min, t->tm_sec,
        agent, o->name, o->address);
    fclose(log);
}

void *agent_thread(void *arg) {
    char *agent_name = (char *)arg;

    while (1) {
        for (int i = 0; i < data->count; ++i) {
            if (data->orders[i].type == EXPRESS && data->orders[i].status == PENDING) {
                data->orders[i].status = DELIVERED;
                strcpy(data->orders[i].agent, agent_name);
                log_delivery(agent_name, &data->orders[i]);
                sleep(1); // simulasi pengiriman
            }
        }
        sleep(3); // jeda sebelum cek ulang
    }

    return NULL;
}

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

    pthread_t agents[3];
    char *names[] = {"A", "B", "C"};

    for (int i = 0; i < 3; ++i)
        pthread_create(&agents[i], NULL, agent_thread, names[i]);

    for (int i = 0; i < 3; ++i)
        pthread_join(agents[i], NULL);

    shmdt(data);
    return 0;
}
