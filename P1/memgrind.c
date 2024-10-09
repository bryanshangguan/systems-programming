#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>  // For gettimeofday()
#include "mymalloc.h"

#define NUM_RUNS 50  // Number of runs for each test
#define NUM_ALLOC 120  // Number of allocations

// Function to measure the time taken to execute a workload
double time_difference(struct timeval start, struct timeval end) {
    return (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
}

void task1() {
    // malloc() and free() a 1-byte object, 120 times
    for (int i = 0; i < NUM_ALLOC; i++) {
        char *ptr = (char *)malloc(1);  // Allocating 1 byte
        free(ptr);
    }
}

void task2() {
    // Use malloc() to get 120 1-byte objects, then free() them
    char *pointers[NUM_ALLOC];
    for (int i = 0; i < NUM_ALLOC; i++) {
        pointers[i] = (char *)malloc(1);
    }
    for (int i = 0; i < NUM_ALLOC; i++) {
        free(pointers[i]);
    }
}

void task3() {
    // Randomly allocate/free memory until 120 allocations are made
    char *pointers[NUM_ALLOC] = {NULL};
    int allocations = 0;
    int frees = 0;

    while (allocations < NUM_ALLOC) {
        int action = rand() % 2;  // Randomly choose between malloc (0) and free (1)

        if (action == 0 && allocations < NUM_ALLOC) {
            pointers[allocations++] = (char *)malloc(1);
        } else if (action == 1 && frees < allocations) {
            free(pointers[frees++]);
        }
    }

    // Free remaining allocated memory
    for (int i = frees; i < allocations; i++) {
        free(pointers[i]);
    }
}

// Additional tasks (task 4 and task 5) can be defined here.
void task4() {
    // Allocate and free 10 large chunks of memory (100 bytes each)
    char *pointers[10];
    for (int i = 0; i < 10; i++) {
        pointers[i] = (char *)malloc(100);
    }
    for (int i = 0; i < 10; i++) {
        free(pointers[i]);
    }
}

void task5() {
    // Repeatedly allocate and free memory of varying sizes
    for (int i = 0; i < NUM_ALLOC; i++) {
        char *ptr = (char *)malloc((i % 10) + 1);  // Allocating varying sizes (1 to 10 bytes)
        free(ptr);
    }
}

int main() {
    struct timeval start, end;
    double total_time = 0.0;

    // Task 1: malloc() and immediately free() a 1-byte object, 120 times
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task1();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 1: %f ms\n", total_time / NUM_RUNS);

    // Task 2: Use malloc() to get 120 1-byte objects, then free them
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task2();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 2: %f ms\n", total_time / NUM_RUNS);

    // Task 3: Randomly allocate and free until 120 allocations are made
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task3();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 3: %f ms\n", total_time / NUM_RUNS);

    // Task 4: Allocate and free 10 large chunks of memory (100 bytes each)
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task4();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 4: %f ms\n", total_time / NUM_RUNS);

    // Task 5: Repeatedly allocate and free memory of varying sizes
    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task5();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 5: %f ms\n", total_time / NUM_RUNS);

    return 0;
}
