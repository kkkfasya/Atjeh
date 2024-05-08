#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, char **argv) {
    Chunk *chunk = (Chunk*) malloc(sizeof(Chunk));
    init_chunk(chunk);
    init_VM();

    int constant = add_constant(chunk, 6.9);
    append_chunk(chunk, OP_CONSTANT, 123); // operation
    append_chunk(chunk, constant, 123); // constant value
    append_chunk(chunk, OP_RETURN, 123);
    disassemble_chunk(chunk, "TEST CHUNK");
    interpret_vm(chunk);

    free_VM();
    free_chunk(chunk);

    return 0;
}

