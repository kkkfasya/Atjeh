#include <stdio.h>
#include "debug.h"
#include "chunk.h"

uint8_t disassemble_instruction(Chunk *chunk, uint8_t offset) {
    printf("%04d\t", offset); // %04d e.g will format 7 to 0007

    if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) { // if it's in the same line
        printf(" |\t");
    } else {
        printf("%4d\t", chunk->lines[offset]);
    }

    uint8_t instruction = chunk->code[offset];
    switch (instruction) {
        case OP_RETURN:
            printf("OP_RETURN\n");
            return offset + 1;
            break;

        case OP_CONSTANT:
            offset = disassemble_constant_instruction("OP_CONSTANT", chunk, offset);
            return offset;

        default:
            fprintf(stderr, "[ERROR] Unkown OP_CODE (Operation Code)\n");
            return offset + 1;
    }
}

void disassemble_chunk(Chunk *chunk, const char* name) {
    printf("================= %s =================\n", name);
    printf("-------------------------------------------------------------\n");
    printf("OFFSET\tLINE\tOP_CODE\t\tCONSTANT OFFSET\t\tCONSTANT VALUE\n");
    printf("-------------------------------------------------------------\n");
    int offset = 0;

    while (offset < chunk->used_count) {
        printf("%04d\t", offset); // %04d e.g will format 7 to 0007, instruction offset
        offset = disassemble_instruction(chunk, offset);

        if (offset > 0 && chunk->lines[offset] == chunk->lines[offset - 1]) { // if it's in the same line
            printf(" |\t");
        } else {
            printf("%4d\t", chunk->lines[offset]);
        }
    } 
}


int disassemble_constant_instruction(const char *name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1]; // +1 for accessing next value and that value is constant (cuz current value is OP_CODE)
    printf("%s\t%04d", name, constant);
    printf("\t\t\t%g\n", chunk->constants.values[constant]);
    return offset + 2; // 1 for the OP_CONSTANT and another 1 for the constant (so +2)
}
