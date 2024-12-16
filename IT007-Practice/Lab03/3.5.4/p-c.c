#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>

#define SIZE 10

int main() {
    const char *name = "shared_buffer";
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, SIZE);
    char *buffer = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    pid_t pid = fork();
    if (pid == 0) { // Consumer
        int sum = 0;
        while (1) {
            if (buffer[0] != '\0') {
                int value = atoi(buffer);
                printf("Consumer: Read %d\n", value);
                sum += value;
                memset(buffer, 0, SIZE); // Clear buffer
                if (sum > 100) {
                    printf("Consumer: Total = %d, stopping...\n", sum);
                    break;
                }
            }
        }
    } 
    else if (pid > 0) { // Producer
        srand(time(NULL));
        while (1) {
            if (buffer[0] == '\0') {
                int value = rand() % 11 + 10;
                printf("Producer: Write %d\n", value);
                snprintf(buffer, SIZE, "%d", value);
                if (waitpid(pid, NULL, WNOHANG) > 0) {
                    break;
                }
            }
        }
    } 
    else 
        perror("fork failed");

    munmap(buffer, SIZE);
    shm_unlink(name);
    return 0;
}
