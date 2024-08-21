#include <stdlib.h>
#include "memory.h"
#include "object.h"
#include "vm.h"

void *dynamic_realloc(void *ptr, size_t old_size, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    /*
    else if (new_size < old_size) {
        // TODO: shrink chunk
    }
    else if (new_size > old_size) {
        // TODO: make bigger chunk
    }
    */


    void *alloc = realloc(ptr, new_size);
    if (alloc == NULL) exit(1);
    return alloc;
}

static void free_object(Obj *obj) {
    switch (obj->type) {
        case OBJ_STRING: {
                             ObjString* string = (ObjString*)obj;
                             FREE_ARRAY(char, string->str, string->len + 1);
                             FREE(ObjString, obj);
                             break;
                         }
    }
}

// walking a linked list and freeing its nodes
void free_objects() {
    Obj *obj = vm.objects;
    while (obj != NULL) {
        Obj *next = obj->next_obj;
        free_object(obj);
        obj = next;
    }
}




