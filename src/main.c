#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "vm.h"

int main(void) {
    Chunk *chunk = (Chunk *)  malloc(sizeof(Chunk));
    Value constant;
    init_chunk(chunk);
    init_VM();

    append_chunk(chunk, OP_CONSTANT, 123);
    constant = add_constant(chunk, 6.9);
    append_chunk(chunk, constant, 123);
    append_chunk(chunk, OP_NEGATE, 123);
    append_chunk(chunk, OP_RETURN, 123);

    disassemble_chunk(chunk, "TEST CHUNk");
    interpret_VM(chunk);


    return 0;
}

