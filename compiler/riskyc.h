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

#endif
