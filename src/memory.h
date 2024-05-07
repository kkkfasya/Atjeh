#ifndef ATJEH_MEMORY_H
#define ATJEH_MEMORY_H
#include "common.h"

#define GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2) // if capacity < 8, set it to 8, otherwise we double it


// uint8_t grow_capacity(uint8_t capacity);

/* 
  old_size: 	  new_size: 	            Operation:
  0 	          Non‑zero 	              Allocate new block.
  Non‑zero 	    0 	                    Free allocation.
  Non‑zero 	    Smaller than oldSize 	  Shrink existing allocation.
  Non‑zero 	    Larger than oldSize 	  Grow existing allocation.
*/ 
void *dynamic_realloc(void *ptr, size_t old_size, size_t new_size);


#endif // ATJEH_MEMORY_H
