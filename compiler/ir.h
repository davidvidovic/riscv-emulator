#ifndef IR
#define IR

#include <stdio.h>
#include <string.h>
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
    IR_register ir_reg;
} IR_value;

typedef struct IR_node
{
    IR_instruction_type ir_type;
    instruction_type instr_type;
    char* instruction;
    
    struct IR_node *prev;
    struct IR_node *next;

    int reg;
    IR_value rs1;
    IR_value rs2;
    IR_value rd;
} IR_node;

// LIFO queue (stack) for labels
typedef struct LIFO_node {
    IR_node *ptr;
    struct LIFO_node *next;
} LIFO_node;

typedef struct Stack {
    LIFO_node* top;
} Stack;



IR_node* create_IR();
IR_node* insert_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack);
IR_node* populate_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack);

void init_stack(Stack* stack);
LIFO_node* create_LIFO_node(IR_node *node);
void push(Stack* stack, IR_node *node);
IR_node* pop(Stack *stack);
void print_IR(IR_node *IR_head, IR_node *IR_tail);


#endif