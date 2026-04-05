logger.c


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <signal.h>

#define MSG_KEY 1234

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
    printf("\nLogger stopped\n");
    exit(0);
}

int main() {
    signal(SIGTERM, stop);

    printf("Logger started...\n");

    int msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    int fd = open("orders.txt", O_CREAT | O_WRONLY | O_APPEND, 0644);
    if (fd < 0) {
        perror("file open");
        exit(1);
    }

    Message msg;
    char buffer[200];

    while (1) {
        printf("Waiting for message...\n");

        if (msgrcv(msgid, &msg, sizeof(msg) - sizeof(long), 1, 0) == -1) {
            perror("msgrcv");
            continue;
        }

        printf("Message received!\n");

        sprintf(buffer, "OrderID=%d Item=%s Status=%s\n",
                msg.order.order_id,
                msg.order.item,
                msg.status);

        write(fd, buffer, strlen(buffer));

        printf("%s", buffer);
    }
}