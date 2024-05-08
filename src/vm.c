#include "chunk.h"
#include "common.h"
#include <stdbool.h>
#include <stdio.h>
#include "value.h"
#include "debug.h"
#include "vm.h"

VM vm; // i only need 1 VM so instead of passing pointer here and there i just make a global variable


void init_VM() {

}

void free_VM() {

}

static InterpretResult run() {
#ifdef DEBUG_TRACE_EXECUTION
    disassemble_instruction(vm.chunk,
                           (uint8_t) (vm.ip - vm.chunk->code));
#endif
#define READ_BYTE() (*vm.ip++) // ?
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])

    printf("%ud", READ_BYTE());
    while (1) {
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT:
                Value constant = READ_CONSTANT();
                printf("%g\n", constant);
                break;

            case OP_RETURN: 
                return INTERPRET_OK;

            default:
                return INTERPRET_RUNTIME_ERROR;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret_vm(Chunk *chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}
