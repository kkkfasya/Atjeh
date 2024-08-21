#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, obj_type) \
    (type*) allocate_obj(sizeof(type), obj_type)

static Obj *allocate_obj(size_t size, ObjType type) {
    Obj *obj = (Obj*) dynamic_realloc(NULL, 0, size);
    obj->type = type;
    obj->next_obj = vm.objects;
    vm.objects = obj; // TODO: move this to the top, im sure the order doesn't matter

    return obj;
}

static ObjString *allocate_string(char *str, int len) {
    ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
    string->len = len;
    string->str = str;
    return string;
}

ObjString *copy_string(const char *str, int len) {
    char *heap_chars = ALLOCATE(char, len + 1);
    memcpy(heap_chars, str, len);
    heap_chars[len] = '\0'; // add the classic null terminator
    return allocate_string(heap_chars, len);
}

ObjString *take_string(char *str, int length) {
    return allocate_string(str, length);
}

void print_object(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", GET_CSTRING(value));
            break;
    }
}



