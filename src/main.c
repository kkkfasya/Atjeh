#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"

int main(int argc, char **argv) {
    Chunk *chunk = (Chunk*) malloc(sizeof(Chunk));
    init_chunk(chunk);

    int constant = add_constant(chunk, 10.1);
    append_chunk(chunk, OP_RETURN);
    append_chunk(chunk, OP_CONSTANT); // operation
    append_chunk(chunk, constant); // constant value
    disassemble_instruction(chunk, "TEST CHUNK");

    free_chunk(chunk);

    return 0;
}

