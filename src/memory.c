#include <stdlib.h>
#include <alloca.h>
#include "memory.h"


void *dynamic_realloc(void *ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    else if (new_size < old_size) {
        // TODO: shrink chunk
    }
    else if (new_size > old_size) {
        // TODO: make bigger chunk
    }


    void *alloc = realloc(ptr, new_size);
    if (alloc == NULL) {
        exit(1);
    }
    return alloc;
}
