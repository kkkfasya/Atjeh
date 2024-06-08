#include <stdio.h>
#include "debug.h"
#include "chunk.h"


uint32_t disassemble_constant_instruction(Chunk *chunk, uint32_t offset) {
    uint32_t constant_value_offset = chunk->code[offset + 1];
    printf("\t%04d\t\t'", constant_value_offset);
    printf("%g", chunk->constants.values[constant_value_offset]);
    printf("'\n");
    return offset + 2; // 1 for the OP_CONSTANT and another 1 for the constant (so +2)
}

uint32_t disassemble_instruction(Chunk *chunk, uint32_t offset) {
    printf("%04d\t", offset); // %04d e.g will format 7 to 0007, instruction offset
    uint32_t instruction = chunk->code[offset];
    uint32_t line = chunk->lines[offset];

    // print line number
    if (offset > 0 && line == line - 1) { // if it's in the same line
        printf("|\t");
    } else {
        printf("%d\t", line);
    }

    switch (instruction) {
        case OP_CONSTANT:
            printf("%-16s", "OP_CONSTANT");
            uint32_t f = disassemble_constant_instruction(chunk, offset);
            return f;

        case OP_NEGATE:
            printf("OP_NEGATE\n");
            return offset + 1;

        case OP_ADD:
            printf("OP_ADD\n");
            return offset + 1;

        case OP_SUBTRACT:
            printf("OP_SUBTRACT\n");
            return offset + 1;

        case OP_MULTIPLY:
            printf("OP_MULTIPLY\n");
            return offset + 1;

        case OP_DIVIDE:
            printf("OP_DIVIDE\n");
            return offset + 1;

        case OP_RETURN:
            printf("OP_RETURN\n");
            return offset + 1;

        default:
#ifdef STDERR_OUT
            fprintf(stderr, "[LINE: %d] [ERROR] Unkown OP_CODE (Operation Code)\n", line);
#endif
            fprintf(stdout, "[ERROR] Unkown OP_CODE (Operation Code)\n");
            return offset + 1;

    }
}

void disassemble_chunk(Chunk *chunk, const char *name) {
    printf("\n%38s\n", name);
    printf("====================================================================================\n");
    printf("OFFSET\tLINE\tOP_CODE\t\tCONSTANT_OFFSET\t\tCONSTANT_VALUE\n");
    printf("====================================================================================\n");

    for (uint32_t offset = 0; offset < chunk->used_count;) {
        offset = disassemble_instruction(chunk, offset);
    }
}



