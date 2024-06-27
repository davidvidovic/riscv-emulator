#ifndef register_allocation
#define register_allocation

#include "instructions.h"
#include "symboltable.h"

typedef struct register_pool {
    /* 
    * Array of 32 values to hold information which ID from Symbol Table (ST entry) is currently stored in which register 
    *
    * register_state[0] is never to be used since it points to x0 register. For convinience array is 32-element long and addressing 
    * array elements is done over enum IR_register type of data (defined in instructions.h)
    *
    * Initially all registers must be set to NULL
    */
    char** registers[32];
    int count[32];
} register_pool;

register_pool* init_register_pool();
void add_id_to_register(register_pool *rp, IR_register reg, char *id);
void print_register_allocation(register_pool *rp, IR_register reg);
void print_register_allocation_ALL(register_pool *rp);
void remove_id_from_register(register_pool *rp, IR_register reg, char *id);
void remove_register_allocation_ALL(register_pool *rp);
IR_register find_ID(register_pool *rp, const char* id);
IR_register find_empty_register(register_pool *rp);
IR_register find_empty_register_without_REGs(register_pool *rp, IR_register reg1, IR_register reg2);
IR_register min_count_register(register_pool *rp);
IR_register min_count_register_without_ID(register_pool *rp, char* id1);
IR_register min_count_register_without_IDs(register_pool *rp, char* id1, char* id2);
IR_register min_count_register_without_REGs(register_pool *rp, IR_register reg1, IR_register reg2);
int ht_find_id_elsewhere(register_pool *rp, ht* table, const char* id);
int get_register_count(register_pool* rp, IR_register reg);
char* get_id_from_register(register_pool* rp, IR_register reg);

/* a0-a7 regs */
IR_register find_empty_argument_register(register_pool *rp);
IR_register find_non_empty_argument_register(register_pool *rp);
void store_19(register_pool *rp);

#endif