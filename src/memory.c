#include <stdlib.h>
#include "memory.h"

void *grow_array(void *ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    void *alloc = realloc(ptr, new_size);
    return alloc;
}
