#ifndef ATJEH_DEBUG_H
#define ATJEH_DEBUG_H
#include <stdio.h>
#include "chunk.h"

uint8_t disassemble_instruction(Chunk *chunk, uint8_t offset);
void disassemble_chunk(Chunk *chunk, const char *name);
uint8_t disassemble_constant_instruction(Chunk *chunk, uint8_t offset);


#endif // ATJEH_DEBUG_H
