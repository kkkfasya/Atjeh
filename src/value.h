#ifndef ATJEH_VALUE_H
#define ATJEH_VALUE_H
#include "common.h"

typedef enum {
    VALUE_NIL,
    VALUE_BOOL,
    VALUE_NUMBER,
    VALUE_OBJ // NOTE: VALUE_OBJ refer to value that's stored in the heap, e.g string
} ValueType;

typedef struct Obj Obj;
typedef struct ObjString ObjString;

typedef struct {
    ValueType type;
    union { // Tagged union
        bool boolean;
        double number;
        Obj *obj;
    } as; // NOTE: as? i think 'value' fits better
} Value;

/* Constant pool is an array of values. looks up the constant value by index in that array. */
typedef struct {
    int capacity;
    int used_count;
    Value *values;
} ValueArray; // store constant, maybe i should change it to ConstantPool

#define BOOL_VAL(value)   ((Value) {VALUE_BOOL, {.boolean = value}} )
#define NUMBER_VAL(value) ((Value) {VALUE_NUMBER, {.number = value}} )
#define NIL_VAL           ((Value) {VALUE_NIL, {.number = 0}} )
#define OBJ_VAL(object)   ((Value) {VALUE_BOOL, {.obj = (Obj*) object}})
// nil is 0, end of debate

// NOTE: AS_<value_type>
#define GET_BOOL(value)    ((value).as.boolean)
#define GET_NUMBER(value)  ((value).as.number)
#define GET_OBJ(value)     ((value).as.obj)

#define IS_BOOL(value)    ((value).type == VALUE_BOOL)
#define IS_NIL(value)     ((value).type == VALUE_NIL)
#define IS_NUMBER(value)  ((value).type == VALUE_NUMBER)
#define IS_OBJ(value)     ((value).type == VALUE_OBJ)

void init_value_array(ValueArray *array);
void append_value_array(ValueArray *array, Value value);
void free_value_array(ValueArray *array);
void print_value(Value value);
bool is_values_equal(Value a, Value b);

#endif // ATJEH_VALUE_H
