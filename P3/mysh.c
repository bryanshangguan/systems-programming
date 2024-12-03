#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#include <stdbool.h>
#include "command_parser.h"
#include "builtins.h"
#include "io.h"
#include "wildcards.h"
#include "utils.h"

#define PROMPT "mysh> "
#define MAX_HISTORY 100

// Globals for command history
char *history[MAX_HISTORY];
int history_count = 0;
int history_index = -1;

// Globals for terminal settings
struct termios original_termios;

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &original_termios);
    raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void add_to_history(const char *command) {
    if (history_count < MAX_HISTORY) {
        history[history_count++] = strdup(command);
    } else {
        free(history[0]);
        memmove(history, history + 1, sizeof(char *) * (MAX_HISTORY - 1));
        history[MAX_HISTORY - 1] = strdup(command);
    }
}

void free_history() {
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }
}

void execute_command(const char *command) {
    Command *cmd = parse_command(command);
    if (!cmd) {
        fprintf(stderr, "Error parsing command\n");
        return;
    }

    if (is_builtin(cmd->name)) {
        int status = execute_builtin(cmd);
        free_command(cmd);
        if (status == 1) { // Exit command
            printf("Exiting my shell.\n");
            exit(0);
        }
        return;
    } else if (strchr(command, '|')) { // Pipeline handling
        int pipefd[2];
        if (pipe(pipefd) == -1) {
            perror("pipe");
            free_command(cmd);
            return;
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            if (exec_command(cmd) == -1) {
                perror("exec failed");
                exit(EXIT_FAILURE);
            }
        }

        Command *cmd2 = parse_command(strchr(command, '|') + 1);
        pid_t pid2 = fork();
        if (pid2 == 0) {
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            if (exec_command(cmd2) == -1) {
                perror("exec failed");
                exit(EXIT_FAILURE);
            }
        }

        close(pipefd[0]);
        close(pipefd[1]);
        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
        free_command(cmd2);
    } else {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
        } else if (pid == 0) {
            if (setup_io(cmd) != 0) exit(EXIT_FAILURE);
            if (exec_command(cmd) == -1) {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                printf("Command failed with code %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Terminated by signal: %s\n", strsignal(WTERMSIG(status)));
            }
        }
    }

    free_command(cmd);
}

void run_interactive_mode() {
    enable_raw_mode();
    char buffer[1024];
    size_t len = 0;

    printf("Welcome to my shell!\n");

    while (1) {
        printf(PROMPT);
        fflush(stdout);

        len = 0;
        buffer[0] = '\0';

        while (1) {
            char c;
            read(STDIN_FILENO, &c, 1);

            if (c == '\n') {
                if (len > 0) {
                    buffer[len] = '\0';
                    printf("\n"); // Print a newline before executing the command
                    add_to_history(buffer);
                    history_index = history_count;
                    execute_command(buffer);
                }
                break;
            } else if (c == 127) { // Backspace
                if (len > 0) {
                    len--;
                    printf("\b \b");
                    fflush(stdout);
                }
            } else if (c == '\033') { // Arrow keys
                read(STDIN_FILENO, &c, 1); // skip [
                read(STDIN_FILENO, &c, 1);
                if (c == 'A') { // Up
                    if (history_index > 0) {
                        history_index--;
                        printf("\r%s%s", PROMPT, history[history_index]);
                        strcpy(buffer, history[history_index]);
                        len = strlen(buffer);
                    }
                } else if (c == 'B') { // Down
                    if (history_index < history_count - 1) {
                        history_index++;
                        printf("\r%s%s", PROMPT, history[history_index]);
                        strcpy(buffer, history[history_index]);
                        len = strlen(buffer);
                    } else {
                        history_index = history_count;
                        printf("\r%s", PROMPT);
                        buffer[0] = '\0';
                        len = 0;
                    }
                }
            } else { // Regular character
                buffer[len++] = c;
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }

    disable_raw_mode();
    printf("Exiting my shell.\n");
}

void run_batch_mode(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening batch file");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    while (fgets(buffer, sizeof(buffer), file)) {
        buffer[strcspn(buffer, "\n")] = '\0';
        execute_command(buffer);
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    atexit(disable_raw_mode);
    atexit(free_history);

    if (argc == 2) {
        run_batch_mode(argv[1]);
    } else {
        run_interactive_mode();
    }

    return 0;
}

