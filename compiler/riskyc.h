#ifndef MAIN_H
#define MAIN_H

typedef struct
{
    int line, col, length;
} Coordinate;

typedef struct
{
    Coordinate src;
    const char *name;
} ID_struct;


#endif
