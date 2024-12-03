#ifndef IO_H
#define IO_H

#include "command_parser.h"

/**
 * Sets up input and output redirection for the given command.
 *
 * @param cmd The command containing redirection information.
 * @return 0 on success, -1 on failure.
 */
int setup_io(Command *cmd);

/**
 * Executes an external command using execv.
 *
 * @param cmd The command structure to execute.
 * @return -1 if execv fails (it does not return on success).
 */
int exec_command(Command *cmd);

#endif // IO_H

