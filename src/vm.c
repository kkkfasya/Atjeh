#include <stdio.h>
#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "common.h"
#include "value.h"


VM vm; // i only need 1 VM so instead of passing pointer here and there i just make a global variable

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


InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
    while (1) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("\nDEBUG_TRACE_EXECUTION\n");
    for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
        printf("[ %g ]\n", *slot);
    }
    disassemble_instruction(vm.chunk, (uint8_t) (vm.ip - vm.chunk->code));
#endif
        uint8_t instruction;
        switch(instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                printf("[VM] Constant: %g\n", constant);
                break;
             }

            case OP_NEGATE:
                push(-pop());
                break;

            case OP_RETURN: {
                printf("[VM] RETURN VALUE: %g\n", pop());
                return INTERPRET_OK;
             }
        }

    }


#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret_VM(Chunk *chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;

    return run();
}
