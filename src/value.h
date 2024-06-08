#ifndef ATJEH_VALUE_H
#define ATJEH_VALUE_H
#include "common.h"

typedef double Value; // as in constant value e.g 1 + 2;

/* Constant pool is an array of values. looks up the constant value by index in that array. */
typedef struct {
    uint32_t capacity;
    uint32_t used_count;
    Value *values;
} ValueArray; // store constant, maybe i should change it to ConstantPool

void init_value_array(ValueArray *array);
void append_value_array(ValueArray *array, Value value);
void free_value_array(ValueArray *array);

#endif // ATJEH_VALUE_H
