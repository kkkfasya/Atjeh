#ifndef ATJEH_MEMORY_H
#define ATJEH_MEMORY_H
#include "common.h"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2) // if capacity < 8, set it to 8, otherwise we double it

#define GROW_ARRAY(type, pointer, old_size, new_size) \
    (type*)dynamic_realloc(pointer, sizeof(type) * (old_size), \
            sizeof(type) * (new_size))

#define FREE_ARRAY(type, pointer, old_size) \
    dynamic_realloc(pointer, sizeof(type) * (old_size), 0)

/*
  old_size: 	  new_size: 	            Operation:
  0 	          Non‑zero 	              Allocate new block.
  Non‑zero 	    0 	                    Free allocation.
  Non‑zero 	    Smaller than oldSize 	  Shrink existing allocation.
  Non‑zero 	    Larger than oldSize 	  Grow existing allocation.
*/
void *dynamic_realloc(void *ptr, size_t old_size, size_t new_size);


#endif // ATJEH_MEMORY_H
