#ifndef RISKYC
#define RISKYC

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
char* print_operation(operation_type op);
int declare(const char* name, const char* datatype, int line, ASTnode* node);
ASTnode* check_declaration(const char* name);

#endif
