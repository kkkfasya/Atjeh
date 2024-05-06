#include <stdlib.h>
#include <alloca.h>
#include "memory.h"

void *chunk_realloc(void *ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    void *alloc = realloc(ptr, new_size);
    if (alloc == NULL) {
        exit(1);
    }
    return alloc;
}
