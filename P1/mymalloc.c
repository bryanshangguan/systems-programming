#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
#define ALIGNMENT 8
#define HEADER_SIZE (sizeof(Header))

void coalesce_chunks();  // Forward declaration of coalesce_chunks

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


void myfree(void* ptr, char* file, int line) {
    if (!initialized) {
        fprintf(stderr, "free: Heap not initialized (%s:%d)\n", file, line);
        exit(2);
    }

    if (ptr == NULL) {
        fprintf(stderr, "free: Null pointer (%s:%d)\n", file, line);
        return;
    }

    // Calculate the start of the chunk from the pointer (pointer is to the payload)
    Header* chunk_to_free = (Header*)((char*)ptr - HEADER_SIZE);

    // Check if the pointer is within the heap bounds
    if ((void*)chunk_to_free < (void*)heap.bytes || (void*)chunk_to_free >= (void*)(heap.bytes + MEMLENGTH)) {
        fprintf(stderr, "free: Pointer out of heap bounds (%s:%d)\n", file, line);
        exit(2);
    }

    // Check if the chunk is already free
    if (chunk_to_free->is_free) {
        fprintf(stderr, "free: Double free detected (%s:%d)\n", file, line);
        exit(2);
    }

    // Mark the chunk as free
    chunk_to_free->is_free = 1;

    // Coalesce with adjacent free chunks
    coalesce_chunks();
}

void coalesce_chunks() {
    Header* current = (Header*)heap_start;

    while ((void*)current < (void*)(heap.bytes + MEMLENGTH)) {
        Header* next = (Header*)((char*)current + HEADER_SIZE + current->size);

        // Check if the current and next chunks are both free
        if (current->is_free && (void*)next < (void*)(heap.bytes + MEMLENGTH) && next->is_free) {
            // Coalesce the current and next chunks by merging their sizes
            current->size += HEADER_SIZE + next->size;
        } else {
            current = next; // Move to the next chunk
        }
    }
}