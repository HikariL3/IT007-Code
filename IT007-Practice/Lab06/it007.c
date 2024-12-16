// File: shell_interface.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_LINE 80  // The maximum length of a command
#define HISTORY_SIZE 10  // Size of the command history buffer

char *history[HISTORY_SIZE];  // History buffer
int history_count = 0;  // Total number of commands entered
int should_run = 1;  // Flag to determine when to exit the shell

void display_prompt() {
    printf("it007sh> ");
    fflush(stdout);
}

void add_to_history(const char *command) {
    if (history_count < HISTORY_SIZE) {
        history[history_count] = strdup(command);
    } else {
        free(history[0]);
        for (int i = 1; i < HISTORY_SIZE; i++) {
            history[i - 1] = history[i];
        }
        history[HISTORY_SIZE - 1] = strdup(command);
    }
    history_count++;
}

void print_history() {
    int start = history_count > HISTORY_SIZE ? history_count - HISTORY_SIZE : 0;
    for (int i = start; i < history_count; i++) {
        printf("%d %s\n", i + 1, history[i % HISTORY_SIZE]);
    }
}

void execute_command(char **args, int background) {
    pid_t pid = fork();

    if (pid == 0) {  // Child process
        execvp(args[0], args);
        perror("Execution failed");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        perror("Fork failed");
    } else {  // Parent process
        if (!background) {
            waitpid(pid, NULL, 0);
        }
    }
}

void handle_redirection(char **args) {
    int in_redirect = -1, out_redirect = -1;
    char *input_file = NULL, *output_file = NULL;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0) {
            input_file = args[i + 1];
            in_redirect = i;
        } else if (strcmp(args[i], ">") == 0) {
            output_file = args[i + 1];
            out_redirect = i;
        }
    }

    if (in_redirect != -1) {
        int fd = open(input_file, O_RDONLY);
        if (fd < 0) {
            perror("Input redirection failed");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
        args[in_redirect] = NULL;
    }

    if (out_redirect != -1) {
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("Output redirection failed");
            exit(EXIT_FAILURE);
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
        args[out_redirect] = NULL;
    }
}

void handle_pipe(char **args) {
    int pipe_index = -1;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "|") == 0) {
            pipe_index = i;
            break;
        }
    }

    if (pipe_index != -1) {
        args[pipe_index] = NULL;
        char **left = args;
        char **right = &args[pipe_index + 1];

        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("Pipe failed");
            exit(EXIT_FAILURE);
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {  // First child
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            execvp(left[0], left);
            perror("Execution failed");
            exit(EXIT_FAILURE);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {  // Second child
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);
            execvp(right[0], right);
            perror("Execution failed");
            exit(EXIT_FAILURE);
        }

        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }
}

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nTerminated command with Ctrl+C\n");
        display_prompt();
        fflush(stdout);
    }
}

int main() {
    signal(SIGINT, signal_handler);

    char input[MAX_LINE];
    char *args[MAX_LINE / 2 + 1];

    while (should_run) {
        display_prompt();

        if (fgets(input, MAX_LINE, stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = '\0';  // Remove trailing newline

        if (strcmp(input, "exit") == 0) {
            should_run = 0;
            continue;
        }

        add_to_history(input);

        char *token = strtok(input, " ");
        int index = 0;

        while (token != NULL) {
            args[index++] = token;
            token = strtok(NULL, " ");
        }
        args[index] = NULL;

        if (strchr(input, '|')) {
            handle_pipe(args);
        } else if (strchr(input, '<') || strchr(input, '>')) {
            pid_t pid = fork();
            if (pid == 0) {
                handle_redirection(args);
                execvp(args[0], args);
                perror("Execution failed");
                exit(EXIT_FAILURE);
            } else {
                waitpid(pid, NULL, 0);
            }
        } else {
            execute_command(args, 0);
        }
    }

    // Cleanup
    for (int i = 0; i < HISTORY_SIZE; i++) {
        free(history[i]);
    }

    return 0;
}
