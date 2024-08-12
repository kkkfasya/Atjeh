#ifndef ATJEH_DEBUG_H
#define ATJEH_DEBUG_H
#include <stdio.h>
#include "chunk.h"

int disassemble_instruction(Chunk *chunk, int offset);
void disassemble_chunk(Chunk *chunk, const char *name);
int disassemble_constant_instruction(Chunk *chunk, int offset);


#endif // ATJEH_DEBUG_H
