#include "control_flow.h"

void init_cfg(control_flow_graph *cfg)
{
    cfg->leader = NULL;
    cfg->last_block = NULL;
}


int bb_cnt = 0;

void populate_cfg(control_flow_graph *cfg, IR_node *IR_head, IR_node *IR_tail)
{
    /* Itearte over entire IR and separate instuctions into basic blocks of linear code */
    /* 
    * Rules for building basic blocks:
    * 1. first instruction in IR (IR_head) is a leader
    * 2. Any target of any jump (cond or uncond) is a leader
    * 3. Any instruction following any jump (cond or uncond) is a leader
    */
    
    basic_block *bb_init = crate_basic_block(IR_head->prev, bb_cnt++);
    cfg->leader = bb_init;

    

    cfg->last_block = create_graph(bb_init, IR_head, IR_tail->next);
    cfg->last_block->last_instruction = IR_tail->next;

    basic_block *tmp = cfg->leader;
    while(tmp->bb_next != NULL)
    {
        printf("BLOCK %d: ", tmp->bb_number);
        if(tmp->leader->ir_type == LABEL)
        {
            printf("LABEL %s", tmp->leader->instruction);
        }
        else if (tmp->leader->ir_type == JAL)
        {
            printf("%s %s", tmp->leader->instruction, tmp->leader->rs1.label);
        }
        else
        {
            printf("%s R%d", tmp->leader->instruction, tmp->leader->rs1.reg);
        }

        printf(" and last instr is ");
        if(tmp->last_instruction->ir_type == LABEL)
        {
            printf("LABEL %s \n", tmp->last_instruction->instruction);
        }
        else if (tmp->last_instruction->ir_type == JAL)
        {
            printf("%s %s\n", tmp->last_instruction->instruction, tmp->last_instruction->rs1.label);
        }
        else if (tmp->last_instruction->ir_type == SW)
        {
            printf("%s %s\n", tmp->last_instruction->instruction, tmp->last_instruction->rd.name);
        }
        else printf("%s %s\n", tmp->last_instruction->instruction, tmp->last_instruction->rs1.name);
        

        tmp = tmp->bb_next;
    }
}


basic_block* create_graph(basic_block *this_block, IR_node *IR_head, IR_node *IR_tail)
{
    basic_block *bb_next;

    if(IR_tail->next->next != IR_head)
    {
        this_block = create_graph(this_block, IR_head, IR_tail->next);
    }

    

    if(IR_tail->ir_type == JAL
        || IR_tail->ir_type == JALR 
        || IR_tail->ir_type == BNE 
        || IR_tail->ir_type == BEQ
        || IR_tail->ir_type == BGE
        || IR_tail->ir_type == BLT
        || IR_tail->ir_type == BGT
        || IR_tail->ir_type == BLE
        || IR_tail->ir_type == BGEU
        || IR_tail->ir_type == BLTU
        || IR_tail->ir_type == LABEL
    )
    {
        /* Should I cut the cord like this here? */
        //IR_tail->next->prev = NULL; 
        //IR_tail->next = NULL; 

        bb_next = crate_basic_block(IR_tail, bb_cnt++);
        this_block->last_instruction = IR_tail->next;
        this_block->bb_next = bb_next;
        bb_next->bb_prev = this_block;
        return bb_next; 
    }   
    
    return this_block;
}


basic_block* crate_basic_block(IR_node *leader, int bb_number)
{
    basic_block *bb = (basic_block*)malloc(sizeof(basic_block));
    bb->leader = leader;
    bb->last_instruction = NULL;
    bb->bb_number = bb_number;
    bb->bb_next = NULL;
    bb->bb_prev = NULL;
}

