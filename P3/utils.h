#ifndef UTILS_H
#define UTILS_H

void print_error(const char *msg);
char *str_trim(char *str);
char **split_string(const char *str, const char *delim, int *count);
void free_string_array(char **arr, int count);

#endif

