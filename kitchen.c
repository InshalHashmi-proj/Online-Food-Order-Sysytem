kitchen.c


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define MSG_KEY 1234

// Structures
typedef struct {
    int order_id;
    char item[20];
} Order;

typedef struct {
    long type;
    Order order;
    char status[20];
} Message;

void stop(int sig) {
    exit(0);
}

int main() {
    signal(SIGTERM, stop);

    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);

    Order o;
    Message msg;
    msg.type = 1;

    while (1) {
        read(STDIN_FILENO, &o, sizeof(o));

        msg.order = o;

        sleep(1); // simulate cooking

        strcpy(msg.status, "READY");

        msgsnd(msgid, &msg, sizeof(msg) - sizeof(long), 0);
    }
}