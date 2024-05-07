#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "value.h"


void init_value_array(ValueArray *array) {
    array->capacity = 0;
    array->used_count = 0;
    array->values = NULL;
}

void append_value_array(ValueArray *array, Value value) {
    if (array->capacity < array->used_count + 1) { // capacity not enough so we extend it
        int old_capacity = array->capacity;
        array->capacity = GROW_CAPACITY(old_capacity);
        array->values = (Value *) dynamic_realloc(array->values, old_capacity, array->capacity);
    }
    array->values[array->used_count] = value; // e.g. array->code[0] = byte
    array->used_count++;
};

void free_value_array(ValueArray *array) {
    free(array->values);
    array->values = NULL;
    init_value_array(array);
}


