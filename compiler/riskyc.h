#ifndef RISKYC
#define RISKYC

/* Size of data types in bytes */
#define SIZE_INT        4
#define SIZE_CHAR       1
#define SIZE_FLOAT      4
#define SIZE_DOUBLE     8
#define SIZE_POINTER    8

#include "ast.h"
#include "symboltable.h"
#include "ir.h"
#include "control_flow.h"
#include "register_allocation.h"

typedef struct
{
    int line, col, length;
} Coordinate;

typedef struct
{
    Coordinate src;
    const char *name;
    const char *datatype;
} ID_struct;



int declare(const char* name, const char* datatype, int line, ASTnode* node, ht*);
ASTnode* check_declaration(const char* name, ht*);
void ht_set_type_sp_offset(const char* key, id_type type, int sp_offset, ht*);

#endif
