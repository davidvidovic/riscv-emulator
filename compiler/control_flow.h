#ifndef FLOW_CONTROL
#define FLOW_CONTROL

#include "ir.h"

typedef struct basic_block
{
    IR_node *leader;
    IR_node *last_instruction;

    struct basic_block *bb_prev;
    struct basic_block *bb_next;

    int bb_number;
} basic_block;

typedef struct control_flow_graph 
{
    basic_block *leader;
    basic_block *last_block;
} control_flow_graph;


/* CFG */
void init_cfg(control_flow_graph *cfg);
control_flow_graph* populate_cfg(IR_node *IR_head, IR_node *IR_tail);


/* Basic blocks */
basic_block* crate_basic_block(); 

#endif