#include <stdio.h>
#include "debug.h"
#include "chunk.h"


void disassemble_instruction(Chunk *chunk, const char* name) {
    printf("================= %s =================\n", name);
    printf("-------------------------------------------------------------\n");
    printf("OFFSET\tOP_CODE\t\tCONSTANT OFFSET\t\tCONSTANT VALUE\n");
    printf("-------------------------------------------------------------\n");
    int offset = 0;

    while (offset < chunk->used_count) {
        printf("%04d\t", offset); // %04d e.g will format 7 to 0007
        uint8_t instruction = chunk->code[offset];
        switch (instruction) {
            case OP_RETURN:
                printf("OP_RETURN\n");
                offset++;
                break;
            case OP_CONSTANT:
                offset = disassemble_constant_instruction("OP_CONSTANT", chunk, offset);
                break;
            default:
                fprintf(stderr, "[ERROR] Unkown OP_CODE (Operation Code)\n");
                offset++;
                break;
        }
    }
}


int disassemble_constant_instruction(const char *name, Chunk* chunk, int offset) {
    uint8_t constant = chunk->code[offset + 1]; // +1 for accessing next value and that value is constant (cuz current value is OP_CODE)
    printf("%s\t%04d", name, constant);
    printf("\t\t\t%g\n", chunk->constants.values[constant]);
    return offset + 2; // 1 for the OP_CONSTANT and another 1 for the constant (so +2)
}
