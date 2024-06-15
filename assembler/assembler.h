#ifndef ASSEMBLER
#define ASSEMBLER

#include <stdio.h>
#include <string.h>
#include "../compiler/ir.h"
#include "../compiler/instructions.h"

void assemble_binary_output(IR_node *IR_head, IR_node *IR_tail);
char* assemble_I_type(IR_node *node);
char* assemble_R_type(IR_node *node);
char* assemble_S_type(IR_node *node);
char* assemble_B_type(IR_node *node, IR_node *IR_head, IR_node *IR_tail);
char* assemble_J_type(IR_node *node, IR_node *IR_head, IR_node *IR_tail);

#endif