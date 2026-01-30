#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

#define HEAP_SIZE 1024
static uint8_t memory_pool[HEAP_SIZE];
static size_t offset = 0;

void* my_malloc(size_t size) {
    
    size_t aligned_offset = (offset + 7) & ~7;

    if (aligned_offset + size > HEAP_SIZE) {
        printf("Error: Out of memory!\n");
        return NULL;
    }
    void* ptr = &memory_pool[aligned_offset];
    offset = aligned_offset + size;

    printf("Allocated %zu bytes. Offset moved from %zu to %zu\n", size, aligned_offset, offset);
    return ptr;
}

void my_free_all() {
    offset = 0;
    
}

int main() {
    
    char* c = (char*)my_malloc(sizeof(char));
    int* i = (int*)my_malloc(sizeof(int));
    my_free_all();

    return 0;
}