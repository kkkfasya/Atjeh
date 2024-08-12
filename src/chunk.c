#include <stdlib.h>
#include "chunk.h"
#include "memory.h"
#include "value.h"

void init_chunk(Chunk *chunk) {
    chunk->capacity = 0;
    chunk->used_count = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    init_value_array(&chunk->constants);
}

void append_chunk(Chunk *chunk, uint8_t byte, int line) {
    // capacity not enough so we extend it, also +1 because we want to add a byte
  if (chunk->capacity < chunk->used_count + 1) {
    int old_cap = chunk->capacity;
    chunk->capacity = GROW_CAPACITY(old_cap);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code,
        old_cap, chunk->capacity);
    chunk->lines = GROW_ARRAY(int, chunk->lines,
        old_cap, chunk->capacity);
  }

  chunk->code[chunk->used_count] = byte;
  chunk->lines[chunk->used_count] = line;
  chunk->used_count++;
}

void free_chunk(Chunk *chunk) {
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    FREE_ARRAY(int, chunk->lines, chunk->capacity);
    free_value_array(&chunk->constants);
    init_chunk(chunk);
}

int add_constant(Chunk *chunk, Value value) {
    /* After adding the constant, we return the index where the constant was appended 
     * so that we can locate that same constant later. */
  append_value_array(&chunk->constants, value);
  return chunk->constants.used_count - 1;
}

