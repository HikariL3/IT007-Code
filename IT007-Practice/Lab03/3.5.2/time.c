#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        return 1;
    }

    struct timeval start, end;
    pid_t pid = fork();

    if (pid == 0) { 
        gettimeofday(&start, NULL);
        execl("/bin/sh", "sh", "-c", argv[1], (char *)NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);
    } 
    else if (pid > 0) { 
        wait(NULL);
        gettimeofday(&end, NULL);

        double elapsed_time = (end.tv_sec - start.tv_sec) + 
                              (end.tv_usec - start.tv_usec) / 1e6;
        printf("Execution time: %.6f seconds\n", elapsed_time);
    } 
    else 
        perror("fork failed");
    return 0;
}
