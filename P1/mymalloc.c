#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

#define MEMLENGTH 4096
#define ALIGNMENT 8
#define HEADER_SIZE (sizeof(Header))

void coalesce_chunks();

static union {
    char bytes[MEMLENGTH];
    double not_used;
} heap;

typedef struct Header {
    size_t size;
    int is_free;
} Header;

static void* heap_start = NULL;
static int initialized = 0;

size_t align_size(size_t size) {
    return (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
}

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

void initialize_heap() {
    if (!initialized) {
        heap_start = (void*)heap.bytes;
        Header* first_chunk = (Header*)heap_start;
        first_chunk->size = MEMLENGTH - HEADER_SIZE;
        first_chunk->is_free = 1;
        initialized = 1;
        atexit(leak_detector);
    }
}

void* mymalloc(size_t size, char* file, int line) {
    if (!initialized) {
        initialize_heap();
    }

    if (size == 0) {
        fprintf(stderr, "malloc: Unable to allocate 0 bytes (%s:%d)\n", file, line);
        return NULL;
    }

    size = align_size(size);
    Header* current = (Header*)heap_start;

    while ((void*)current < (void*)(heap.bytes + MEMLENGTH)) {
        if (current->is_free && current->size >= size) {
            size_t remaining_space = current->size - size;
            current->is_free = 0;
            current->size = size;

            if (remaining_space >= HEADER_SIZE + ALIGNMENT) {
                Header* next_chunk = (Header*)((char*)current + HEADER_SIZE + size);
                next_chunk->size = remaining_space - HEADER_SIZE;
                next_chunk->is_free = 1;
            }
            
            return (void*)((char*)current + HEADER_SIZE);
        }

        current = (Header*)((char*)current + current->size + HEADER_SIZE);
    }

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

    Header* current = (Header*)heap_start;
    int found = 0;

    while ((void*)current < (void*)(heap.bytes + MEMLENGTH)) {
        void* payload_start = (void*)((char*)current + HEADER_SIZE);
        if (payload_start == ptr) {
            found = 1;
            break;
        }
        current = (Header*)((char*)current + current->size + HEADER_SIZE);
    }

    if (!found) {
        fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
        exit(2);
    }

    if (current->is_free) {
        fprintf(stderr, "free: Double free detected (%s:%d)\n", file, line);
        exit(2);
    }

    current->is_free = 1;

    coalesce_chunks();
}

void coalesce_chunks() {
    Header* current = (Header*)heap_start;

    while ((void*)current < (void*)(heap.bytes + MEMLENGTH)) {
        Header* next = (Header*)((char*)current + HEADER_SIZE + current->size);

        if ((void*)next >= (void*)(heap.bytes + MEMLENGTH)) {
            break;
        }

        if (current->is_free && next->is_free) {
            current->size += HEADER_SIZE + next->size;
        } else {
            current = next;
        }
    }
}