2. order_gen.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

// Structure
typedef struct {
    int order_id;
    char item[20];
} Order;

pthread_mutex_t lock;
int order_id = 1;

void* generate(void* arg) {
    char* item = (char*)arg;

    while (1) {
        Order o;

        pthread_mutex_lock(&lock);
        o.order_id = order_id++;
        pthread_mutex_unlock(&lock);

        strcpy(o.item, item);

       printf("Generated Order: ID=%d Item=%s\n", o.order_id, o.item);
       write(STDOUT_FILENO, &o, sizeof(o));
        sleep(1);
    }
}

void stop(int sig) {
    exit(0);
}

int main() {
    signal(SIGTERM, stop);

    pthread_t t1, t2, t3;
    pthread_mutex_init(&lock, NULL);

    pthread_create(&t1, NULL, generate, "Pizza");
    pthread_create(&t2, NULL, generate, "Burger");
    pthread_create(&t3, NULL, generate, "Fries");

    pthread_join(t1, NULL);
    return 0;
}








3. kitchen.c
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








4. logger.c
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





main.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

pid_t p1, p2, p3;

void cleanup(int sig) {
    printf("\nShutting down...\n");

    kill(p1, SIGTERM);
    kill(p2, SIGTERM);
    kill(p3, SIGTERM);

    while (wait(NULL) > 0);

    printf("Shutdown complete\n");
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    int fd[2];
    pipe(fd);

    // Order Generator
    p1 = fork();
    if (p1 == 0) {
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        execl("./order_gen", "order_gen", NULL);
        perror("exec order_gen");
        exit(1);
    }

    // Kitchen
    p2 = fork();
    if (p2 == 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
        execl("./kitchen", "kitchen", NULL);
        perror("exec kitchen");
        exit(1);
    }

    // Logger
    p3 = fork();
    if (p3 == 0) {
        execl("./logger", "logger", NULL);
        perror("exec logger");
        exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    while (1) pause();
}