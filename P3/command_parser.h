#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

typedef struct Command {
    char *name;       // Command name
    char **args;      // Arguments array
    char *input_file; // Input redirection file
    char *output_file;// Output redirection file
} Command;

/**
 * Parses a raw input string into a Command structure.
 *
 * @param input The raw input string.
 * @return A pointer to the dynamically allocated Command structure.
 */
Command *parse_command(const char *input);

/**
 * Frees the memory associated with a Command structure.
 *
 * @param cmd The Command structure to free.
 */
void free_command(Command *cmd);

#endif // COMMAND_PARSER_H

