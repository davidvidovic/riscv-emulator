#ifndef SYMBOL
#define SYMBOL

typedef struct
{
    unsigned int line, col, length;
} Coordinate;

typedef struct
{
    Coordinate src;
    char *name;
} IDstruct;


#endif
