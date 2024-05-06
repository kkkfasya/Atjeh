#include <stdio.h>
#include "debug.h"
#include "chunk.h"


void disassemble_instruction(Chunk *chunk, const char* name) {
    printf("=== %s ===\n", name);
    int offset = 0;

    while (offset < chunk->used_count) {
        printf("%04d\t", offset); // %04d e.g will format 7 to 0007
        uint8_t instruction = chunk->code[offset];
        switch (instruction) {
            case OP_RETURN:
                printf("OP_RETURN\n");
                offset++;
                break;

            default:
                fprintf(stderr, "[ERROR] Unkown OP_CODE (Operation Code)\n"); 
                offset++;
                break;
        }
        offset++;
    }
}

