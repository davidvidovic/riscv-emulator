#ifndef ST
#define ST

#include <stddef.h>
#include <stdbool.h>
#include "ast.h"
#include "instructions.h"
// #include "register_allocation.h"

/*
*   Symbol table structure is to be implemented as a hash table
*   This is the best structure performance-wise
*   Language C does not provide hash-table API in it's standard lib
*   So this hash table functionality is implemented by hand
*   Referencing to Ben Hoyt's "How to implement a hash table in C" article
*/

typedef enum AD_type {AD_id, AD_reg} AD_type;

typedef struct address_descriptor address_descriptor;

typedef struct {
    const char* key;
    const char* value;
    int line;
    ASTnode* node;
    id_type type;
    int sp_offset;

    /* Register allocation stuff */
    address_descriptor **AD; // array of address descriptors
    int AD_count;
} ht_entry;


typedef struct address_descriptor {
    AD_type type;
    char* id;
    IR_register reg;
} address_descriptor;


struct ht {
    ht_entry* entries;
    size_t capacity;
    size_t length;
    int sum_offset;
};

// Symboltable struct that will be implemented as a hash table
typedef struct ht ht;

// Creates a hash table and returns pointer to it
ht* ht_create(void);

// Free memory allocated for given table
void ht_destroy(ht* table);

// Get item with given key (key is a string)
const char* ht_get_key(ht* table, const char* key);
int ht_get_line(ht* table, const char* key);
ASTnode* ht_get_ASTnode(ht* table, const char* key);
ht_entry* get_ht_entry(ht* table, const char* key);
int get_sf_offset(ht* table, const char* key);
void ht_set_sf_offset(ht* table, const char* key, int offset);



/* AD stuff */

void ht_get_AD(ht* table, const char* key);
void ht_set_AD_id(ht* table, const char *key, const char* id);
void ht_set_AD_reg(ht* table, const char *key, IR_register reg);
void ht_remove_AD_id(ht *table, const char *key, const char* id);
void ht_remove_AD_reg(ht *table, const char *key, IR_register reg);
IR_register ht_get_AD_holding_reg(ht* table, const char* key);




// Set item with given key to a value (must not be NULL)
const char* ht_set(ht* table, const char* key, const char* value, int line, ASTnode* node, id_type type, int sp_offset);

// Returns number of items in table
size_t ht_length(ht* table);


// Hash table iterator
// In the case of compiler and symbol table, key will be IDENTIFIER's name
// And value will be it's DATATYPE
typedef struct {
    const char* key;
    const char* value;
    int line;
    ASTnode* node;
    id_type type;
    int sp_offset;

    ht* _table;
    size_t _index;
} hti;

// Creates hash table iterator
hti ht_iterator(ht* table);

// Move iterator to the next item in table, update iterator key and value to current item and return true
// If no more items in table, return false
bool ht_next(hti* it);

#endif
