#include "wildcards.h"
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void expand_wildcards(const char *pattern, char ***expanded, int *count) {
    char *directory = strdup(pattern);
    char *wildcard = strrchr(directory, '*');
    if (!wildcard) {
        *expanded = malloc(sizeof(char *));
        (*expanded)[0] = strdup(pattern);
        *count = 1;
        free(directory);
        return;
    }

    *wildcard = '\0';
    DIR *dir = opendir(directory[0] ? directory : ".");
    if (!dir) {
        perror("opendir failed");
        *expanded = malloc(sizeof(char *));
        (*expanded)[0] = strdup(pattern);
        *count = 1;
        free(directory);
        return;
    }

    struct dirent *entry;
    *expanded = NULL;
    *count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.' && pattern[0] != '.') {
            continue; // Skip hidden files
        }
        if (strstr(entry->d_name, wildcard + 1)) {
            *expanded = realloc(*expanded, sizeof(char *) * (*count + 1));
            (*expanded)[*count] = strdup(entry->d_name);
            (*count)++;
        }
    }

    closedir(dir);
    free(directory);
}

