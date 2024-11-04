#include <stdio.h>
#include "debug.h"
#include "chunk.h"
#include "value.h"


int disassemble_constant_instruction(Chunk *chunk, int offset) {
    uint8_t constant_value_offset = chunk->code[offset + 1];
    printf("\t%04d\t\t'", constant_value_offset);
    printf("[CONSTANT_VALUE]: %g", GET_NUMBER(chunk->constants.values[constant_value_offset])); // WARN: might result in stupid
    printf("'\n");
    return offset + 2; // 1 for the OP_CONSTANT and another 1 for the constant (so +2)
}

static int disassemble_byte_instruction(const char *name, Chunk *chunk,
        int offset) {
    uint8_t slot = chunk->code[offset + 1];
    printf("%-16s %4d\n", name, slot);
    return offset + 2; 
}

// HACK: what the fuck
static int disassemble_jump_instruction(const char *name, int sign, Chunk *chunk, int offset) {
    uint16_t jump = (uint16_t)(chunk->code[offset + 1] << 8);
    jump |= chunk->code[offset + 2];
    printf("%-16s %4d -> %d\n", name, offset,
            offset + 3 + sign * jump);
    return offset + 3;
}

int disassemble_instruction(Chunk *chunk, int offset) {
    printf("%04d\t", offset); // %04d e.g will format 7 to 0007, instruction offset
    int instruction = chunk->code[offset];

    // print line number
    if (offset > 0 && offset == offset - 1) { // if it's in the same line
        printf("|\t");
    } else {
        printf("%d\t", offset);
    }

    switch (instruction) {
        case OP_POP:
            printf("OP_POP\n");
            return offset + 1;

        case OP_CONSTANT:
            printf("%-16s", "OP_CONSTANT");
            return disassemble_constant_instruction(chunk, offset);

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

        case OP_NIL:
            printf("OP_NIL\n");
            return offset + 1;

        case OP_TRUE:
            printf("OP_TRUE\n");
            return offset + 1;

        case OP_FALSE:
            printf("OP_FALSE\n");
            return offset + 1;

        case OP_NOT:
            printf("OP_NOT\n");
            return offset + 1;

        case OP_EQUAL:
            printf("OP_EQUAL\n");
            return offset + 1;

        case OP_GREATER:
            printf("OP_GREATER\n");
            return offset + 1;

        case OP_LESS:
            printf("OP_LESS\n");
            return offset + 1;

        case OP_RETURN:
            printf("OP_RETURN\n");
            return offset + 1;

        case OP_PRINT:
            printf("OP_PRINT\n");
            return offset + 1;

    case OP_DEFINE_GLOBAL:
            printf("OP_DEFINE_GLOBAL\n");
            return disassemble_constant_instruction(chunk, offset);

    case OP_GET_GLOBAL:
            printf("OP_GET_GLOBAL\n");
            return disassemble_constant_instruction(chunk, offset);

    case OP_SET_GLOBAL:
            printf("OP_SET_GLOBAL\n");
            return disassemble_constant_instruction(chunk, offset);

    case OP_GET_LOCAL: 
            return disassemble_byte_instruction("OP_GET_LOCAL", chunk, offset);

    case OP_SET_LOCAL:
            return disassemble_byte_instruction("OP_SET_LOCAL", chunk, offset);

    case OP_JUMP:
            return disassemble_jump_instruction("OP_JUMP", 1, chunk, offset);

    case OP_JUMP_IF_FALSE:
            return disassemble_jump_instruction("OP_JUMP_IF_FALSE", 1, chunk, offset);

    case OP_LOOP:
            return disassemble_jump_instruction("OP_LOOP", -1, chunk, offset);

        default:
            fprintf(stderr, "[ERROR]: Unkown OP_CODE (Operation Code)\n");
            return offset + 1;

    }
}

void disassemble_chunk(Chunk *chunk, const char *name) {
    printf("\n%38s\n", name);
    printf("====================================================================================\n");
    printf("OFFSET\tLINE\tOP_CODE\t\tCONSTANT_OFFSET\t\tCONSTANT_VALUE\n");
    printf("====================================================================================\n");

    for (int offset = 0; offset < chunk->used_count;) {
        offset = disassemble_instruction(chunk, offset);
    }
}



