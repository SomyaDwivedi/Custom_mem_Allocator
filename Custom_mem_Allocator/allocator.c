#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE 1024
static uint8_t memory_pool[HEAP_SIZE];
static size_t offset = 0;

typedef struct Block {
    size_t size;
    int is_free;
    struct Block* next;
} Block;

void* my_malloc(size_t size) {
    size_t total_size = sizeof(Block) + size;
    total_size = (total_size + 7) & ~7; 

    Block* current = (Block*)memory_pool;
    Block* prev = NULL;

    //Search for a free block
    while (current != NULL) {
        // We need to check if we are within the initialized heap bounds
        // (In a real OS, we'd check memory boundaries more strictly)
        if ((uint8_t*)current >= &memory_pool[offset]) {
            break; // We reached the uninitialized part of the heap
        }

        if (current->is_free && current->size >= size) {
            current->is_free = 0;
            printf("Reusing block of size %zu\n", current->size);
            return (void*)(current + 1);
        }
        prev = current;
        current = current->next;
    }

    //No free block found. Create a new one.
    if (offset + total_size > HEAP_SIZE) {
        printf("Out of memory!\n");
        return NULL;
    }

    Block* header = (Block*)&memory_pool[offset];
    header->size = size;
    header->is_free = 0;
    header->next = NULL;

    //Link the previous block to this new one!
    if (prev != NULL) {
        prev->next = header;
    }

    offset += total_size;
    printf("Allocated new block of size %zu\n", size);
    return (void*)(header + 1);
}

void my_free(void* ptr) {
    if (ptr == NULL) return;
    Block* header = (Block*)ptr - 1;
    header->is_free = 1;
    printf("Freed block of size %zu\n", header->size);
}

int main() {
    //Allocate int
    int* a = (int*)my_malloc(sizeof(int));
    *a = 10;

    //Allocate char
    char* b = (char*)my_malloc(sizeof(char));
    *b = 'Z';

    //Free the int (creates a hole)
    my_free(a);

    //Allocate another int (should REUSE the hole from 'a')
    int* c = (int*)my_malloc(sizeof(int));
    *c = 20;

    return 0;
}   