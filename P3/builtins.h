#ifndef BUILTINS_H
#define BUILTINS_H

#include "command_parser.h"
#include <stdbool.h>

/**
 * Checks if a command is a built-in.
 *
 * @param name The name of the command.
 * @return true if the command is a built-in, false otherwise.
 */
bool is_builtin(const char *name);

/**
 * Executes a built-in command.
 *
 * @param cmd The Command structure for the built-in.
 * @return 0 on success, -1 on failure.
 */
int execute_builtin(Command *cmd);

#endif // BUILTINS_H

