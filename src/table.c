#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

#define TABLE_MAX_LOAD 0.75 // we grow the table when the load capacity is 75%

void init_table(Table *table) {
    table->capacity = 0;
    table->used_count = 0;
    table->entries = NULL;
}

void free_table(Table *table) {
    FREE_ARRAY(Entry, table->entries, table->capacity);
    init_table(table);
}

static Entry *find_entry(Entry *entries, int capacity, ObjString *key) {
    uint32_t index = key->hash % capacity; // mod by capacity so it stays in the table
    Entry *shit_value = NULL;

    for (;;) {
        Entry *e = &entries[index]; // get the "entry" ptr from "entries"
        if (e->key == NULL) {
            if (IS_NIL(e->value)) {
                return shit_value != NULL ? shit_value : e; // empty entry 
            } else if (shit_value == NULL){
                shit_value = e; // found the shit
            }
        } else if (e->key == key) {
            return e; // We found the key.
        }
        index = (index + 1) % capacity; // the bucket has an entry in it, but with a different key. 
                                        // This is a collision. In that case, we do "linear probing"
                                        // https://en.wikipedia.org/wiki/Linear_probing
    }
}

static void adjust_capacity(Table *table, int capacity) {
    /* So basically adjust_capacity just makes new shit then 
     * re-inserting old item to this new shit 
     * isnt it kind of slow? but it works for now */
    Entry *entries = ALLOCATE(Entry, capacity);
    for (int i = 0; i < capacity; i++) {
        entries[i].key = NULL;
        entries[i].value = NIL_VAL;
    }

    /* Re-inserting every entry into the new empty array of bucket 
     * We discard the shit_value when we adjust_capacity */
    table->used_count = 0;
    for (int i = 0; i < table->capacity; i++) {
        Entry *entry = &table->entries[i];
        if (entry->key == NULL) continue;

        Entry *bucket = find_entry(entries, capacity, entry->key);
        bucket->key = entry->key;
        bucket->value = entry->value;
        table->used_count++;
    }
    // #freethearray
    // #arraylivesmatter
    FREE_ARRAY(Entry, table->entries, table->capacity);
    table->entries = entries;
    table->capacity = capacity;
}


bool table_set(Table *table, ObjString *key, Value value) {
    bool is_new_key = false;

    if (table->used_count + 1 > table->capacity * TABLE_MAX_LOAD) {
        int capacity = GROW_CAPACITY(table->capacity);
        adjust_capacity(table, capacity);
    }

    // so we check do the key exist
    Entry *e = find_entry(table->entries, table->capacity, key);
    if (e->key == NULL) is_new_key = true;

    /* An infinite loop is a much worse problem than a few extra array slots 
     * for the sake of load factor, we consider tombstones to be full buckets */ 
    if (is_new_key && IS_NIL(e->value)) table->used_count++; 


    // if it's new, we add to the count, otherwise we overwrite the value in given key
    if (is_new_key) table->used_count++;
    e->key = key;
    e->value = value;
    return is_new_key;
}

// NOTE: table_add_all()
void table_move_data(Table *from, Table *to) { // not very c-like, usually it's dest, src not the other way around
    for (int i = 0; i < from->capacity; i++) {
        Entry *entry = &from->entries[i];
        if (entry->key != NULL) {
            table_set(to, entry->key, entry->value);
        }
    }
}

bool table_get(Table* table, ObjString *key, Value *value) {
    // so the value of the key that we request will be passed to *value
    if (table->used_count == 0) return false;

    Entry *entry = find_entry(table->entries, table->capacity, key);
    if (entry->key == NULL) return false;

    *value = entry->value;
    return true;
}

bool table_delete(Table* table, ObjString* key){
    if (table->used_count == 0) return false;

    // Find the entry.
    Entry *e = find_entry(table->entries, table->capacity, key);
    if (e->key == NULL) return false;

    /* We dont leave it empty, we add shit value so it has something because probing
     * even tho it's shit, atleast it's not empty right?
     * the official name is Tombstone */
    e->key = NULL;
    e->value = BOOL_VAL(true);
    return true;
}

ObjString *table_find_string(Table *table, const char *str, int len, uint32_t hash) {
    if (table->used_count == 0) return NULL;
    uint32_t index = hash % table->capacity;

    for (;;) {
        Entry *e = &table->entries[index];
        if (e->key == NULL && IS_NIL(e->value)) { // Stop if we find an empty non-tombstone entry
            return NULL;
        } else if (e->key->len == len 
                    && e->key->hash == hash 
                    && memcmp(e->key->str, str, len) == 0) {
            
            // We found the string
            return e->key;
        }

        index = (index + 1) % table->capacity;
    }

}
