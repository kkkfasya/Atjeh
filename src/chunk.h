#ifndef ATJEH_CHUNK_H
#define ATJEH_CHUNK_H
#include "common.h"
#include "value.h"

// TODO: Add OP_CONSTANT_LONG (4-byte || 32-bit) for big constant value
// One-byte operation code (OpCode)
typedef enum {
    OP_CONSTANT, // 1 byte operation, 1 more byte to load the constant (2-byte total)
    OP_NEGATE,
    OP_ADD,
    OP_SUBTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,

    OP_RETURN,
} OpCode;

// TODO: add something like run-length encoding to encode line number more efficient
// basically dynamic array to hold array of bytes
typedef struct {
    uint32_t capacity; // number of memory that's allocated
    uint32_t used_count; // number of memory that's in use
    uint32_t *code; // byte code
    uint32_t *lines; // Used to track the current line so when error occurs this line will be displayed
                    // When a runtime error occurs, we look up the line number at the same index as the current instruction’s offset in the code array.
    ValueArray constants;
} Chunk;

// TODO: maybe make init_chunk like curl_easy_init hmmm...
/* Empty chunk */
void init_chunk(Chunk *chunk);

/* Add byte to end of Chunk
 * @param uint32_t line used to track line number
 */
void append_chunk(Chunk *chunk, uint8_t byte, uint32_t line);

/* After chunk is being freed, it's being initialize again */
void free_chunk(Chunk *chunk);

uint32_t add_constant(Chunk *chunk, Value value);


#endif // ATJEH_CHUNK_H
