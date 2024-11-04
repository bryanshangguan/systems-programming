#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

// Structure to hold a word and its count
typedef struct WordCount {
    char *word;
    int count;
    struct WordCount *next;
} WordCount;

WordCount *head = NULL;

// Function to add or update the word count
void add_word(const char *word) {
    WordCount *current = head;
    WordCount *prev = NULL;

    // Check if the word already exists in the list
    while (current != NULL) {
        if (strcmp(current->word, word) == 0) {
            current->count++;
            return;
        }
        prev = current;
        current = current->next;
    }

    // Create a new word node if not found
    WordCount *new_word = malloc(sizeof(WordCount));
    new_word->word = strdup(word);
    new_word->count = 1;
    new_word->next = NULL;

    if (prev == NULL) {
        head = new_word;
    } else {
        prev->next = new_word;
    }
}

// Helper function to check if a filename ends with .txt
int is_txt_file(const char *filename) {
    const char *ext = strrchr(filename, '.');
    return (ext != NULL && strcmp(ext, ".txt") == 0);
}

// Helper function to determine if a file or directory should be ignored
int should_ignore(const char *name) {
    return (name[0] == '.');
}

// Function to process a single file and count word occurrences
void process_file(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    char buffer[4096];
    ssize_t bytes_read;
    char word[256];
    int word_index = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            if (isalpha(buffer[i]) || buffer[i] == '\'' || (buffer[i] == '-' && word_index > 0 && isalpha(buffer[i + 1]))) {
                word[word_index++] = buffer[i];
            } else {
                if (word_index > 0) {
                    word[word_index] = '\0';
                    add_word(word);
                    word_index = 0;
                }
            }
        }
    }

    if (word_index > 0) {
        word[word_index] = '\0';
        add_word(word);
    }

    close(fd);
}

// Function to recursively process directories and process .txt files
void process_directory(const char *dirname) {
    DIR *dir = opendir(dirname);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (should_ignore(entry->d_name)) {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        struct stat path_stat;
        if (stat(path, &path_stat) == -1) {
            perror("Error getting file status");
            continue;
        }

        if (S_ISDIR(path_stat.st_mode)) {
            process_directory(path);
        } else if (S_ISREG(path_stat.st_mode) && is_txt_file(entry->d_name)) {
            process_file(path);
        }
    }

    closedir(dir);
}

// Function to print the sorted word list and counts
void print_word_counts(void) {
    // Convert linked list to array for sorting
    int count = 0;
    WordCount *current = head;
    while (current != NULL) {
        count++;
        current = current->next;
    }

    WordCount **array = malloc(count * sizeof(WordCount *));
    current = head;
    for (int i = 0; i < count; i++) {
        array[i] = current;
        current = current->next;
    }

    // Sort the array by count (descending) and lexicographically for ties
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (array[i]->count < array[j]->count || 
                (array[i]->count == array[j]->count && strcmp(array[i]->word, array[j]->word) > 0)) {
                WordCount *temp = array[i];
                array[i] = array[j];
                array[j] = temp;
            }
        }
    }

    // Print the sorted words and their counts
    for (int i = 0; i < count; i++) {
        printf("%s %d\n", array[i]->word, array[i]->count);
        free(array[i]->word);
        free(array[i]);
    }

    free(array);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file or directory>...\n", argv[0]);
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; i++) {
        struct stat path_stat;
        if (stat(argv[i], &path_stat) == -1) {
            perror("Error getting file status");
            continue;
        }

        if (S_ISDIR(path_stat.st_mode)) {
            process_directory(argv[i]);
        } else if (S_ISREG(path_stat.st_mode)) {
            process_file(argv[i]);
        } else {
            fprintf(stderr, "Skipping non-regular file: %s\n", argv[i]);
        }
    }

    print_word_counts();
    return EXIT_SUCCESS;
}
