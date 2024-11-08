#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "vm.h"
#include "chunk.h"
#include "debug.h"
#include "common.h"
#include "value.h"
#include "compiler.h"
#include "object.h"
#include "memory.h"
#include "table.h"


static void reset_stack();
static Value peek(int distance);
static void runtime_error(const char* format, ...); 
static bool is_falsey(Value value); 
static void print_stack_value(Value value); 
static void concat_str(); 
static InterpretResult run(); 

// Turns out, making this dynamic serve zero benefits (AFAIK)
VM vm; // i only need 1 VM so instead of passing pointer here and there, just make a global variable

static void reset_stack() {
    vm.stack_top = vm.stack;
}

void init_VM() {
    reset_stack();
    vm.objects = NULL;
    init_table(&vm.strings);
    init_table(&vm.globals);
}

void free_VM() {
    free_table(&vm.strings);
    free_table(&vm.globals);
    free_objects();
}

void push(Value value) {
    *vm.stack_top = value;
    vm.stack_top++; // increment the pointer to point to the next unused slot in the array now that the previous slot is occupied.
}

Value pop() {
    vm.stack_top--;
    return *vm.stack_top;
}

static Value peek(int distance) {
    return vm.stack_top[-1 - distance];
}


static void runtime_error(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    /* We look into the chunk’s debug line array using the current bytecode instruction index minus one. 
NOTE: That’s because the interpreter advances past each instruction before executing it. */
    size_t instruction = vm.ip - vm.chunk->code - 1;
    int line = vm.chunk->lines[instruction];
    fprintf(stderr, "[line %d] in script\n", line);
    reset_stack();
}

// nil and false, is falsey, other value is true
static bool is_falsey(Value value) {
    return IS_NIL(value) || (IS_BOOL(value) &&  !GET_BOOL(value));
}


// Only for debug purpose
static void print_stack_value(Value value) {
    switch (value.type) {
        case VALUE_BOOL:
            printf(GET_BOOL(value) ? "[ true ]\n" : "[ false ]\n");
            break;

        case VALUE_NUMBER:
            printf("[ %g ]\n", GET_NUMBER(value));
            break;

        case VALUE_NIL:
            printf("nil"); // TODO: change to my syntax later, "hana"
            break;

        default:
            fprintf(stderr, "[ERROR]: Unknown value");
    }
}


static void concat_str() {
    ObjString *a = GET_STRING_PTR(pop());
    ObjString *b = GET_STRING_PTR(pop());

    int len = a->len + b->len;
    char *str = ALLOCATE(char, len + 1); // once again, +1, sweet null terminator

    // This is somewhat a visual explanation for the code below

    /* 
     * 0x0 == [] <= a->str
     * 0x0 == [a->str]
     * 0x5 == [a->str + a->len]
     * 0x5 == [a->str] <= b->str
     * 0x5 == [a->str+b->str] 
     */

    memcpy(str, a->str, a->len);
    memcpy(str + a->len, b->str, b->len);

    // 0x11 == ['\0']
    str[len] = '\0';

    ObjString *result = take_string(str, len);
    push(OBJ_VAL(result));
}


static InterpretResult run() {
#define READ_BYTE() (*vm.ip++) // point to next opcode/instruction, then get the opcode
#define READ_SHORT() \
    (vm.ip += 2, (uint16_t) ((vm.ip[-2] << 8) | vm.ip[-1])) // HACK: what the fuck?
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_STRING() GET_STRING_PTR(READ_CONSTANT())
#define BINARY_OP(value_type, op) \
    { \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) { \
            runtime_error("[ERROR]: Operands must be numbers."); \
            return INTERPRET_RUNTIME_ERROR; \
        } \
        double b = GET_NUMBER(pop()); \
        double a = GET_NUMBER(pop()); \
        push(value_type(a op b)); \
    } \

