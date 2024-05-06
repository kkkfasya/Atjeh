#include <stdlib.h>
#include "chunk.h"
#include "memory.h"

void init_chunk(Chunk *chunk) {
    chunk->capacity = 0;
    chunk->used_count = 0;
    chunk->code = NULL;
}

void append_chunk(Chunk *chunk, uint8_t byte) {
    if (chunk->capacity < chunk->used_count + 1) { // capacity not enough so we extend it
        int old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->code = (uint8_t*) grow_array(chunk->code, old_capacity, chunk->capacity);
    }
    chunk->code[chunk->used_count] = byte;
    chunk->used_count++;
}
;
