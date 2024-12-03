#include "io.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int setup_io(Command *cmd) {
    if (cmd->input_file) {
        int fd_in = open(cmd->input_file, O_RDONLY);
        if (fd_in == -1) {
            perror("Error opening input file");
            return -1;
        }
        if (dup2(fd_in, STDIN_FILENO) == -1) {
            perror("Error redirecting input");
            close(fd_in);
            return -1;
        }
        close(fd_in);
    }

    if (cmd->output_file) {
        int fd_out = open(cmd->output_file, O_WRONLY | O_CREAT | O_TRUNC, 0640);
        if (fd_out == -1) {
            perror("Error opening output file");
            return -1;
        }
        if (dup2(fd_out, STDOUT_FILENO) == -1) {
            perror("Error redirecting output");
            close(fd_out);
            return -1;
        }
        close(fd_out);
    }

    return 0;
}

int exec_command(Command *cmd) {
    if (!cmd || !cmd->name) {
        fprintf(stderr, "Invalid command\n");
        return -1;
    }

    if (execvp(cmd->name, cmd->args) == -1) {
        perror("execvp failed");
        return -1;
    }

    return 0;
}

