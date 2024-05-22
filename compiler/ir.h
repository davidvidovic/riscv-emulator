#ifndef IR
#define IR

#include <stdio.h>
#include "ast.h"
#include "instructions.h"

typedef struct IR_node
{
    IR_instruction_type ir_type;
    
    struct IR_node *next;
    struct IR_node *prev;
} IR_node;

IR_node* create_IR(ASTnode *root);
void insert_IR(ASTnode *root, IR_node *node);
IR_node* create_IR_node(IR_instruction_type ir_type, IR_node *prev, IR_node *next);

#endif