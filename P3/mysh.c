#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "command_parser.h" // Header for parsing commands
#include "builtins.h"       // Header for built-in commands
#include "io.h"             // Header for input/output utilities
#include "wildcards.h"      // Header for wildcard expansion
#include "utils.h"          // General utility functions

#define PROMPT "mysh> " // Prompt for interactive mode

void run_interactive_mode();
void run_batch_mode(const char *filename);
void execute_command(char *command);

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s [batch_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 2) {
        run_batch_mode(argv[1]);
    } else {
        run_interactive_mode();
    }

    return 0;
}

void run_interactive_mode() {
    char *line = NULL;
    size_t len = 0;

    printf("Welcome to my shell!\n");

    while (1) {
        printf(PROMPT);

        if (getline(&line, &len, stdin) == -1) {
            // Handle end of input (Ctrl+D)
            break;
        }

        if (strlen(line) > 1) { // Ignore empty lines
            execute_command(line);
        }
    }

    free(line);
    printf("Exiting my shell.\n");
}

void run_batch_mode(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening batch file");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        if (strlen(line) > 1) { // Ignore empty lines
            execute_command(line);
        }
    }

    free(line);
    fclose(file);
}

void execute_command(char *command) {
    // Parse the command into tokens
    Command *cmd = parse_command(command);
    if (!cmd) {
        fprintf(stderr, "Error parsing command\n");
        return;
    }

    // Check for built-in commands
    if (is_builtin(cmd->name)) {
        if (execute_builtin(cmd) != 0) {
            fprintf(stderr, "Error executing built-in command: %s\n", cmd->name);
        }
    } else {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
        } else if (pid == 0) {
            // In child process
            if (setup_io(cmd) != 0) {
                exit(EXIT_FAILURE);
            }
            if (exec_command(cmd) == -1) {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        } else {
            // In parent process, wait for child to finish
            int status;
            waitpid(pid, &status, 0);

            if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
                printf("Command failed with code %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Terminated by signal: %s\n", strsignal(WTERMSIG(status)));
            }
        }
    }

    free_command(cmd); // Clean up dynamically allocated command structure
}
