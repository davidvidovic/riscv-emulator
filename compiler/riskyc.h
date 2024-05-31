#ifndef RISKYC
#define RISKYC

/* Size of data types in bytes */
#define SIZE_INT    4
#define SIZE_CHAR   1
#define SIZE_FLOAT  4
#define SIZE_DOUBLE 8

#include "ast.h"

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

//ASTnode *root = NULL;
int declare(const char* name, const char* datatype, int line, ASTnode* node);
ASTnode* check_declaration(const char* name);
void ht_set_type_sp_offset(const char* key, id_type type, int sp_offset);

#endif
