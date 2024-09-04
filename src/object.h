#ifndef ATJEH_OBJECT_H
#define ATJEH_OBJECT_H

#include "common.h"
#include "value.h"


#define OBJ_TYPE(obj) (GET_OBJ(value)->type)
#define IS_STRING(value) is_obj_type(value, OBJ_STRING)

// NOTE: AS_<shit>
#define GET_STRING_PTR(value)       ( (ObjString*) GET_OBJ(value))
#define GET_CSTRING(value)      ( ((ObjString*) GET_OBJ(value))->str)

typedef enum {
    OBJ_STRING,
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

#endif // ATJEH_OBJECT_H
