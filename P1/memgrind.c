#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

#define NUM_RUNS 50
#define NUM_ALLOC 120

double time_difference(struct timeval start, struct timeval end) {
    return (double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_usec - start.tv_usec) / 1000.0;
}

void task1() {
    for (int i = 0; i < NUM_ALLOC; i++) {
        char *ptr = (char *)malloc(1);
        free(ptr);
    }
}

void task2() {
    char *pointers[NUM_ALLOC];
    for (int i = 0; i < NUM_ALLOC; i++) {
        pointers[i] = (char *)malloc(1);
    }
    
    for (int i = 0; i < NUM_ALLOC; i++) {
        free(pointers[i]);
    }
}

void task3() {
    char *pointers[NUM_ALLOC] = {NULL};
    int allocations = 0;
    int frees = 0;

    while (allocations < NUM_ALLOC) {
        int action = rand() % 2;

        if (action == 0 && allocations < NUM_ALLOC) {
            pointers[allocations++] = (char *)malloc(1);
        } else if (action == 1 && frees < allocations) {
            free(pointers[frees++]);
        }
    }

    for (int i = frees; i < allocations; i++) {
        free(pointers[i]);
    }
}

void task4() {
    char *pointers[10];
    for (int i = 0; i < 10; i++) {
        pointers[i] = (char *)malloc(100);
    }
    
    for (int i = 0; i < 10; i++) {
        free(pointers[i]);
    }
}

void task5() {
    for (int i = 0; i < NUM_ALLOC; i++) {
        char *ptr = (char *)malloc((i % 10) + 1);
        free(ptr);
    }
}

void test_invalid_free_non_malloc() {
    int x;
    free(&x);
}

void test_invalid_free_not_chunk_start() {
    int *p = (int *)malloc(sizeof(int) * 2);
    free(p + 1);
    free(p);
}

void test_double_free() {
    int *p = (int *)malloc(sizeof(int) * 100);
    free(p);
    free(p);
}

int main() {
    struct timeval start, end;
    double total_time = 0.0;

    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task1();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 1: %f ms\n", total_time / NUM_RUNS);

    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task2();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 2: %f ms\n", total_time / NUM_RUNS);

    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task3();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 3: %f ms\n", total_time / NUM_RUNS);

    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task4();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 4: %f ms\n", total_time / NUM_RUNS);

    gettimeofday(&start, NULL);
    for (int i = 0; i < NUM_RUNS; i++) {
        task5();
    }
    gettimeofday(&end, NULL);
    total_time = time_difference(start, end);
    printf("Task 5: %f ms\n", total_time / NUM_RUNS);

    printf("\nRunning error detection tests:\n");

    printf("Test 1: Freeing non-malloc memory\n");
    test_invalid_free_non_malloc();

    printf("Test 2: Freeing pointer not at start of chunk\n");
    test_invalid_free_not_chunk_start();

    printf("Test 3: Double free\n");
    test_double_free();

    return 0;
}
