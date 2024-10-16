#ifndef AJTEH_TABLE_H
#define AJTEH_TABLE_H

#include "value.h"
#include "common.h"

// NOTE: find out about tag jumping
/*
 * {
 *    ObjString* "key": Value value
 * }
 */
typedef struct {
    ObjString *key; 
    Value value;
} Entry;

typedef struct {
    int used_count;
    int capacity;
    Entry *entries;
} Table;

void init_table(Table *table);
void free_table(Table *table);
bool table_set(Table *table, ObjString *key, Value value);
bool table_get(Table *table, ObjString *key, Value *value);
bool table_delete(Table* table, ObjString *key);
void table_data_move(Table *from, Table *to);
ObjString *table_find_string(Table *table, const char *str, int len, uint32_t hash);


#endif // AJTEH_TABLE_H
