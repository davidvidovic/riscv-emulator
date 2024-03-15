#ifndef SM_H
#define SM_H

#include <stddef.h>
#include <stdbool.h>

/*
*   Symbol table structure is to be implemented as a hash table
*   This is the best structure performance-wise
*   Language C does not provide hash-table API in it's standard lib
*   So this hash table functionality is implemented by hand
*   Referencing to Ben Hoyt's "How to implement a hash table in C" article
*/

// Symboltable struct that will be implemented as a hash table
typedef struct ht ht;

// Creates a hash table and returns pointer to it
ht* ht_create(void);

// Free memory allocated for given table
void ht_destroy(ht* table);

// Get item with given key (key is a string)
const char* ht_get(ht* table, const char* key);

// Set item with given key to a value (must not be NULL)
const char* ht_set(ht* table, const char* key, void* value);

// Returns number of items in table
size_t ht_length(ht* table);


// Hash table iterator
// In the case of compiler and symbol table, key will be IDENTIFIER's name
// And value will be it's DATATYPE
typedef struct {
    const char* key;
    const char* value;

    ht* _table;
    size_t _index;
} hti;

// Creates hash table iterator
hti ht_iterator(ht* table);

// Move iterator to the next item in table, update iterator key and value to current item and return true
// If no more items in table, return false
bool ht_next(hti* it);

#endif