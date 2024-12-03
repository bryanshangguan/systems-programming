#include "command_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

Command *parse_command(const char *input) {
    Command *cmd = malloc(sizeof(Command));
    if (!cmd) return NULL;

    cmd->name = NULL;
    cmd->args = NULL;
    cmd->input_file = NULL;
    cmd->output_file = NULL;

    char *copy = strdup(input);
    char *token;
    int arg_count = 0;

    token = strtok(copy, " \t\n");
    while (token) {
        if (strcmp(token, "<") == 0) {
            token = strtok(NULL, " \t\n");
            if (token) cmd->input_file = strdup(token);
        } else if (strcmp(token, ">") == 0) {
            token = strtok(NULL, " \t\n");
            if (token) cmd->output_file = strdup(token);
        } else if (!cmd->name) {
            cmd->name = strdup(token);
            cmd->args = realloc(cmd->args, sizeof(char *) * (arg_count + 1));
            cmd->args[arg_count++] = strdup(token); // Include command name as first argument
        } else {
            cmd->args = realloc(cmd->args, sizeof(char *) * (arg_count + 1));
            cmd->args[arg_count++] = strdup(token);
        }
        token = strtok(NULL, " \t\n");
    }

    cmd->args = realloc(cmd->args, sizeof(char *) * (arg_count + 1));
    cmd->args[arg_count] = NULL; // Ensure args is NULL-terminated

    free(copy);
    return cmd;
}

void free_command(Command *cmd) {
    if (!cmd) return;
    free(cmd->name);
    if (cmd->args) {
        for (int i = 0; cmd->args[i]; i++) {
            free(cmd->args[i]);
        }
        free(cmd->args);
    }
    free(cmd->input_file);
    free(cmd->output_file);
    free(cmd);
}

