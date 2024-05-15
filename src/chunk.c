#include <stdlib.h>
#include "chunk.h"
#include "memory.h"
#include "value.h"

void init_chunk(Chunk *chunk) {
    chunk->capacity = 0;
    chunk->used_count = 0;
    chunk->code = NULL;
    chunk->lines = NULL;
    init_value_array(&(chunk->constants));
}

void append_chunk(Chunk *chunk, uint8_t byte, uint8_t line) {
    if (chunk->capacity < chunk->used_count + 1) { // capacity not enough so we extend it, also +1 because we want to add a byte
        uint8_t old_capacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(old_capacity);
        chunk->code = (uint8_t *) dynamic_realloc(chunk->code, old_capacity, chunk->capacity);
        chunk->lines = (uint8_t *) dynamic_realloc(chunk->lines, old_capacity, chunk->capacity);
    }
    chunk->code[chunk->used_count] = byte; // e.g. chunk->code[0] = byte
    chunk->lines[chunk->used_count] = line;
    chunk->used_count++;
};

void free_chunk(Chunk *chunk) {
    free(chunk->code);
    free(chunk->lines);
    chunk->code = NULL;
    chunk->lines = NULL;
    free_value_array(&(chunk->constants));
    init_chunk(chunk);
}

uint8_t add_constant(Chunk *chunk, Value value) {
    append_value_array(&(chunk->constants), value);
    return  chunk->constants.used_count - 1; // After adding the constant, we return the index where the constant was appended so that we can locate that same constant later.
}

