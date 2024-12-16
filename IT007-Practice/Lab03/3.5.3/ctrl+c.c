#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

void sigintr(int sig) {
    printf("\ncount.sh has stopped\n");
    exit(0);
}

int main() {
    signal(SIGINT, sigintr);

    printf("Welcome to IT007, I am H1k4riLe :3\n");

    pid_t pid = fork();

    if (pid == 0) { 
        execl("./count.sh", "./count.sh", "120", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0)  
        wait(NULL);
    else
        perror("fork failed");
    

    return 0;
}
