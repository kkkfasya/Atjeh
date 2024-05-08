#ifndef ATJEH_VM_H
#define ATJEH_VM_H

#include "chunk.h"

typedef struct {
    Chunk *chunk;
    uint8_t *ip; // instruction pointer, this is to keep track the location of the instruction currently executed
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void init_VM();
void free_VM();
InterpretResult interpret_vm(Chunk *chunk);


#endif // ATJEH_VM_H
