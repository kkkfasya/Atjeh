#ifndef ATJEH_CHUNK_H
#define ATJEH_CHUNK_H
#include "common.h"
#include "value.h"

//  TODO: Add OP_CONSTANT_LONG (4-byte || 32-bit) for big constant value
typedef enum {
    OP_CONSTANT,
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,

    OP_NOT,
    OP_EQUAL,
    OP_GREATER,
    OP_LESS,

    OP_TRUE,
    OP_FALSE,
    OP_NIL,

    OP_RETURN,
} OP_CODE;

typedef struct {
    int used_count;
    int capacity;
    uint8_t *code;
    int *lines; // Used to track the current line so when error occurs this line will be displayed
    ValueArray constants;
} Chunk;

void init_chunk(Chunk *chunk);
void append_chunk(Chunk *chunk, uint8_t byte, int line);
void free_chunk(Chunk *chunk);
int add_constant(Chunk *chunk, Value value);

#endif // ATJEH_CHUNK_H
