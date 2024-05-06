#ifndef atjeh_memory_h
#define atjeh_memory_h
#include "common.h"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2) // if capacity < 8, set it to 8, otherwise we double it

// uint8_t grow_capacity(uint8_t capacity);
void *grow_array(void *ptr, size_t old_size, size_t new_size);


#endif // atjeh_memory_h
