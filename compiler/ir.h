#ifndef IR
#define IR

#include <stdio.h>
#include "ast.h"
#include "instructions.h"

typedef union IR_value
{
    int int_constant;
    float float_constant;
    char char_constant;
    char* name;
    char* label;
    int reg;
} IR_value;

typedef struct IR_node
{
    instruction_type ir_type;
    
    struct IR_node *prev;
    struct IR_node *next;

    int reg;
    IR_value rs1;
    IR_value rs2;
    IR_value rd;
} IR_node;

IR_node* create_IR();
IR_node* insert_IR(ASTnode *root, IR_node *head);
IR_node* populate_IR(ASTnode *root, IR_node *head);

#endif