#ifndef ATJEH_VM_H
#define ATJEH_VM_H
#include "chunk.h"
#include "value.h"

#define STACK_MAX_SIZE 1024

typedef struct {
    Chunk *chunk;
    uint32_t *ip; // instruction pointer, this is to keep track the location of the instruction currently executed
    Value stack[STACK_MAX_SIZE];
    Value *stack_top; //  stack_top points just past the last item e.g {0, 1, -> NULL}
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

void init_VM();
void free_VM();
InterpretResult interpret_VM(const char *src);
void push(Value value);
Value pop();


#endif // ATJEH_VM_H
