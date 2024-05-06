#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "chunk.h"

int main(int argc, char **argv) {
    Chunk *chunk = (Chunk*) malloc(sizeof(Chunk));
    init_chunk(chunk);
    append_chunk(chunk, OP_RETURN);
    free_chunk(chunk);

    return 0;
}

