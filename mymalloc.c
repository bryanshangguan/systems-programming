#include "mymalloc.h"
#include <stdio.h>
#include <stdlib.h>

#define MEMLENGTH 4096
#define ALIGNMENT 8
#define HEADER_SIZE (sizeof(Header))

// Union to enforce alignment
static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

// Metadata structure for each chunk
typedef struct Header {
    size_t size; // Size of the chunk (including header)
    int is_free; // Is the chunk free?
} Header;

// Start of the heap
static void* heap_start = NULL;
static int initialized = 0;

// Helper to align size to the nearest multiple of 8
size_t align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

// Function to initialize the heap
void initialize_heap() {
    if (!initialized) {
        heap_start = (void*)heap.bytes;
        Header* first_chunk = (Header*)heap_start;
        first_chunk->size = MEMLENGTH - HEADER_SIZE;
        first_chunk->is_free = 1; // Entire heap is free initially
        initialized = 1;
        atexit(leak_detector); // Register leak detector
    }
}

// Function to detect memory leaks at program exit
void leak_detector() {
    Header* current = (Header*)heap_start;
    size_t leaked_bytes = 0;
    int leaked_chunks = 0;
    
    while ((void*)current < (void*)(heap.bytes + MEMLENGTH)) {
        if (!current->is_free) {
            leaked_bytes += current->size;
            leaked_chunks++;
        }
        current = (Header*)((char*)current + current->size + HEADER_SIZE);
    }
    
    if (leaked_chunks > 0) {
        fprintf(stderr, "mymalloc: %zu bytes leaked in %d objects.\n", leaked_bytes, leaked_chunks);
    }
}

// mymalloc implementation
void* mymalloc(size_t size, char* file, int line) {
    if (!initialized) {
        initialize_heap();
    }

    if (size == 0) {
        fprintf(stderr, "malloc: Unable to allocate 0 bytes (%s:%d)\n", file, line);
        return NULL;
    }

    size = align_size(size); // Align requested size
    Header* current = (Header*)heap_start;

    // Traverse through the heap to find a free chunk
    while ((void*)current < (void*)(heap.bytes + MEMLENGTH)) {
        if (current->is_free && current->size >= size) {
            size_t remaining_space = current->size - size;
            current->is_free = 0;
            current->size = size;

            // Split the chunk if there is leftover space
            if (remaining_space >= HEADER_SIZE + ALIGNMENT) {
                Header* next_chunk = (Header*)((char*)current + HEADER_SIZE + size);
                next_chunk->size = remaining_space - HEADER_SIZE;
                next_chunk->is_free = 1;
            }

            // Return the payload (the part after the header)
            return (void*)((char*)current + HEADER_SIZE);
        }

        current = (Header*)((char*)current + current->size + HEADER_SIZE);
    }

    // If we reach here, no suitable chunk was found
    fprintf(stderr, "malloc: Unable to allocate %zu bytes (%s:%d)\n", size, file, line);
    return NULL;
}