#ifdef DEBUG_TRACE_EXECUTION
    Value stack_debug[1024] = {0};
    int i = 0;
    printf("\nDEBUG_TRACE_EXECUTION\n");

    // BUG: the bug is actually here, it doesnt execute
    // for (Value *slot = vm.stack; slot < vm.stack_top; slot++) {
    //     stack_debug[i] = *slot;
    //     i++;
    // }
    //
    // for (i = i - 1; i >= 0; i--) {
    //     Value v = stack_debug[i];
    //     print_stack_value(v);
    // }
    //

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

        case OP_POP: pop(); break;

        case OP_NIL:
                     push(NIL_VAL); 
                     break;

        case OP_TRUE:
                     push(BOOL_VAL(true)); 
                     break;

        case OP_FALSE:
                     push(BOOL_VAL(false));
                     break;

        case OP_ADD: {
                         if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
                             concat_str();
                         } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
                             double b = GET_NUMBER(pop());
                             double a = GET_NUMBER(pop());
                             push(NUMBER_VAL(a + b));
                         } else {
                             runtime_error(
                                     "Operands must be two numbers or two strings.");
                             return INTERPRET_RUNTIME_ERROR;
                         }
                         break;
                     }


        case OP_SUBTRACT:
                     BINARY_OP(NUMBER_VAL, -);
                     break;

        case OP_MULTIPLY:
                     BINARY_OP(NUMBER_VAL, *);
                     break;

        case OP_DIVIDE:
                     BINARY_OP(NUMBER_VAL, /);
                     break;

        case OP_NEGATE: {
                            if (!IS_NUMBER(peek(0))) {
                                runtime_error("[ERROR]: Operand must be number.");
                                return INTERPRET_RUNTIME_ERROR;
                            }
                            push(NUMBER_VAL(-GET_NUMBER(pop())));
                            break;
                        }
        case OP_NOT:
                        push(BOOL_VAL(is_falsey(pop())));
                        break;

        case OP_EQUAL: {
                           Value b = pop();
                           Value a = pop();
                           push(BOOL_VAL(is_values_equal(a, b)));
                           break;
                       }

        case OP_GREATER:  BINARY_OP(BOOL_VAL, >); break;

        case OP_LESS:     BINARY_OP(BOOL_VAL, <); break;
        case OP_PRINT:
                          print_value(pop());
                          printf("\n");
                          break;
        case OP_DEFINE_GLOBAL:
                          /* Get the name of the variable from the constant table. Then take the value from the top of the stack and store it in a hash table with that name as the key. */

                          // honestly i dont know where most of the data goes its quite confusing right now
                          // by the time i finish this i'll probably will only understand 50% of it
                          // TODO: REFACTOR, too many macros and helper function it is unhelpful anymore
                          ObjString *var_name = READ_STRING();
                          table_set(&vm.globals, var_name, peek(0));
                          pop();
                          break;

        case OP_GET_GLOBAL: {
                                ObjString *name = READ_STRING();
                                Value value;
                                if (!table_get(&vm.globals, name, &value)) {
                                    runtime_error("Undefined variable '%s'.", name->str);
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                                push(value);
                                break;
                            }

        case OP_SET_GLOBAL: {
                                // If the variable hasn’t been defined yet, 
                                // it’s a runtime error to try to assign to it
                                // no fucking implicit declaration, i like it explicit
                                ObjString *name = READ_STRING();
                                if (table_set(&vm.globals, name, peek(0))) {
                                    table_delete(&vm.globals, name);
                                    runtime_error("Undefined variable '%s'.", name->str);
                                    return INTERPRET_RUNTIME_ERROR;
                                }
                                break;
                            }

        case OP_GET_LOCAL: {
                               uint8_t slot = READ_BYTE();
                               push(vm.stack[slot]); 
                               break;
                           }

        case OP_SET_LOCAL: {
                               uint8_t slot = READ_BYTE();
                               vm.stack[slot] = peek(0); // add current local var to onto stack
                               break;
                           }
        case OP_JUMP_IF_FALSE: {
                                   uint16_t offset = READ_SHORT();
                                   if (is_falsey(peek(0))) vm.ip += offset;
                                   break;
                               }
        case OP_JUMP: {
                          uint16_t offset = READ_SHORT();
                          vm.ip += offset;
                          break;
                      }

        case OP_LOOP: {

                          uint16_t offset = READ_SHORT();
                          vm.ip -= offset;
                          break;
                      }

        case OP_RETURN: 
                      return INTERPRET_OK;
    }

    return INTERPRET_OK;
#undef READ_BYTE
#undef READ_STRING
#undef READ_CONSTANT
#undef BINARY_OP
#undef READ_SHORT
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



