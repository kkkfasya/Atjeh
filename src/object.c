#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "value.h"
#include "vm.h"

#define ALLOCATE_OBJ(type, obj_type) \
    (type*) allocate_obj(sizeof(type), obj_type)

static Obj *allocate_obj(size_t size, ObjType type);
static ObjString *allocate_string(char *str, int len, uint32_t hash);
static uint32_t hash_string(const char* key, int length);

static Obj *allocate_obj(size_t size, ObjType type) {
    Obj *obj = (Obj*) dynamic_realloc(NULL, 0, size);
    obj->type = type;
    obj->next_obj = vm.objects;
    vm.objects = obj; // TODO: move this to the top, im sure the order doesn't matter (edit: i think it does now)

    return obj;
}

static ObjString *allocate_string(char *str, int len, uint32_t hash) {
    // ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING); // NOTE: i think this macro is unnecessary
    ObjString *string = (ObjString *) allocate_obj(sizeof(ObjString), OBJ_STRING);
    string->len = len;
    string->str = str;
    string->hash = hash;
    table_set(&vm.strings, string, NIL_VAL);
    return string;
}

static uint32_t hash_string(const char* key, int length) {
    // NOTE: HASH ALGORITHM: FNV-1a
    // initial hash value
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        // scrambling with be bits and shits
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}


ObjString *copy_string(const char *str, int len) {
    char *heap_chars = ALLOCATE(char, len + 1);
    uint32_t hash = hash_string(str, len);
    ObjString* interned = table_find_string(&vm.strings, str, len, hash);
    if (interned != NULL) return interned;
    // char *heap_chars = (char *) dynamic_realloc(NULL, 0, sizeof(char) + (len + 1)); // hmmm
    memcpy(heap_chars, str, len);
    heap_chars[len] = '\0'; // add the classic null terminator
    return allocate_string(heap_chars, len, hash);
}

ObjString *take_string(char *str, int len) {
    uint32_t hash = hash_string(str, len);
    return allocate_string(str, len, hash);
}

void print_object(Value value) {
    switch (OBJ_TYPE(value)) {
        case OBJ_STRING:
            printf("%s", GET_CSTRING(value));
            break;
    }
}



