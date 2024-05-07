#ifndef ATJEH_CHUNK_H
#define ATJEH_CHUNK_H
#include "common.h"
#include "value.h"

// One-byte operation code (OpCode)
typedef enum {
    OP_CONSTANT, // 1 byte operation, 1 more byte to load the constant (2-byte total)
    OP_RETURN,
} OpCode;

// basically dynamic array to hold array of bytes
typedef struct {
    uint8_t capacity; // number of memory that's allocated
    uint8_t used_count; // number of memory that's in use
    uint8_t *code; // byte code
    ValueArray constants;
} Chunk;

// TODO: maybe make init_chunk like curl_easy_init hmmm...
// set chunk to 0
void init_chunk(Chunk *chunk);

// Add byte to end of chunk
void append_chunk(Chunk *chunk, uint8_t byte);

// After chunk is being freed, it's being initialize again
void free_chunk(Chunk *chunk);

int add_constant(Chunk *chunk, Value value);


#endif // ATJEH_CHUNK_H
