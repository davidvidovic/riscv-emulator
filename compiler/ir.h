#ifndef IR
#define IR

#include <stdio.h>
#include "ast.h"

void create_IR(ASTnode *root, FILE *asm_file);

#endif