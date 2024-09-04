#ifndef ATJEH_VM_H
#define ATJEH_VM_H
#include "chunk.h"
#include "value.h"
#include "table.h"

#define STACK_MAX_SIZE 1024

typedef struct {
    Chunk *chunk;
    uint8_t *ip; // instruction pointer, this is to keep track the location of the instruction currently executed
    Value stack[STACK_MAX_SIZE];
    Value *stack_top; //  stack_top points just past the last item e.g {0, 1, -> NULL}
    Table strings;
    Obj *objects; 
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR,
} InterpretResult;

extern VM vm; // vm can be used across the file, the variable and its state

void init_VM();
void free_VM();
InterpretResult interpret_VM(const char *src);
void push(Value value);
Value pop();
bool is_values_equal(Value a, Value b);


#endif // ATJEH_VM_H
