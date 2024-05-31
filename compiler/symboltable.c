#include "symboltable.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    const char* key;
    const char* value;
    int line;
    ASTnode* node;
    //id_type type;
} ht_entry;

struct ht {
    ht_entry* entries;
    size_t capacity;
    size_t length;
    ASTnode* node;
};

#define INITIAL_CAPACITY    64


ht* ht_create(void)
{
    ht* table = malloc(sizeof(ht));

    if(table == NULL) return NULL;

    table->length = 0;
    table->capacity = INITIAL_CAPACITY;

    // Create entries with all the keys NULL to begin with, meaning all entries are empty
    // For this, calloc is used
    // Calloc - allocates memory for an array of (table->capacity) objects of size (ht_entry)
    table->entries = calloc(table->capacity, sizeof(ht_entry));

    if(table->entries == NULL)
    {
        free(table);
        return NULL;
    }

    return table;
}


void ht_destroy(ht* table)
{
    for(size_t i = 0; i < table->capacity; i++)
    {
        free((void*)table->entries[i].key);
    }
    free(table->entries);
    free(table);
}


// Hash function implementation
// To implement hash_key, a 64-bit FNV-1a algorithm is used
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

// Return 64-bit FNV-1a hash for key 
static uint64_t hash_key(const char* key) 
{
    uint64_t hash = FNV_OFFSET;
    for (const char* p = key; *p; p++) {
        hash ^= (uint64_t)(unsigned char)(*p);
        hash *= FNV_PRIME;
    }
    return hash;
}


// Internal function to set an entry (without expanding table).
static const char* ht_set_entry(ht_entry* entries, size_t capacity,
        const char* key, const char* value, size_t* plength, int line, ASTnode* node) {
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            // Found key (it already exists), update value.
            entries[index].value = value;
            entries[index].line = line;
            entries[index].node = node;
            return entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    // Didn't find key, allocate+copy if needed, then insert it.
    if (plength != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*plength)++;
    }
    entries[index].key = (char*)key;
    entries[index].value = value;
    entries[index].line = line;
    entries[index].node = node;
    return key;
}


const char* ht_get_key(ht* table, const char* key) 
{
    // AND hash with capacity-1 to ensure it's within entries array
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {
            // Found key, return value
            return table->entries[index].key;
        }
        // Key wasn't in this slot, move to next (linear probing)
        index++;
        if (index >= table->capacity) {
            // At end of entries array, wrap around
            index = 0;
        }
    }
    return NULL;
}


int ht_get_line(ht* table, const char* key) 
{
    // AND hash with capacity-1 to ensure it's within entries array
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {
            // Found key, return value
            return table->entries[index].line;
        }
        // Key wasn't in this slot, move to next (linear probing)
        index++;
        if (index >= table->capacity) {
            // At end of entries array, wrap around
            index = 0;
        }
    }
    return NULL;
}

ASTnode* ht_get_ASTnode(ht* table, const char* key) 
{
    // AND hash with capacity-1 to ensure it's within entries array
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {
            return table->entries[index].node;
        }
        // Key wasn't in this slot, move to next (linear probing)
        index++;
        if (index >= table->capacity) {
            // At end of entries array, wrap around
            index = 0;
        }
    }
    return NULL;
}


// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool ht_expand(ht* table) {
    // Allocate new entries array.
    size_t new_capacity = table->capacity * 2;
    if (new_capacity < table->capacity) {
        return false;  // overflow (capacity would be too big)
    }
    ht_entry* new_entries = calloc(new_capacity, sizeof(ht_entry));
    if (new_entries == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        ht_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, new_capacity, entry.key,
                         entry.value, NULL, entry.line, entry.node);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = new_capacity;
    return true;
}


const char* ht_set(ht* table, const char* key, const char* value, int line, ASTnode* node) {
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }

    // If length will exceed half of current capacity, expand it
    if (table->length >= table->capacity / 2) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }

    // Set entry and update length
    return ht_set_entry(table->entries, table->capacity, key, value, &table->length, line, node);
}


size_t ht_length(ht* table) {
    return table->length;
}


hti ht_iterator(ht* table) {
    hti it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(hti* it) {
    // Loop till we've hit end of entries array.
    ht* table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            ht_entry entry = table->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}





