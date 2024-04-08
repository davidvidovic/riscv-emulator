#ifndef MAIN_H
#define MAIN_H

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

#endif
