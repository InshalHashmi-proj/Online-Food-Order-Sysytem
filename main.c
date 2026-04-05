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