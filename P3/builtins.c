#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool is_builtin(const char *name) {
    const char *builtins[] = {"cd", "pwd", "exit", "which"};
    size_t num_builtins = sizeof(builtins) / sizeof(builtins[0]);

    for (size_t i = 0; i < num_builtins; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

int execute_builtin(Command *cmd) {
    if (strcmp(cmd->name, "cd") == 0) {
        if (!cmd->args[1]) { // No argument provided
            fprintf(stderr, "cd: missing argument\n");
            return -1;
        }
        if (chdir(cmd->args[1]) == -1) { // Change directory
            perror("cd failed");
            return -1;
        }
        return 0;
    } else if (strcmp(cmd->name, "pwd") == 0) {
        char cwd[1024];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s\n", cwd);
        } else {
            perror("pwd failed");
        }
        return 0;
    } else if (strcmp(cmd->name, "exit") == 0) {
        return 1; // Indicate that the shell should terminate
    } else if (strcmp(cmd->name, "which") == 0) {
        if (!cmd->args[1]) {
            fprintf(stderr, "Usage: which <command>\n");
            return -1;
        }
        const char *paths[] = {"/usr/local/bin", "/usr/bin", "/bin"};
        for (size_t i = 0; i < 3; i++) {
            char full_path[1024];
            snprintf(full_path, sizeof(full_path), "%s/%s", paths[i], cmd->args[1]);
            if (access(full_path, X_OK) == 0) {
                printf("%s\n", full_path);
                return 0;
            }
        }
        fprintf(stderr, "Command not found: %s\n", cmd->args[1]);
        return -1;
    }
    return 0;
}

