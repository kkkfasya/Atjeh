#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "memory.h"
#include "object.h"
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

bool is_values_equal(Value a, Value b) {
    if (a.type != b.type) return false;
    switch (a.type) {
        case VALUE_BOOL:   return GET_BOOL(a) == GET_BOOL(b);
        case VALUE_NIL:    return true;
        case VALUE_NUMBER: return GET_NUMBER(a) == GET_NUMBER(b);
        case VALUE_OBJ: {
                            ObjString *str_a = GET_STRING_PTR(a);
                            ObjString *str_b = GET_STRING_PTR(b);
                            // TODO: will it have an effect if i wrap this in parentheses?
                            return str_a->len == str_b->len && 
                                memcmp(str_a->str, str_b->str, str_a->len) == 0;
                        }
        default:         return false;
    }
}

void print_value(Value value) {
    switch (value.type) {
        case VALUE_BOOL:
            printf(GET_BOOL(value) ? "true" : "false");
            break;

        case VALUE_NUMBER:
            printf("%g", GET_NUMBER(value));
            break;

        case VALUE_NIL:
            printf("nil"); // TODO: change to my syntax later, "hana"
            break;

        case VALUE_OBJ:
            print_object(value);
            break;

        default:
            fprintf(stderr, "[ERROR]: Unknown value");
    }
}






