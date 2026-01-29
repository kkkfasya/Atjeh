#ifndef ATJEH_OBJECT_H
#define ATJEH_OBJECT_H

#include "chunk.h"
#include "common.h"
#include "value.h"

#define OBJ_TYPE(obj)       (GET_OBJ(obj)->type)
#define IS_STRING(value)    is_obj_type(value, OBJ_STRING)
#define IS_FUNCTION(value)  is_obj_type(value, OBJ_FUNCTION)
#define IS_NATIVE(value)    is_obj_type(value, OBJ_NATIVE)

// NOTE: AS_<shit>
// NOTE: GET_AS is a better name perhaps change to that later
#define GET_STRING_PTR(value)       ( (ObjString*) GET_OBJ(value)) // get ptr to "this" NOTE: AS_STRING, delete later
#define GET_CSTRING(value)      ( ((ObjString*) GET_OBJ(value))->str) // get "this"
#define GET_FUNCTION(value) ((ObjFunction*)GET_OBJ(value))
#define GET_NATIVE(value) \
    ( ((ObjNative*) GET_OBJ(value))->function )


typedef enum {
    OBJ_FUNCTION,
    OBJ_NATIVE, // this is for native function
    OBJ_STRING
} ObjType;


/*
 * a linked list that stores every Obj. 
 * The VM can traverse that list to find every single object that has been allocated on the heap, 
 * whether or not the user’s program or the VM’s stack still has a reference to it. */

/* We’ll use an intrusive list—the Obj struct itself will be the linked list node. Each Obj gets a pointer to the next Obj in the chain.
*/

struct Obj {
    ObjType type;
    struct Obj *next_obj;
};

/* Because ObjString is an Obj, it also needs the state all Objs share. 
 * we do that by having its first field be an Obj. C specifies that 
 * struct fields are arranged in memory in the order that they are declared 
 * We doing it this way because we wanna do 'type-punning' */
struct ObjString {
    Obj obj;
    int len;
    char *str;
    uint32_t hash;
};

typedef struct {
    Obj obj;
    int arity; // number of param of a function
    Chunk chunk;
    ObjString *name; // function name
} ObjFunction;

// basically NativeFn is a pointer to a function which takes 2 args
// NOTE: i think changing this to a function instead of function pointer makes it cleaner
typedef Value (*NativeFn)(int argCount, Value* args);

typedef struct {
    Obj obj;
    NativeFn function;
} ObjNative;

/* If this is a macro, it would cause stupid shit
 * lets say we do is_obj_type(pop()), then it will pop() twice*/
static inline bool is_obj_type(Value value, ObjType type) {
    return IS_OBJ(value) && GET_OBJ(value)->type == type;
}
// damn bruh we really reimplementing string here i love c
ObjString *copy_string(const char *str, int len);
// Take ownership of the string
ObjString *take_string(char *str, int len);
void print_object(Value value);

/* Create function */
ObjFunction *new_function();

ObjNative *new_native_function(NativeFn function);

#endif // ATJEH_OBJECT_H
