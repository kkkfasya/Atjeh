#include <stdio.h>
#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "common.h"
#include "value.h"
#include "compiler.h"


// TODO: dynamic VM, maybe...
VM vm; // i only need 1 VM so instead of passing pointer here and there, just make a global variable

static void reset_stack() {
    vm.stack_top = vm.stack;
}

void init_VM() {
    reset_stack();
}

void free_VM() {

}

void push(Value value) {
    *vm.stack_top = value;
    vm.stack_top++; // increment the pointer to point to the next unused slot in the array now that the previous slot is occupied.
}

Value pop() {
    vm.stack_top--;
    return *vm.stack_top;
}


static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) \
    { \
        Value a = pop(); \
        Value b = pop(); \
        push(a op b); \
    }

    while (1) {
#ifdef DEBUG_TRACE_EXECUTION
    Value stack_debug[1024] = {0};
    int i = 0;
    printf("\nDEBUG_TRACE_EXECUTION\n");

    for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
        stack_debug[i] = *slot;
        i++;
    }

    for (i = i - 1; i >= 0; i--) {
        printf("[ %g ]\n", stack_debug[i]);
    }


    disassemble_instruction(vm.chunk, (int) (vm.ip - vm.chunk->code));
#endif

        uint8_t instruction;
        switch(instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                printf("[VM] Constant: ");
                print_value(constant);
                printf("\n");
                break;
             }

            case OP_ADD:
                  BINARY_OP(+);
                  break;
            
            case OP_SUBTRACT:
                  BINARY_OP(-);
                  break;
            
            case OP_MULTIPLY:
                  BINARY_OP(*);
                  break;
            
            case OP_DIVIDE:
                  BINARY_OP(/);
                  break;
            
            case OP_NEGATE:
                push(-pop());
                break;

            case OP_RETURN: {
                printf("[VM] Return Value: ");
                print_value(pop());
                printf("\n");
                return INTERPRET_OK;
             }

        }

    }

#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}


InterpretResult interpret_VM(const char *src) {
    /* We create a new empty chunk and pass it over to the compiler.
     * The compiler will take the user’s program and fill up the chunk with bytecode. 
     */
    Chunk chunk;
    init_chunk(&chunk);

    if (!compile(src, &chunk)) {
        free_chunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    free_chunk(&chunk);
    return result;
}



