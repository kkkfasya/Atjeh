#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "value.h"


void init_value_array(ValueArray *array) {
    array->values = NULL;
    array->capacity = 0;
    array->used_count = 0;
}

void append_value_array(ValueArray *array, Value value) {
    if (array->capacity < array->used_count + 1) {
        int old_cap = array->capacity;
        array->capacity = GROW_CAPACITY(old_cap);
        array->values = GROW_ARRAY(Value, array->values, old_cap, array->capacity);
    }
    array->values[array->used_count] = value;
    array->used_count++;
};

void free_value_array(ValueArray *array) {
    FREE_ARRAY(Value, array->values, array->capacity);
    init_value_array(array);
}

void print_value(Value value) {
    printf("%g", value);
}


