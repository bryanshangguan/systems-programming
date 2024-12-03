#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_error(const char *msg) {
    fprintf(stderr, "%s\n", msg);
}

char *str_trim(char *str) {
    char *end;

    // Trim leading space
    while (*str == ' ') str++;

    if (*str == 0) return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;

    *(end + 1) = 0;
    return str;
}

char **split_string(const char *str, const char *delim, int *count) {
    char *copy = strdup(str);
    char *token = strtok(copy, delim);
    char **tokens = NULL;
    *count = 0;

    while (token) {
        tokens = realloc(tokens, sizeof(char *) * (*count + 1));
        tokens[*count] = strdup(token);
        (*count)++;
        token = strtok(NULL, delim);
    }

    free(copy);
    return tokens;
}

void free_string_array(char **arr, int count) {
    for (int i = 0; i < count; i++) {
        free(arr[i]);
    }
    free(arr);
}

