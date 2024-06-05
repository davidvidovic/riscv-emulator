#include "control_flow.h"

int bb_cnt = 0;

basic_block* crate_basic_block()
{
    basic_block *new_block = (basic_block*)malloc(sizeof(basic_block));;
    new_block->leader = NULL;
    new_block->last_instruction = NULL;
    new_block->bb_next = NULL;
    new_block->bb_prev = NULL;
    new_block->bb_number = bb_cnt++;
    return new_block;
}

control_flow_graph* populate_cfg(IR_node *IR_head, IR_node *IR_tail)
{
    control_flow_graph *cfg = (control_flow_graph*)malloc(sizeof(control_flow_graph));
    cfg->leader = NULL;
    cfg->last_block = NULL;

    /* IR head is HEAD_TYPE and next in line is .main */
    IR_head = IR_head->prev;

    basic_block *bb_lead = crate_basic_block(); 
    bb_lead->leader = IR_head;
    
    
 
    
    while(IR_head != IR_tail)
    {
        IR_head = IR_head->prev;

        if(IR_head->ir_type == IR_LABEL
        || IR_head->ir_type == IR_BRANCH
        || IR_head->ir_type == IR_JUMP)
        {           
            
            bb_lead->last_instruction = IR_head->next;

            basic_block *new_leader = crate_basic_block();
            new_leader->leader = IR_head;
            new_leader->bb_prev = bb_lead;
            bb_lead->bb_next = new_leader;
            
            if(bb_lead->bb_number == 0)
            {
                cfg->leader = bb_lead;
            }

            bb_lead = new_leader;
        }
    }
    bb_lead->last_instruction = IR_tail;
    cfg->last_block = bb_lead;
    return cfg;
}





