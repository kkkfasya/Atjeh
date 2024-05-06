#ifndef atjeh_chunk_h
#define atjeh_chunk_h
#include "common.h"

// One-byte operation code (OpCode)
typedef enum {
    OP_RETURN,
} OpCode;

// basically dynamic array to hold array of bytes
typedef struct {
    uint8_t capacity; // number of memory that's allocated
    uint8_t used_count; // number of memory that's in use
    uint8_t *code;
} Chunk;

void init_chunk(Chunk *chunk);
void append_chunk(Chunk *chunk, uint8_t byte);

#endif // atjeh_chunk_h
