#ifndef WILDCARDS_H
#define WILDCARDS_H

/**
 * Expands wildcard patterns in a given string.
 *
 * @param pattern The pattern containing a wildcard.
 * @param expanded Pointer to store expanded results.
 * @param count Pointer to store the number of results.
 */
void expand_wildcards(const char *pattern, char ***expanded, int *count);

#endif // WILDCARDS_H

