#include "ir.h"

/*
* Function that walks the AST and creates IR tree based on it
*/

int register_counter = 1;
int label_counter = 1;
int ST_pointer = 0;
IR_register argument_register_usage = a0;

extern ht** ST_vector;

IR_node* populate_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack, Stack *break_stack, Stack *continue_stack, Stack *return_stack, Stack *array_element_stack, register_pool *rp, ht* table)
{
    IR_node *this_node = NULL;

    if(root->nodetype == FUNCTION_NODE)
    { 
        this_node = insert_IR(root, head, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);

        if(root->right != NULL) 
        {
            this_node = populate_IR(root->right, this_node, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);
        }
    }
    else
    {
        if(root->right != NULL) 
        {
            this_node = populate_IR(root->right, head, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);
        }
    }


    /* Prio execution */

    if(root->nodetype == IF_NODE || root->nodetype == ELSE_NODE)
    { 
        this_node = insert_IR(root, this_node, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);
    }

    if(root->nodetype == OPERATION_NODE && root->operation == EQU_OP && root->left != NULL && root->left->nodetype == ARRAY_ELEMENT_NODE && root->right->nodetype == OPERATION_NODE)
    {
        push(array_element_stack, this_node);
    }

    if(root->right != NULL)
    {
        if((root->nodetype != ELSE_NODE && root->right->nodetype == IF_NODE) ||root->right->nodetype == ELSE_NODE || root->right->nodetype == WHILE_NODE || root->right->nodetype == FOR_NODE)
        {    
            this_node = insert_IR(root, this_node, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);
        }
    }


    /* Normal execution */
    if(root->nodetype == FUNCTION_HEAD_NODE)
    {
        table = ST_vector[ST_pointer++];
    }


    if(root->left != NULL && root->nodetype != FUNCTION_NODE) // watch out when added parameters to non-main function
    {
        if(this_node == NULL)
            this_node = populate_IR(root->left, head, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);
        else
            this_node = populate_IR(root->left, this_node, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);
    }

    if(this_node != NULL)
        head = this_node; 

    if(root->nodetype == FUNCTION_NODE)
    {
        char *ls = malloc(5 * sizeof(char));
        sprintf(ls, "L%d", root->right->value.label_count);
        while(stack->top != NULL)
        {
            IR_node *help = pop(stack);
            if(help->instr_type == JAL)
                help->rs1.label = ls;
            else if(help->ir_type == IR_BRANCH)
                help->rd.label = ls;
        }

        while(return_stack->top != NULL)
        {
            IR_node *help = pop(return_stack);
            if(help->instr_type == JAL)
                help->rs1.label = ls;
            else if(help->ir_type == IR_BRANCH)
                help->rd.label = ls;
        }

        return clean_up(rp, this_node, root->line, table->sum_offset);
    }

    if(root->nodetype == IF_NODE
        || root->nodetype == ELSE_NODE 
        || (root->right != NULL && (root->right->nodetype == IF_NODE 
                                    ||root->right->nodetype == ELSE_NODE 
                                    || root->right->nodetype == WHILE_NODE 
                                    || root->right->nodetype == FOR_NODE
                                    )
            )
        )
        return this_node;
    else
        return insert_IR(root, head, stack, secondary_stack, break_stack, continue_stack, return_stack, array_element_stack, rp, table);
}

/*
* Create head
*/

IR_node* create_IR()
{
    IR_node *IR_head = (IR_node *)malloc(sizeof(IR_node));
    IR_head->ir_type = NONE;
    IR_head->instr_type = HEAD;
    IR_head->next = NULL;
    IR_head->prev = NULL;
    IR_head->reg = 0;
    return IR_head;
}

/*
* Create base IR node
*/

IR_node* insert_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack, Stack *break_stack, Stack *continue_stack, Stack *return_stack, Stack* array_element_stack, register_pool *rp, ht* table)
{ 
    if(root->nodetype == CONSTANT_NODE && root->value.value_INT == 0)
        return head;

    IR_node *node = (IR_node *)malloc(sizeof(IR_node));
    
    node->prev = NULL;
    node->next = head;
    head->prev = node;
    
    node->reg = register_counter++;
    node->ir_type = NONE;
    node->instr_type = IR_NO_TYPE;

    char *tmp = malloc(5 * sizeof(char));
    IR_node *jmp = (IR_node *)malloc(sizeof(IR_node));

    switch(root->nodetype)
    {
        case CONSTANT_NODE:
            
        break;

        case ID_NODE:
            
        break;

        case POINTER_NODE:
            
        break;

        case ARRAY_ELEMENT_NODE:
            /* Node that represents array element, which is accessed by calculating expression inside [] */

            // Issue a load of a pointer, get it inside some register
            load_array_element(rp, table, root, &node, &head);
            
            
        break;

        case OPERATION_NODE:
            node->line = root->line;
            
            switch(root->operation)
            {
            	IR_register holding_reg;
                case EQU_OP:
                    /* 
                    * To replace unnecessary store instruction, register allocation algorithm is introduced.
                    * In a nutshell: register_poll holds information about which values are live in which register 
                    * every symbol_table entry has AD field (address_descriptor) which shows very can ACTIVE value of said ID can be found
                    * 
                    * On LOAD: check if ID is already in a register, if yes, use that register. If not, find empty register for use. If can't be fonud,
                    * find an appropriate register for spill (currently based on minimum ID's it refers to, which is very very gredeey, basiclly count
                    * will be greater than 1 only on copy instructions...).
                    * 
                    * On STORE: check if this ID is live inside any register, if yes, use that register as destination. 
                    * If not, some register needs to be assigned to this value. 
                    * If there is empty register, assign that register.
                    * If not, repeat algoritham for spilling. When register is choosen, store instructions must be issued for all live IDs inside it.
                     */

                    store_19(rp, table);
       
                    if(root->left->nodetype == ARRAY_ELEMENT_NODE)
                    {
                        if(root->right->nodetype == CONSTANT_NODE)
                        {
                            node = get_reg(rp, table, root->right, node, &head);
                        }
                        else if(root->right->nodetype == ID_NODE)
                        {
                            node = get_reg(rp, table, root, node, &head);
                            node->rd.reg = node->rs2.reg;
                        }
                        else
                        {
                            /* Operation to the right */
                            
                            node->rd.reg = pop(array_element_stack)->rd.reg;
                        }

                        IR_node *temp;
                        temp = store_array_element(rp, table, root, node, head);
                        temp->prev = NULL;
                        temp->next = node;
                        node->prev = temp;
                        return temp;
                    }

                    holding_reg = find_ID(rp, root->left->name);

                    if(holding_reg == 0)
                    {
                        IR_node *temp = (IR_node*)malloc(sizeof(IR_node));
                        
                        if(root->right->nodetype == ID_NODE)
                        {
                            /* Copy instruction */      
 
                            IR_register operand2_reg = find_ID(rp, root->right->name);

                            if(operand2_reg == 0)
                            {
                                node->ir_type = IR_LOAD;
                                node->instr_type = LW;
                                node->instruction = "lw";
                                node->rd.reg = holding_reg;
                                //node->rs1.name = root->right->name;
                                node->rs1.reg = s0;
                                node->sf_offset = get_sf_offset(table, root->right->name)*(-1);
                            }
                            else
                            {
                                while(get_register_count(rp, operand2_reg) != 0)
                                {
                                    IR_node *sw_temp = (IR_node*)malloc(sizeof(IR_node));
                                    sw_temp->ir_type = IR_STORE;
                                    sw_temp->instr_type = SW;
                                    sw_temp->instruction = "sw";
                                    sw_temp->rs1.reg = operand2_reg;
                                    sw_temp->rs1.name = get_id_from_register(rp, operand2_reg);
                                    sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
                                    sw_temp->rd.reg = s0;
                                    sw_temp->prev = NULL;
                                    sw_temp->next = head;
                                    head->prev = sw_temp;

                                    head = sw_temp;
                                    remove_id_from_register(rp, operand2_reg, sw_temp->rs1.name);
                                }
                                
                                node->next = head;
                                head->prev = node;
                                add_id_to_register(rp, operand2_reg, root->left->name);
                            }

                            node->line = root->line;
                            update_line_number_IR(&node);

                            
                            return node;
                        }

                        if(root->right->nodetype == CONSTANT_NODE)
                        {            
                            node = get_reg(rp, table, root, node, &head); 

                            load_imm(root->right, &node, &head);                           

                            node->line = root->line;
                            update_line_number_IR(&node);

                            if(root->left->nodetype == POINTER_NODE || root->left->nodetype == ARRAY_ELEMENT_NODE)
                            {
                                node = store_pointer(rp, table, &node, root->left->left);
                            }
                                
                            return node;
                        }

                        if(root->left->nodetype == CONSTANT_NODE)
                        {
                            node = get_reg(rp, table, root, node, &head);
                            
                            load_imm(root->left, &node, &head);
                              

                            node->line = root->line;
                            update_line_number_IR(&node);
                            return node;
                        }

                        if(root->left == root->right->left)
                        {
                            head->prev = NULL;
                            head->rd.reg = head->rs1.reg;

                            head->line = root->line;
                            update_line_number_IR(&head);
                            return head;
                        }

                        if(root->left == root->right->right)
                        {
                            head->prev = NULL;
                            if(root->right->left == CONSTANT_NODE)
                            {
                            	head->rd.reg = head->rs2.reg;
                            }
                            else
                            {
                            	head->rd.reg = head->rs1.reg;
                            }

                            head->line = root->line;
                            update_line_number_IR(&head);
                            return head;
                        }
                        
                	    /* Assign new register to hold resulting value of past OPERATION */
                        
                	    temp = get_reg(rp, table, root, head, &(head->next));   

                        temp->line = root->line;
                        update_line_number_IR(&temp); 

                        if(root->left->nodetype == POINTER_NODE)
                        {
                            temp = store_pointer(rp, table, &temp, root->left);
                        }

		                return temp;
                    }
                    else
                    {
                        /* There is an operation to the right so just edit it's rd register */
                        if(root->right->nodetype != CONSTANT_NODE && root->right->nodetype != ID_NODE  && root->right->nodetype != POINTER_NODE)
                        {
                            head->prev = NULL;
                            head->rd.reg = holding_reg;

                            head->line = root->line;
                            update_line_number_IR(&head);

                            if(root->left->nodetype == POINTER_NODE)
                            {
                                head = store_pointer(rp, table, &head, root->left);
                            }

                            return head;
                        }
                        else if(root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE)
                        {
                            /* Copy instruction */

                            /* In exspression: c = b; 
                            * If c exists in some register already, look for b in another register
                            * If b is found, remove c from it's register and simply add c to b's register, but first store everything from b's register
                            * If b is not found, load value of b inside C's existing register
                            */
                            IR_register operand2_reg = find_ID(rp, root->right->name);

                            if(operand2_reg == 0)
                            {
                                node->ir_type = IR_LOAD;
                                node->instr_type = LW;
                                node->instruction = "lw";
                                node->rd.reg = holding_reg;
                                //node->rs1.name = root->right->name;
                                node->rs1.reg = s0;
                                node->sf_offset = get_sf_offset(table, root->right->name)*(-1);
                            }
                            else
                            {
                                remove_id_from_register(rp, holding_reg, root->left->name);

                                while(get_register_count(rp, operand2_reg) != 0)
                                {
                                    IR_node *sw_temp = (IR_node*)malloc(sizeof(IR_node));
                                    sw_temp->ir_type = IR_STORE;
                                    sw_temp->instr_type = SW;
                                    sw_temp->instruction = "sw";
                                    sw_temp->rs1.reg = operand2_reg;
                                    sw_temp->rs1.name = get_id_from_register(rp, operand2_reg);
                                    sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
                                    sw_temp->rd.reg = s0;
                                    sw_temp->prev = NULL;
                                    sw_temp->next = head;
                                    head->prev = sw_temp;

                                    head = sw_temp;
                                    remove_id_from_register(rp, operand2_reg, sw_temp->rs1.name);
                                }

                                add_id_to_register(rp, operand2_reg, root->left->name);

                                if(root->left->nodetype == POINTER_NODE)
                                {
                                    head = store_pointer(rp, table, &head, root->left);
                                }

                                return head;
                            }
                        }
                        else 
                        {
                            node->rd.reg = holding_reg;

                            load_imm(root->right, &node, &head);

                            if(root->left->nodetype == POINTER_NODE)
                            {
                                node = store_pointer(rp, table, &node, root->left);
                            }

                            node->line = root->line;
                            update_line_number_IR(&node);

                            return node;
                        }
                    }
                break;

                case ADD_OP:

                    node = get_OP_node(rp, table, root, node, &head);

                    node->rd.reg = get_holding_reg(rp, table, node, &head, 0, 0);       

                    /* Very dirty */
                    add_id_to_register(rp, node->rd.reg, "19");
                    
                    if(root->right->nodetype == CONSTANT_NODE)
                    {
                        node->ir_type = IR_OP_IMM;
                        node->instr_type = ADDI;
                        node->instruction = "addi";

                        node->rs2.int_constant = root->right->value.value_INT; // INT only
                        return node;
                        // Delete load const IR_node, two nodes ago
                        //node->next->next->next->prev = node->next;
                        //node->next->next = node->next->next->next->next;
                        
                    }
                    else if(root->left->nodetype == CONSTANT_NODE)
                    {
                        node->ir_type = IR_OP_IMM;
                        node->instr_type = ADDI;
                        node->instruction = "addi";

                        if(root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE)
                        {
                            node->rs1.reg = node->rs2.reg;
                        }

                        node->rs2.int_constant = root->left->value.value_INT; // INT only    
                    }
                    else
                    {
                        node->ir_type = IR_OP;
                        node->instr_type = ADD;
                        node->instruction = "add";
                    }
                    return node;
                break;

                case SUB_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                
                    node->instr_type = SUB;
                    node->ir_type = IR_OP;
                    node->instruction = "sub";
                break;

                /*
                * If it is logical expression it should be followed by branch or jump instructions
                * No matter if it is expression evaluating if statement or value assigment to variable
                */
                case LOGIC_EQU_OP:
                    node = get_OP_node(rp, table, root, node, &head);

                    node->instr_type = BNE;
                    node->ir_type = IR_BRANCH;
                    node->instruction = "bne";         
                    push(stack, node);       
                break;

                case LOGIC_GET_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                    
                    node->instr_type = BLT;
                    node->ir_type = IR_BRANCH;
                    node->instruction = "blt";
                    push(stack, node); 
                break;

                case LOGIC_GT_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                    
                /*
                * BLE - branch if less or equal => is BGE when operand are reversed
                */
                    node->instr_type = BGE;
                    node->ir_type = IR_BRANCH;
                    node->instruction = "bge";
                    push(stack, node); 
                break;

                case LOGIC_LET_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                    
                /*
                * BGT - branch if greater than => is BLT when operand are reversed
                */
                    node->instr_type = BLT;
                    node->ir_type = IR_BRANCH;
                    node->instruction = "blt";
                    push(stack, node); 
                break;

                case LOGIC_LT_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                    
                    // godbolt does optimization by reducing constant operand by one... and using diff instr
                    node->instr_type = BGE;
                    node->ir_type = IR_BRANCH;
                    node->instruction = "bge";
                    push(stack, node);  
                    
                break;

                case LOGIC_NEQU_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                    
                    node->instr_type = BEQ;
                    node->ir_type = IR_BRANCH;
                    node->instruction = "beq";
                    push(stack, node); 
                break;

                case LOGIC_NOT_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                    
                    node->instr_type = SLTIU;
                    node->ir_type = IR_OP_IMM;
                    node->instruction = "sltiu";
                    node->rd.reg = node->rs1.reg;
                    node->rs2.int_constant = 1;
                    
                    //push(stack, node); // no need to push anything on stack, but resolves "stack empty" bug the easiest way

                    IR_node *nxt = (IR_node *)malloc(sizeof(IR_node));
                    nxt->next = node;
                    nxt->prev = NULL;
                    node->prev = nxt;

                    nxt->instr_type = ANDI;
                    nxt->ir_type = IR_OP_IMM;
                    nxt->instruction = "andi";
                    nxt->rd.reg = node->rd.reg;
                    nxt->rs1.reg = node->rs1.reg;
                    nxt->rs2.int_constant = 0xff;

                    IR_node *insert_branch = (IR_node *)malloc(sizeof(IR_node));
                    insert_branch->next = nxt;
                    insert_branch->prev = NULL;
                    nxt->prev = insert_branch;

                    // Only if it is loop's boolean expression!
                    insert_branch->instr_type = BNE;
                    insert_branch->ir_type = IR_BRANCH;
                    insert_branch->instruction = "bne";

                    insert_branch->rs1.reg = node->rs1.reg;
                    insert_branch->rs2.reg = 0;
                    push(stack, insert_branch);

                    insert_branch->line = root->line;
                    update_line_number_IR(&insert_branch);
                    return insert_branch;
                break;

                case LOGIC_AND_OP:
                    IR_node *id_left = (IR_node*)malloc(sizeof(IR_node));
                    IR_node *id_right = (IR_node*)malloc(sizeof(IR_node));

                    if(root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE)
                    {
                        id_right = create_BEQ_node(rp, table, root->right, id_right, &(node->next));
                        id_right->next = head;
                        push(stack, id_right); 

                        id_left->prev = NULL;
                        id_left->next = id_right;
                        id_right->prev = id_left;
                    }
                    else if(root->right->nodetype == CONSTANT_NODE)
                    {
                        id_right = create_BEQ_node(rp, table, root->right, id_right, &(node->next));
                        id_right->next = head;
                        push(stack, id_right); 

                        id_left->prev = NULL;
                        id_left->next = id_right;
                        id_right->prev = id_left;
                    }
                    else
                    {
                        // On the right OPERATION (nested logical ops)
                        id_left->prev = NULL;
                        id_left->next = head;
                        head->prev = id_left;
                    }

                    id_left = create_BEQ_node(rp, table, root->left, id_left, &(id_left->next));
                    push(stack, id_left); 

                    id_left->line = root->line;
                    update_line_number_IR(&id_left);
                    
                    return id_left;
                    break;


                case LOGIC_OR_OP:
                    
                    id_left = (IR_node*)malloc(sizeof(IR_node));
                    id_right = (IR_node*)malloc(sizeof(IR_node));

                    if(root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE)
                    {
                        id_right = create_BNE_node(rp, table, root->right, id_right, &(node->next));
                        id_right->next = head;
                        push(secondary_stack, id_right); 

                        id_left->prev = NULL;
                        id_left->next = id_right;
                        id_right->prev = id_left;
                    }
                    else if(root->right->nodetype == CONSTANT_NODE)
                    {
                        id_right = create_BNE_node(rp, table, root->right, id_right, &(node->next));
                        id_right->next = head;
                        push(secondary_stack, id_right); 

                        id_left->prev = NULL;
                        id_left->next = id_right;
                        id_right->prev = id_left;
                    }
                    else
                    {
                        // On the right OPERATION (nested logical ops)
                        IR_node *popped = pop(stack);
                        popped->instr_type = BNE;
                        popped->instruction = "bne";
                        push(secondary_stack, popped); // holy shit
                        id_left->prev = NULL;
                        id_left->next = head;
                        head->prev = id_left;
                    }
               
                    id_left = create_BEQ_node(rp, table, root->left, id_left, &(id_left->next));          
                    push(stack, id_left); 

                    id_left->line = root->line;
                    update_line_number_IR(&id_left);
                    return id_left;
                break;

                case BITWISE_NOT_OP:
                    /* Pseudo NOT by ISA docs */
                    node = get_OP_node(rp, table, root, node, &head);
                    node->instr_type = XORI;
                    node->ir_type = IR_OP_IMM;
                    node->instruction = "xori";

                    node->rs2.int_constant = -1;
                break;
            }

            /* Implement other operations */    

            node->line = root->line;
            update_line_number_IR(&node);
        break;

        case IF_NODE:
            root->value.label_count = label_counter++;
            node->instr_type = LABEL;
            node->ir_type = IR_LABEL;
            
            sprintf(tmp, "L%d", root->value.label_count);
            node->instruction = tmp;
            root->left->value.label_count = root->value.label_count;

            while(secondary_stack->top != NULL)
            {
                IR_node* help = pop(secondary_stack);
                if(help->ir_type == IR_JUMP)
                    help->rs1.label = tmp;
                else if(help->ir_type == IR_BRANCH)
                    help->rd.label = tmp;
            }

            node->line = root->line;
            update_line_number_IR(&node);
        break;

        case ELSE_NODE:           
            jmp->instr_type = JAL;
            jmp->ir_type = IR_JUMP;
            jmp->instruction = "jal";
            jmp->rd.reg = 0; // by ISA docs - pseudo j (jamp) instruction is jal with rd set as x0
            // jmp->next = head->next; // delete IFs node
            // jmp->prev = NULL;
            // head->next->prev = jmp;
            
            head->prev = jmp;
            jmp->next = head;

            node->next = jmp;
            node->prev = NULL;
            jmp->prev = node;

            root->value.label_count = label_counter++;

            node->instr_type = LABEL;
            node->ir_type = IR_LABEL;
            
            sprintf(tmp, "L%d", root->value.label_count);
            node->instruction = tmp;
            root->left->value.label_count = root->value.label_count;
            IR_node *help = pop(stack);
            if(help->ir_type == IR_JUMP)
                help->rs1.label = tmp;
            else if(help->ir_type == IR_BRANCH)
                help->rd.label = tmp;

            ASTnode *tail = root->right->right;
            tail->right = NULL;
            int num_AND = count_subtree_AND_OPs(tail);

            for(int i = 0; i < num_AND; i++)
            {
                help = pop(stack);
                if(help->ir_type == IR_JUMP)
                    help->rs1.label = tmp;
                else if(help->ir_type == IR_BRANCH)
                    help->rd.label = tmp;
            }
            
            push(stack, jmp);

            node->line = root->line;
            update_line_number_IR(&node);
        break;

        case EXPRESSION_NODE:

            if(root->right != NULL)
            {
                char *tmp = malloc(5 * sizeof(char));
                ASTnode *tail;
                if(root->right->nodetype == IF_NODE)
                {
                    node->instr_type = LABEL;
                    node->ir_type = IR_LABEL;
                    root->value.label_count = label_counter++;
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;                    

                    IR_node *help = pop(stack);
                    if(help->ir_type == IR_JUMP)
                        help->rs1.label = tmp;
                    else if(help->ir_type == IR_BRANCH)
                        help->rd.label = tmp;

                    /* 
                    * When there are multiple subexpressions or operands in logical expressions, for each branching
                    * there will be and instruction node created and it will be pushed to stack, so I make a temporary sub-tree
                    * and cut sub-tree from the rest of AST by inserting NULL pointer to its tail, so I traverse only subtree from current above-IF node
                    * down to boolean expression of if node and count how many times I need to pop nodes off the stack 
                    */

                    tail = root->right->right;
                    tail->right = NULL;
                    int num_AND = count_subtree_AND_OPs(tail);

                    for(int i = 0; i < num_AND; i++)
                    {
                        help = pop(stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }

                    tail = root->right;
                    tail->right = NULL;
                    int num_BREAKS = count_subtree_BREAKs(tail);
                    
                    for(int i = 0; i < num_BREAKS; i++)
                    {
                        help = pop(break_stack);
                        if(help->ir_type == IR_JUMP)
                        {
                            help->rs1.label = tmp;
                        }
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }

                    node->line = root->line;
                    update_line_number_IR(&node);

                }
                else if(root->right->nodetype == ELSE_NODE)
                {
                    node->instr_type = LABEL;
                    node->ir_type = IR_LABEL;
                    root->value.label_count = label_counter++;
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;                    

                    IR_node *help = pop(stack);
                    if(help->ir_type == IR_JUMP)
                        help->rs1.label = tmp;
                    else if(help->ir_type == IR_BRANCH)
                        help->rd.label = tmp;
                    
                    tail = root->right->right;
                    tail->right = NULL;
                    int num_BREAKS = count_subtree_BREAKs(tail);
                    tail = root->right;
                    tail->right = NULL;
                    num_BREAKS += count_subtree_BREAKs(tail);
                    
                    for(int i = 0; i < num_BREAKS; i++)
                    {
                        help = pop(break_stack);
                        if(help->ir_type == IR_JUMP)
                        {
                            help->rs1.label = tmp;
                        }
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }
                    

                    node->line = root->line;
                    update_line_number_IR(&node);              
                }
                else if(root->right->nodetype == WHILE_NODE)
                {
                    node->instr_type = LABEL;
                    node->ir_type = IR_LABEL;
                    root->value.label_count = label_counter++;
                    
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp; 

                    ASTnode *tail = root->right->right->right;
                    tail->right = NULL;

                    IR_node *help = pop(stack);
                    sprintf(tmp, "L%d", root->value.label_count);
                    if(help->ir_type == IR_JUMP)
                        help->rs1.label = tmp;
                    else if(help->ir_type == IR_BRANCH)
                        help->rd.label = tmp;

                    int num_AND = count_subtree_AND_OPs(tail);

                    for(int i = 0; i < num_AND; i++)
                    {
                        help = pop(stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }

                    tail = root->right;
                    tail->right = NULL;
                    int num_BREAKS = count_subtree_BREAKs(tail);

                    for(int i = 0; i < num_BREAKS; i++)
                    {
                        help = pop(break_stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }

                    node->line = root->line;
                    update_line_number_IR(&node);
                }
                else if(root->right->nodetype == FOR_NODE)
                {
                    node->instr_type = LABEL;
                    node->ir_type = IR_LABEL;
                    root->value.label_count = label_counter++;
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp; 

                    IR_node *help = pop(stack);

                    sprintf(tmp, "L%d", root->value.label_count);
                    if(help->ir_type == IR_JUMP)
                        help->rs1.label = tmp;
                    else if(help->ir_type == IR_BRANCH)
                        help->rd.label = tmp;

                    ASTnode *tail = root->right->right;
                    tail->right = NULL;

                    int num_AND = count_subtree_AND_OPs(tail);

                    for(int i = 0; i < num_AND; i++)
                    {
                        help = pop(stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }

                    tail = root->right;
                    tail->right = NULL;
                    int num_BREAKS = count_subtree_BREAKs(tail);

                    for(int i = 0; i < num_BREAKS; i++)
                    {
                        help = pop(break_stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }

                    node->line = root->line;
                    update_line_number_IR(&node);
                }
                else if(root->right->nodetype == LABEL_NODE)
                {
                    root->value.label_count = root->right->value.label_count;

                    if(root->right->right != NULL && root->right->right->nodetype == SCOPE_NODE) // FOR incr/decr expression
                    {
                        while(continue_stack->top != NULL)
                        {
                            IR_node *help = pop(continue_stack);

                            sprintf(tmp, "L%d", root->value.label_count);
                            if(help->ir_type == IR_JUMP)
                                help->rs1.label = tmp;
                            else if(help->ir_type == IR_BRANCH)
                                help->rd.label = tmp;
                        }
                    }
                }
            }
        break;

        case SCOPE_NODE:
            argument_register_usage = a0;
            if(root->left != NULL)
            {
                if(root->left->nodetype != EXPRESSION_NODE && root->left->nodetype != RETURN_NODE) // when something other than exp is to the left of scope
                {
                    node->instr_type = LABEL; 
                    node->ir_type = IR_LABEL;
                    char *tmp = malloc(5 * sizeof(char));
                    root->value.label_count = label_counter++;
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;     

                    
                    tail = root->left;
                    //tail->right = NULL;
                    int num_BREAKS = count_subtree_BREAKs(tail);
                    
                    for(int i = 0; i < num_BREAKS; i++) 
                    {
                        help = pop(break_stack);
                        if(help->ir_type == IR_JUMP)
                        {
                            help->rs1.label = tmp;
                        }
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }

                    if(root->left->nodetype == CASE_NODE)
                    {
                        help = pop(stack);
                        if(help->ir_type == IR_JUMP)
                        {
                            help->rs1.label = tmp;
                        }
                        else if(help->ir_type == IR_BRANCH)
                            help->rd.label = tmp;
                    }
                }
                else
                {
                    root->value.label_count = root->left->value.label_count;
                }

                node->line = root->line;
                update_line_number_IR(&node);
            }      
        break;

        case FUNCTION_NODE:
            node->instr_type = LABEL;
            node->ir_type = IR_LABEL;
            node->instruction = root->left->name;
                      
            /* Stack frame allocation time */
            IR_node *stack_allocation_node = (IR_node*)malloc(sizeof(IR_node));
            stack_allocation_node->ir_type = IR_OP_IMM;
            stack_allocation_node->instr_type = ADDI;
            stack_allocation_node->instruction = "addi";
            stack_allocation_node->rd.reg = sp;
            stack_allocation_node->rs1.reg = sp;

            // stack pointer is always 16-byte aligned
            table->sum_offset += table->sum_offset % 16;
            stack_allocation_node->rs2.int_constant = table->sum_offset*(-1);

            node->prev = stack_allocation_node;
            stack_allocation_node->next = node;


            /* Save return address */
            IR_node *save_ra = (IR_node*)malloc(sizeof(IR_node));
            save_ra->ir_type = IR_STORE;
            save_ra->instr_type = SD;
            save_ra->instruction = "sd";
            save_ra->rs1.reg = ra;
            save_ra->sf_offset = (table->sum_offset - 8);
            save_ra->rd.reg = sp;

            stack_allocation_node->prev = save_ra;
            save_ra->next = stack_allocation_node;
            
            
            /* Save stack frame s0 */
            IR_node *save_s0 = (IR_node*)malloc(sizeof(IR_node));
            save_s0->ir_type = IR_STORE;
            save_s0->instr_type = SD;
            save_s0->instruction = "sd";
            save_s0->rs1.reg = s0;
            save_s0->sf_offset = (table->sum_offset - 16);
            save_s0->rd.reg = sp;

            save_ra->prev = save_s0;
            save_s0->next = save_ra;


            /* Allocate new stack frame s0 */
            IR_node *allocate_s0 = (IR_node*)malloc(sizeof(IR_node));
            allocate_s0->ir_type = IR_OP_IMM;
            allocate_s0->instr_type = ADDI;
            allocate_s0->instruction = "addi";
            allocate_s0->rd.reg = s0;
            allocate_s0->rs1.reg = sp;
            allocate_s0->rs2.int_constant = table->sum_offset;

            save_s0->prev = allocate_s0;
            allocate_s0->next = save_s0;
            allocate_s0->prev = NULL;

            

            allocate_s0->line = ht_get_line(table, root->left->name);
            update_line_number_IR(&allocate_s0);
            
            return allocate_s0;
        break;

        case WHILE_NODE:
            //IR_node *temp_node = (IR_node *)malloc(sizeof(IR_node));

            node->instr_type = JAL;
            node->ir_type = IR_JUMP;
            node->instruction = "jal";
            node->rd.reg = 0; // by ISA docs - pseudo j (jump) instruction is jal with rd set as x0
            //push(stack, node);
            sprintf(tmp, "L%d", root->right->right->right->value.label_count);
            node->rs1.label = tmp;
            root->value.label_count = root->right->right->right->value.label_count;
            //root->value.label_count = label_counter++;

            node->line = root->line;
            update_line_number_IR(&node);
        break;

        case FOR_NODE:
            node->instr_type = JAL;
            node->ir_type = IR_JUMP;
            node->instruction = "jal";
            node->rd.reg = 0; // by ISA docs - pseudo j (jump) instruction is jal with rd set as x0
            sprintf(tmp, "L%d", root->right->right->value.label_count);
            node->rs1.label = tmp;
            root->value.label_count = root->right->right->value.label_count;
            //root->value.label_count = label_counter++;
            node->line = root->line;
            update_line_number_IR(&node);
        break;

        case LABEL_NODE:
            /* This is a special type of node for inserting label before while boolean expression is evaluated */
            node->instr_type = LABEL;
            node->ir_type = IR_LABEL;
            root->value.label_count = label_counter++;
            sprintf(tmp, "L%d", root->value.label_count);
            node->instruction = tmp; 

            /* Risky thing about to happen */
            while(secondary_stack->top != NULL)
            {
                IR_node *help = pop(secondary_stack);
                if(help->instr_type == JAL)
                    help->rs1.label = tmp;
                else if(help->ir_type == IR_BRANCH)
                    help->rd.label = tmp;
            }

            if(root->right != NULL)
            {
                if(root->right->left != NULL && root->right->left->nodetype == OPERATION_NODE)
                {
                    node->line = root->right->left->line;
                }
            }
            
            //update_line_number_IR(&node);
        break;


        case BREAK_NODE:
            node->ir_type = IR_JUMP;
            node->instr_type = JAL;
            node->instruction = "jal";
            node->rd.reg = 0;
            push(break_stack, node);
            node->line = root->line;
            update_line_number_IR(&node);
        break;

        case CASE_NODE:
            root->value.label_count = root->right->value.label_count;
            sprintf(tmp, "L%d", root->value.label_count);
            
            if(root->right->right != NULL && root->right->right->nodetype == CASE_NODE)
            {
                IR_node* temp_pop = pop(stack);
                help = pop(stack);
                push(stack, temp_pop);
            
                if(help->ir_type == IR_JUMP)
                    help->rs1.label = tmp;
                else if(help->ir_type == IR_BRANCH)
                    help->rd.label = tmp;
            }

            //node->line = root->line;
            //update_line_number_IR(&node);
        break;

        case DEFAULT_NODE:
            root->value.label_count = root->right->value.label_count;
            sprintf(tmp, "L%d", root->value.label_count);
            
            if(root->right->right != NULL && root->right->right->nodetype == CASE_NODE)
            {
                help = pop(stack);
            
                if(help->ir_type == IR_JUMP)
                    help->rs1.label = tmp;
                else if(help->ir_type == IR_BRANCH)
                    help->rd.label = tmp;
            }
        break;


        case CONTINUE_NODE:
            node->ir_type = IR_JUMP;
            node->instr_type = JAL;
            node->instruction = "jal";
            node->rd.reg = 0;
            push(continue_stack, node);
            node->line = root->line;
            update_line_number_IR(&node);
        break;


        case RETURN_NODE:
            /* Save return exp into a5 */

            if(root->left != NULL)
            {
                IR_node *save_return = (IR_node*)malloc(sizeof(IR_node));
                
                load_imm(root->left->left, &save_return, &head);
                
                if(root->left->left->nodetype == ID_NODE)
                {
                    save_return->rs1.reg = get_reg_single_ID(rp, table, root->left->left, save_return, &head)->rs1.reg;
                }

                save_return->next = head;
                head->prev = save_return;
                save_return->rd.reg = a5;
                save_return->prev = node;
                node->next = save_return;       
            }

            node->ir_type = IR_JUMP;
            node->instr_type = JAL;
            node->instruction = "jal";
            node->rd.reg = 0;
            push(return_stack, node);
            node->line = root->line;
            update_line_number_IR(&node);
        break;

        case FUNCTION_CALL_NODE:
            node->ir_type = IR_CALL;
            node->instruction = "call";
            node->rd.name = root->left->name;
            
            store_19(rp, table);
            remove_register_allocation_ALL(rp);
        break;

        case ARGUMENT_NODE:

            /* Store arguments on function call in a0-a1 | a2-a7 regs */
            if(root->left->nodetype == CONSTANT_NODE)
            {
                IR_node *temp = (IR_node*)malloc(sizeof(IR_node));
                temp = get_reg(rp, table, root->left, temp, &head);
                temp->prev = node;
                temp->next = head;
                head->prev = temp;
                node->next = temp;
                store_argument(rp, table, &node, &temp, temp->rd.reg);
            }
            else if(root->left->nodetype == ID_NODE)
            {
                node = get_reg(rp, table, root, node, &head);
                store_argument(rp, table, &node, &head, node->rs1.reg);
            }
            {
                /* Opeartion as an argument */
            }
        break;

        case PARAMETER_NODE:
            /* Load value from a0-a7 regs */
            node = load_parameter(rp, table, root, node, &head);
        break;
    }


    if(node->ir_type == NONE)
        return head;
    
    return node;
}

int count_subtree_AND_OPs(ASTnode* root)
{
    int temp = 0;

    if(root->right != NULL)
        temp += count_subtree_AND_OPs(root->right);

    if(root->left != NULL)
        temp += count_subtree_AND_OPs(root->left); 

    if(root->operation == LOGIC_AND_OP)
    {
        temp++;
    }

    return temp;
}

int count_subtree_BREAKs(ASTnode* root)
{
    int temp = 0;

    if(root->right != NULL)
        temp += count_subtree_BREAKs(root->right);

    if(root->left != NULL)
        temp += count_subtree_BREAKs(root->left); 

    if(root->nodetype == BREAK_NODE)
    {
        temp++;
    }

    return temp;
}


/*
* Create LIFO node for LIFO queue for labels
*/

void init_stack(Stack* stack)
{
    stack->top = NULL;
}

LIFO_node* create_LIFO_node(IR_node *node)
{
    LIFO_node* item = (LIFO_node*)malloc(sizeof(LIFO_node));
    item->ptr = node;
    item->next = NULL;
    return item;
}

void push(Stack *stack, IR_node *node)
{
    LIFO_node* item = create_LIFO_node(node);

    if(stack->top == NULL)
    {
        stack->top = item;
    }
    else
    {
        item->next = stack->top;
        stack->top = item;
    }
}

IR_node* pop(Stack *stack)
{
    if(stack->top == NULL)
    {
        printf("Stack empty\n");
        return NULL;
    }

    LIFO_node *temp = stack->top;
    IR_node *ptr = temp->ptr;
    if(temp->next == NULL)
    {
        stack->top = NULL;
    }
    else
    {
        stack->top = temp->next;
    }
    return ptr;
}


void print_IR(IR_node *IR_head, IR_node *IR_tail)
{
    printf("\n\nASM:\n\n");

    int ir_address = 0;

    FILE *asm_file = fopen("../output.s", "w");
    FILE *rows_lookup_file = fopen("lookup_rows.txt", "w");


    while(IR_head != IR_tail)
    {
      IR_head = IR_head->prev;

      if(IR_head->line != 0) fprintf(rows_lookup_file, "%d\n", IR_head->line);

      IR_head->ir_address = ir_address;
      ir_address += 4;

      switch(IR_head->ir_type)
      {
        case IR_LABEL:
          printf("%.4x:\t.%s:\n", IR_head->ir_address, IR_head->instruction);
          fprintf(asm_file, "%.4x:\t.%s:\n", IR_head->ir_address, IR_head->instruction);
        break;

        case IR_OP_IMM:
          printf("%.4x:\t\t%s x%d,x%d,%d\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.int_constant);
          fprintf(asm_file, "%.4x:\t\t%s x%d,x%d,%d\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.int_constant);
        break;

        case IR_STORE:
          printf("%.4x:\t\t%s x%d,%d(x%d)\n", IR_head->ir_address, IR_head->instruction, IR_head->rs1.reg, IR_head->sf_offset, IR_head->rd.reg);
          fprintf(asm_file, "%.4x:\t\t%s x%d,%d(x%d)\n", IR_head->ir_address, IR_head->instruction, IR_head->rs1.reg, IR_head->sf_offset, IR_head->rd.reg);
        break;

        case IR_LOAD:
          printf("%.4x:\t\t%s x%d,%d(x%d)\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->sf_offset, IR_head->rs1.reg);
          fprintf(asm_file, "%.4x:\t\t%s x%d,%d(x%d)\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->sf_offset, IR_head->rs1.reg);
        break;

        case IR_LOAD_IMM:
          printf("%.4x:\t\t%s x%d,%d\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.int_constant);
          fprintf(asm_file, "%.4x:\t\t%s x%d,%d\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.int_constant);
        break;        

        case IR_OP:
          printf("%.4x:\t\t%s x%d,x%d,x%d\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
          fprintf(asm_file, "%.4x:\t\t%s x%d,x%d,x%d\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
        break;

        case IR_BRANCH:
          printf("%.4x:\t\t%s x%d,x%d,.%s\n", IR_head->ir_address, IR_head->instruction, IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
          fprintf(asm_file, "%.4x:\t\t%s x%d,x%d,.%s\n", IR_head->ir_address, IR_head->instruction, IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
        break;

        case IR_JUMP:
            if(IR_head->instr_type == JAL)
            {
                printf("%.4x:\t\t%s x%d,.%s\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.label);
                fprintf(asm_file, "%.4x:\t\t%s x%d,.%s\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->rs1.label);
            }
            else
            {
                printf("%.4x:\t\t%s x%d,%d(x%d)\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->sf_offset, IR_head->rs1.reg);
                fprintf(asm_file, "%.4x:\t\t%s x%d,%d(x%d)\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.reg, IR_head->sf_offset, IR_head->rs1.reg);
            }
        break;

        case IR_CALL:
          printf("%.4x:\t\t%s %s\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.name);
          fprintf(asm_file, "%.4x:\t\t%s %s\n", IR_head->ir_address, IR_head->instruction, IR_head->rd.name);
        break;

        case NONE:
        break;
      }
    }

    fclose(asm_file);
    fclose(rows_lookup_file);
}


IR_node* get_reg(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    IR_register holding_reg;

    if(root->nodetype == CONSTANT_NODE)
    {
        load_imm(root, &node, &head);

        holding_reg = find_empty_register(rp);

        if(holding_reg == 0)
        {
            /* Pick one register for spilling */
            // For now, I'll do this based on minimum count

            /* WARNING: VERY GREEDY! */
            holding_reg = min_count_register(rp);   
    
            // IR_node* sw_temp;
            // if(get_register_count(rp, holding_reg) != 0)
            // {             
            //     sw_temp = (IR_node*)malloc(sizeof(IR_node)); 	
            //     sw_temp->instr_type = SW;
            //     sw_temp->ir_type = IR_STORE;
            //     sw_temp->instruction = "sw";
            //     sw_temp->rs1.name = get_id_from_register(rp, holding_reg);
            //     remove_id_from_register(rp, holding_reg, sw_temp->rs1.name);
            //     sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
            //     sw_temp->rd.reg = s0;
            //     sw_temp->rs1.reg = holding_reg;
            
            //     sw_temp->next = *head;
            //     sw_temp->prev = NULL;
            //     (*head)->prev = sw_temp;
            //     //*head = sw_temp;    

            //     node->prev = NULL;
            //     node->next = sw_temp;
            //     sw_temp->prev = node;
            // }
            while(get_register_count(rp, holding_reg) != 0)
            {  
                head = store_from_register(rp, table, &head, holding_reg); 
            }
            node->next = head;
        }
        
        node->rd.reg = holding_reg; 

        return node;
    }


    if(root->left != NULL && (root->left->nodetype == ID_NODE || root->left->nodetype == POINTER_NODE))
    {
        /* Register allocation */

        // First, see if ID is already in any register
        holding_reg = find_ID(rp, root->left->name);
        if(holding_reg == 0)
        {
            holding_reg = find_empty_register_without_REGs(rp, 0, node->rs2.reg);

            if(holding_reg == 0)
            {
                /* Pick one register for spilling */
                // For now, I'll do this based on minimum count

                /* WARNING: VERY GREEDY! */
                if(root->operation == EQU_OP)
                {
                    if(root->right != NULL && root->right->nodetype != CONSTANT_NODE && root->right->nodetype != ID_NODE &&  root->right->nodetype != POINTER_NODE)
                    {
                        /* Some operation is to the right and is in head */
                        holding_reg = min_count_register_without_REGs(rp, node->rs1.reg, node->next->rd.reg); // *******
                    }
                    else
                    {
                        holding_reg = min_count_register_without_REGs(rp, node->rs1.reg, 0);
                    }
                }
                else
                {
                    holding_reg = min_count_register(rp);
                }
      
                IR_node* sw_temp;
                if(get_register_count(rp, holding_reg) != 0)
                {             
                    sw_temp = (IR_node*)malloc(sizeof(IR_node)); 	
                    sw_temp->instr_type = SW;
                    sw_temp->ir_type = IR_STORE;
                    sw_temp->instruction = "sw";
                    sw_temp->rs1.name = get_id_from_register(rp, holding_reg);
                    remove_id_from_register(rp, holding_reg, sw_temp->rs1.name);
                    sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
                    sw_temp->rd.reg = s0;
                    sw_temp->rs1.reg = holding_reg;
                
                    sw_temp->next = *head;
                    sw_temp->prev = NULL;
                    (*head)->prev = sw_temp;
                    //*head = sw_temp;    
 
                    if(root->right != NULL && (root->right->nodetype == ID_NODE))
                    {
                        /* Issue a load word instruction because operand is ID */
                        
                        IR_node *lw = (IR_node*)malloc(sizeof(IR_node)); 
                        add_id_to_register(rp, holding_reg, root->left->name);
                        
                        lw->ir_type = IR_LOAD;
                        lw->instr_type = LW;
                        lw->instruction = "lw";
                        lw->rd.reg = holding_reg;
                        //lw->rs1.name = root->left->name;    
                        node->rs1.reg = s0;
                        node->sf_offset = get_sf_offset(table, root->left->name)*(-1);   

                        sw_temp->prev = lw;
                        lw->next = sw_temp;
                        lw->prev = node;     

                        node->prev = NULL;
                        node->next = lw;
                        node->rs1.reg = holding_reg;
                        //return node;
                    }
                    else
                    {
                        sw_temp->prev = node;
                        node->prev = NULL;
                        node->next = sw_temp;
                        node->rd.reg = holding_reg;
                        //add_id_to_register(rp, holding_reg, root->left->name);
                        
                        return node;
                    }
                    
                }
                if(root->left->nodetype == ID_NODE)
                {
                    add_id_to_register(rp, holding_reg, root->left->name);
                }
                node->rs1.reg = holding_reg;

                //return node;
            }
            else
            {
                /* Found empty register, insert LW */
                if(root->operation != EQU_OP)
                {
                    IR_node* lui_insert = (IR_node*)malloc(sizeof(IR_node));
                    lui_insert->ir_type = IR_LOAD;
                    if(root->left->nodetype == POINTER_NODE)
                    {
                        lui_insert->instr_type = LD;
                        lui_insert->instruction = "ld";
                    }
                    else
                    {
                        lui_insert->instr_type = LW;
                        lui_insert->instruction = "lw";
                    }
                    lui_insert->rd.reg = holding_reg;
                    //lui_insert->rs1.name = root->left->name;
                    lui_insert->rs1.reg = s0;
                    lui_insert->sf_offset = get_sf_offset(table, root->left->name)*(-1);

                    lui_insert->prev = node;
                    lui_insert->next = *head;
                    (*head)->prev = lui_insert;
                    node->next = lui_insert;

                    node->rs1.reg = lui_insert->rd.reg;
                }
                else
                {
                    node->rd.reg = holding_reg;
                }
                if(root->left->nodetype == ID_NODE || root->left->nodetype == POINTER_NODE)
                    add_id_to_register(rp, holding_reg, root->left->name);
                //ht_set_AD_reg(table, root->left->name, holding_reg);
            }
        }
        else
        {
            if(root->operation == EQU_OP)
            {
                if(root->right != NULL && root->right->nodetype == CONSTANT_NODE)
                {
                    node->rs1.reg = holding_reg; 
                }
                else if(root->right != NULL && (root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE))
                {
                    /* Copying */
                }
                else
                {
                    node->rd.reg = holding_reg; 
                }
            }
            else
            {
                node->rs1.reg = holding_reg; 
            }
        }
    }


    if(root->right != NULL && (root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE))
    {
        /* Register allocation */

        // First, see if ID is already in any register
        holding_reg = find_ID(rp, root->right->name);

        if(holding_reg == 0)
        {
            holding_reg = find_empty_register_without_REGs(rp, node->rs1.reg, 0);
            

            if(holding_reg == 0)
            {
                /* Pick one register for spilling */
                // For now, I'll do this based on minimum count

                /* WARNING: VERY GREEDY! */
                if(root->operation == EQU_OP)
                {
                    holding_reg = min_count_register_without_REGs(rp, node->rs1.reg, node->rs2.reg);
                }
                else
                {
                    holding_reg = min_count_register_without_REGs(rp, node->rs1.reg, 0); // without left register
                }
      
                IR_node* sw_temp;
                if(get_register_count(rp, holding_reg) != 0)
                {             
                    sw_temp = (IR_node*)malloc(sizeof(IR_node)); 	
                    sw_temp->instr_type = SW;
                    sw_temp->ir_type = IR_STORE;
                    sw_temp->instruction = "sw";
                    sw_temp->rs1.name = get_id_from_register(rp, holding_reg);
                    remove_id_from_register(rp, holding_reg, sw_temp->rs1.name);
                    sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
                    sw_temp->rd.reg = s0;
                    sw_temp->rs1.reg = holding_reg;
                
                    sw_temp->next = *head;
                    sw_temp->prev = NULL;
                    (*head)->prev = sw_temp;
                    //*head = sw_temp;    

                    if(root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE)
                    {
                        /* Issue a load word instruction because operand is ID */
                        IR_node *lw = (IR_node*)malloc(sizeof(IR_node)); 
                        add_id_to_register(rp, holding_reg, root->right->name);
                        
                        lw->ir_type = IR_LOAD;
                        lw->instr_type = LW;
                        lw->instruction = "lw";
                        lw->rd.reg = holding_reg;
                        //lw->rs1.name = root->right->name;     
                        lw->rs1.reg = s0;
                        lw->sf_offset = get_sf_offset(table, root->right->name)*(-1); 

                        sw_temp->prev = lw;
                        lw->next = sw_temp;
                        lw->prev = node;     

                        node->prev = NULL;
                        node->next = lw;
                        node->rs2.reg = holding_reg;
                        return node;
                    }
                    else
                    {
                        sw_temp->prev = node;
                        node->prev = NULL;
                        node->next = sw_temp;
                        node->rd.reg = holding_reg;
                        //add_id_to_register(rp, holding_reg, root->right->name);
                        return node;
                    }
                }
                
                add_id_to_register(rp, holding_reg, root->right->name);
                node->rs2.reg = holding_reg;

                //return node;
            }
            else
            {
                /* Found empty register, insert LW */
                
                IR_node* lui_insert = (IR_node*)malloc(sizeof(IR_node));
                lui_insert->ir_type = IR_LOAD;
                lui_insert->instr_type = LW;
                lui_insert->instruction = "lw";
                lui_insert->rd.reg = holding_reg;
                //lui_insert->rs1.name = root->right->name;
                lui_insert->rs1.reg = s0;
                lui_insert->sf_offset = get_sf_offset(table, root->right->name)*(-1);

                lui_insert->prev = node;
                lui_insert->next = *head;
                (*head)->prev = lui_insert;
                node->next = lui_insert;

                node->rs2.reg = holding_reg;
                
                add_id_to_register(rp, holding_reg, root->right->name);
                //ht_set_AD_reg(table, root->left->name, holding_reg);
            }
        }
        else
        {
            node->rs2.reg = holding_reg;    

            if(root->right->nodetype == ID_NODE || root->right->nodetype == POINTER_NODE)
                add_id_to_register(rp, holding_reg, root->right->name);
        }
    }

    return node;
}


IR_node* get_reg_single_ID(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    IR_register holding_reg;
    if(root->nodetype == ID_NODE || root->nodetype == POINTER_NODE)
    {
        /* Register allocation */

        // First, see if ID is already in any register
        holding_reg = find_ID(rp, root->name);

        if(holding_reg == 0)
        {
            holding_reg = find_empty_register(rp);

            if(holding_reg == 0)
            {
                /* Pick one register for spilling */
                // For now, I'll do this based on minimum count

                /* WARNING: VERY GREEDY! */         
                holding_reg = min_count_register(rp);
      
                IR_node* sw_temp;
                if(get_register_count(rp, holding_reg) != 0)
                {             
                    sw_temp = (IR_node*)malloc(sizeof(IR_node)); 	
                    sw_temp->instr_type = SW;
                    sw_temp->ir_type = IR_STORE;
                    sw_temp->instruction = "sw";
                    sw_temp->rs1.name = get_id_from_register(rp, holding_reg);
                    remove_id_from_register(rp, holding_reg, sw_temp->rs1.name);
                    sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
                    sw_temp->rd.reg = s0;
                    sw_temp->rs1.reg = holding_reg;
                
                    sw_temp->next = *head;
                    sw_temp->prev = NULL;
                    (*head)->prev = sw_temp;
                    //*head = sw_temp;    
                }
               

                /* Issue a load word instruction because operand is ID */
                        
                IR_node *lw = (IR_node*)malloc(sizeof(IR_node)); 
                add_id_to_register(rp, holding_reg, root->name);
                
                lw->ir_type = IR_LOAD;
                if(root->nodetype == ID_NODE)
                {
                    lw->instr_type = LW;
                    lw->instruction = "lw";
                }
                else
                {
                    lw->instr_type = LD;
                    lw->instruction = "ld";
                }
                lw->rd.reg = holding_reg;
                //lw->rs1.name = root->name;   
                lw->rs1.reg = s0;
                lw->sf_offset = get_sf_offset(table, root->name)*(-1);    

                sw_temp->prev = lw;
                lw->next = sw_temp;
                lw->prev = node;     

                node->prev = NULL;
                node->next = lw;
                node->rs1.reg = holding_reg;
            }
            else
            {

                IR_node* lui_insert = (IR_node*)malloc(sizeof(IR_node));
                lui_insert->ir_type = IR_LOAD;
                if(root->nodetype == ID_NODE)
                {
                    lui_insert->instr_type = LW;
                    lui_insert->instruction = "lw";
                }
                else
                {
                    lui_insert->instr_type = LD;
                    lui_insert->instruction = "ld";
                }
                lui_insert->rd.reg = holding_reg;
                //lui_insert->rs1.name = root->name;
                lui_insert->rs1.reg = s0;
                lui_insert->sf_offset = get_sf_offset(table, root->name)*(-1); 

                lui_insert->prev = node;
                lui_insert->next = *head;
                (*head)->prev = lui_insert;
                node->next = lui_insert;

                node->rs1.reg = lui_insert->rd.reg;

                add_id_to_register(rp, holding_reg, root->name);
            }
        }
        else
        {
            node->next = (*head);
            (*head)->prev = node;
            node->rs1.reg = holding_reg; 
        }
    }

    return node;
}


IR_node* get_OP_node(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    IR_node *temp_left = (IR_node*)malloc(sizeof(IR_node));
    IR_node *temp_right = (IR_node*)malloc(sizeof(IR_node));
    if((root->left != NULL && root->left->nodetype == ID_NODE) || (root->right != NULL && root->right->nodetype == ID_NODE))
    {     
        if(root->left != NULL && root->left->nodetype == CONSTANT_NODE)
        {     
            temp_left = get_reg(rp, table, root->left, temp_left, head);
            node->rs1.reg = temp_left->rd.reg;
            node->next = temp_left;
            temp_left->prev = node;
            temp_left->next = head;
            (*head)->prev = temp_left;
            
        }
        else if(root->right != NULL && root->right->nodetype == CONSTANT_NODE)
        {
            
            temp_right = get_reg(rp, table, root->right, temp_right, head);
            node->rs2.reg = temp_right->rd.reg; 
            node->next = temp_right;
            temp_right->prev = node;
            temp_right->next = head;
            (*head)->prev = temp_right;
        }

        node = get_reg(rp, table, root, node, &(node->next));    
        node->prev = NULL;
    }
    
    if (root->left->nodetype == OPERATION_NODE)
    {
        node->rs1.reg = (*head)->rd.reg;
    }
    else if(root->right->nodetype == OPERATION_NODE)
    {
        /* Operation to the right */
        node->rs2.reg = (*head)->rd.reg;     
    }

    return node;
}


IR_node* create_BEQ_node(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    IR_node *temp_left = (IR_node*)malloc(sizeof(IR_node));

    if(root->nodetype == ID_NODE)
    {
        temp_left = get_reg_single_ID(rp, table, root, temp_left, head);  
    }
    else if(root->nodetype == CONSTANT_NODE)
    {
        IR_node *lui_node = (IR_node*)malloc(sizeof(IR_node));
        lui_node = get_reg(rp, table, root, lui_node, head);  

        lui_node->prev = temp_left;
        temp_left->next = lui_node;
        temp_left->rs1.reg = lui_node->rd.reg;
    }

    temp_left->ir_type = IR_BRANCH;
    temp_left->instr_type = BEQ;
    temp_left->instruction = "beq";
    // rs1.reg already set?
    temp_left->rs2.reg = 0;    
    return temp_left;
}

IR_node* create_BNE_node(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    IR_node *temp_left = (IR_node*)malloc(sizeof(IR_node));

    if(root->nodetype == ID_NODE)
    {
        temp_left = get_reg_single_ID(rp, table, root, temp_left, head);  
    }
    else if(root->nodetype == CONSTANT_NODE)
    {
        IR_node *lui_node = (IR_node*)malloc(sizeof(IR_node));
        lui_node = get_reg(rp, table, root, lui_node, head);  

        lui_node->prev = temp_left;
        temp_left->next = lui_node;
        temp_left->rs1.reg = lui_node->rd.reg;
    }

    temp_left->ir_type = IR_BRANCH;
    temp_left->instr_type = BNE;
    temp_left->instruction = "bne";
    // rs1.reg already set?
    temp_left->rs2.reg = 0;    
    return temp_left;
}


IR_register get_holding_reg(register_pool *rp, ht *table, IR_node **node, IR_node **head, IR_register without_reg1, IR_register without_reg2)
{
    IR_register holding_reg = find_empty_register_without_REGs(rp, without_reg1, without_reg2);

    if(holding_reg == 0)
    {
        /* Pick one register for spilling */
        // For now, I'll do this based on minimum count

        /* WARNING: VERY GREEDY! */         
        holding_reg = min_count_register_without_REGs(rp, without_reg1, without_reg2);

        // IR_node* sw_temp;
        // if(get_register_count(rp, holding_reg) != 0)
        // {             
        //     sw_temp = (IR_node*)malloc(sizeof(IR_node)); 	
        //     sw_temp->instr_type = SW;
        //     sw_temp->ir_type = IR_STORE;
        //     sw_temp->instruction = "sw";
        //     sw_temp->rs1.name = get_id_from_register(rp, holding_reg);
        //     remove_id_from_register(rp, holding_reg, sw_temp->rs1.name);
        //     sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
        //     sw_temp->rd.reg = s0;
        //     sw_temp->rs1.reg = holding_reg;
        
        //     sw_temp->next = *head;
        //     sw_temp->prev = NULL;
        //     (*head)->prev = sw_temp;
        //     //*head = sw_temp;    

        //     (*node)->prev = NULL;
        //     (*node)->next = sw_temp;
        //     sw_temp->prev = (*node);
        // }
        while(get_register_count(rp, holding_reg) != 0)
        {  
            head = store_from_register(rp, table, &head, holding_reg); 
        }
        (*node)->next = head;

        return holding_reg;
    }
    else
    {
        return holding_reg;
    }
}



IR_node* clean_up(register_pool *rp, IR_node* head, int line, int sp_offset)
{
    /* By C convention, main function will always return int and will always return 0 on clean exit */
    // IR_node *load_zero = (IR_node*)malloc(sizeof(IR_node));

    // load_zero->ir_type = IR_OP_IMM;
    // load_zero->instr_type = ADDI;
    // load_zero->instruction = "addi";
    // load_zero->rd.reg = a0;
    // load_zero->rs1.reg = 0;
    // load_zero->rs2.int_constant = 0;
    // load_zero->next = head;
    // head->prev = load_zero;
    remove_register_allocation_ALL(rp);

    IR_node *load_ra = (IR_node*)malloc(sizeof(IR_node));

    load_ra->ir_type = IR_LOAD;
    load_ra->instr_type = LD;
    load_ra->instruction = "ld";
    load_ra->rd.reg = ra;
    load_ra->sf_offset = sp_offset - 8;
    load_ra->rs1.reg = sp; 
    //load_ra->next = load_zero;
    load_ra->next = head;
    //load_zero->prev = load_ra;
    head->prev = load_ra;

    IR_node *load_s0 = (IR_node*)malloc(sizeof(IR_node));

    load_s0->ir_type = IR_LOAD;
    load_s0->instr_type = LD;
    load_s0->instruction = "ld";
    load_s0->rd.reg = s0;
    load_s0->sf_offset = sp_offset - 16;
    load_s0->rs1.reg = sp; 
    load_s0->next = load_ra;
    load_ra->prev = load_s0;

    IR_node *reset_sp = (IR_node*)malloc(sizeof(IR_node));

    reset_sp->ir_type = IR_OP_IMM;
    reset_sp->instr_type = ADDI;
    reset_sp->instruction = "addi";
    reset_sp->rd.reg = sp;
    reset_sp->rs1.reg = sp; 
    reset_sp->rs2.int_constant = sp_offset;
    reset_sp->next = load_s0;
    load_s0->prev = reset_sp;

    IR_node *jump_ra = (IR_node*)malloc(sizeof(IR_node));

    jump_ra->ir_type = IR_JUMP;
    jump_ra->instr_type = JALR;
    jump_ra->instruction = "jalr";
    jump_ra->rd.reg = 0;
    jump_ra->rs1.reg = ra; 

    jump_ra->next = reset_sp;
    reset_sp->prev = jump_ra;
    jump_ra->prev = NULL;

    jump_ra->line = line;
    update_line_number_IR(&jump_ra);

    return jump_ra;
}

int get_label_address(char* label, IR_node *IR_head, IR_node *IR_tail)
{
    while(IR_head != IR_tail)
    {
        IR_head = IR_head->prev;

        if(IR_head->ir_type == IR_LABEL)
        {
            if(strcmp(IR_head->instruction, label) == 0)
            {
                return IR_head->ir_address;
            }
        }
    }

    return -1;
}

void update_line_number_IR(IR_node** node)
{   
    int line = (*node)->line;
    IR_node *temp = *node;
    while(temp->next != HEAD && temp->next->line == 0)
    {  
        temp = temp->next;
        temp->line = line;
    }
}

void load_imm(ASTnode* root, IR_node** node, IR_node** head)
{
    /* Check if IMM is larger than 12 bits (2^12-1) = 4096 */
    if(root->value.value_INT < 4096)
    {
        (*node)->ir_type = IR_OP_IMM;
        (*node)->instr_type = ADDI;
        (*node)->instruction = "addi";
        (*node)->rs1.reg = 0;
        (*node)->rs2.int_constant = root->value.value_INT;
    }
    else
    {
        /* Break LI into LUI and ADDI */

        IR_node *lui_node = (IR_node*)malloc(sizeof(IR_node));

        lui_node->ir_type = IR_LOAD_IMM;
        lui_node->instr_type = LUI;
        lui_node->instruction = "lui";
        lui_node->rd.reg = (*node)->rd.reg;
        lui_node->rs1.int_constant = root->value.value_INT >> 12;

        (*head)->prev = lui_node;
        lui_node->next = (*head); 
        lui_node->prev = (*node);
        (*node)->next = lui_node;

        (*node)->ir_type = IR_OP_IMM;
        (*node)->instr_type = ADDI;
        (*node)->instruction = "addi";
        (*node)->rs1.reg = 0;
        (*node)->rs2.int_constant = root->value.value_INT & 0xfff; 
    }   
}

IR_node* store_pointer(register_pool *rp, ht *table , IR_node **node, ASTnode *pointer_node)
{
    IR_node *store_node = (IR_node*)malloc(sizeof(IR_node));

    store_node->ir_type = IR_STORE;
    store_node->instr_type = SD;
    store_node->instruction = "sd";

    if(pointer_node->structure == ARRAY)
    {
        IR_node *load_node = (IR_node*)malloc(sizeof(IR_node));

        load_node = get_reg_single_ID(rp, table, pointer_node, load_node, node);
        
        store_node->next = load_node;
        load_node->prev = store_node;

        store_node->rs1.reg = (*node)->rd.reg;
        store_node->sf_offset = pointer_node->element_number * 4 * (-1); // times 8 because thats SIZE_POINTER
        store_node->rd.reg = s0; 
    }
    else
    {
        store_node->next = (*node);
        (*node)->prev = store_node;
    
        store_node->rs1.reg = (*node)->rd.reg;
        store_node->sf_offset = pointer_node->element_number * 8; // times 8 because thats SIZE_POINTER
        store_node->rd.reg = (*node)->rd.reg; /* EQU OP on pointers by default */
    }

    remove_id_from_register(rp, store_node->rs1.reg, pointer_node->name);

    store_node->prev = NULL;
    

    return store_node;
}


void load_array_element(register_pool *rp, ht *table, ASTnode *root, IR_node **node, IR_node **head)
{
    /* This function in called from ARRAY_ELEMENT_NODE */
    /* It is required that root->left is expression that calculates index that is being accessed, 
     * and that head already contains instruction for such calculation (load, or addi in simple terms).
     * Firstly it is required to cacluclate relative offset from arrays base in bytes, to do this we need to 
     * multiply expression's result by 4 - shift left for 2 (SLLI 2)
     * Later we simply calculate abolute address of element based on array's base address and relative offset in bytes
    */

    IR_node *slli_insert = (IR_node*)malloc(sizeof(IR_node));
    slli_insert->ir_type = IR_OP_IMM;
    slli_insert->instr_type = SLLI;
    slli_insert->instruction = "slli";
    
    if(root->right->left->nodetype == CONSTANT_NODE && root->right->right == NULL)
    {
        IR_node* temp = (IR_node*)malloc(sizeof(IR_node));
        temp = get_reg(rp, table, root->right->left, temp, head);

        slli_insert->rd.reg = temp->rd.reg;
        slli_insert->rs1.reg = temp->rd.reg;
        slli_insert->rs2.int_constant = 2;

        (*head)->prev = temp;
        temp->next = (*head);
        temp->prev = slli_insert;
        slli_insert->next = temp;
    }
    else if(root->right->left->nodetype == ID_NODE && root->right->right == NULL)
    {
        IR_node* temp = (IR_node*)malloc(sizeof(IR_node));
        temp = get_reg_single_ID(rp, table, root->right->left, temp, head);
        
        remove_id_from_register(rp, temp->rs1.reg, root->right->left->name);

        slli_insert->rd.reg = temp->rs1.reg;
        slli_insert->rs1.reg = temp->rs1.reg;
        slli_insert->rs2.int_constant = 2;

        (*head)->prev = temp;
        temp->next = (*head);
        temp->prev = slli_insert;
        slli_insert->next = temp;
    }
    else
    {
        slli_insert->rd.reg = (*head)->rd.reg;
        slli_insert->rs1.reg = (*head)->rd.reg;
        slli_insert->rs2.int_constant = 2;
        slli_insert->next = (*head);
        (*head)->prev = slli_insert;
    }

    /* Now calculate absolute address of array's base element, by adding it's sp_offset to s0 */

    IR_node *addi_insert = (IR_node*)malloc(sizeof(IR_node));
    addi_insert->ir_type = IR_OP_IMM;
    addi_insert->instr_type = ADDI;
    addi_insert->instruction = "addi";
    addi_insert->rd.reg = get_holding_reg(rp, table, &addi_insert, &slli_insert, slli_insert->rd.reg, 0);
    addi_insert->rs1.reg = s0;
    addi_insert->rs2.int_constant = get_sf_offset(table, root->left->name) * (-1);
    addi_insert->next = slli_insert;
    slli_insert->prev = addi_insert;

    /* Simply add thos two togehter */

    IR_node *calc_address = (IR_node*)malloc(sizeof(IR_node));
    calc_address->ir_type = IR_OP;
    calc_address->instr_type = ADD;
    calc_address->instruction = "add";
    calc_address->rd.reg = get_holding_reg(rp, table, &calc_address, &addi_insert, slli_insert->rd.reg, addi_insert->rd.reg);
    calc_address->rs1.reg = slli_insert->rd.reg;
    calc_address->rs2.reg = addi_insert->rd.reg;
    calc_address->next = addi_insert;
    addi_insert->prev = calc_address;

    IR_node *srli_node = (IR_node*)malloc(sizeof(IR_node));
    srli_node->ir_type = IR_OP_IMM;
    srli_node->instr_type = SRLI;
    srli_node->instruction = "srli";
    srli_node->rd.reg = slli_insert->rd.reg;
    srli_node->rs1.reg = slli_insert->rd.reg;
    srli_node->rs2.int_constant = 2;
    srli_node->next = calc_address;
    calc_address->prev = srli_node;

    (*node) = srli_node;
}


IR_node* store_array_element(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node *head)
{
    IR_node *store_node = (IR_node*)malloc(sizeof(IR_node));

    store_node->ir_type = IR_STORE;
    store_node->instr_type = SW;
    store_node->instruction = "sw";
    store_node->rd.reg = head->next->rd.reg;
    store_node->sf_offset = 0;
    store_node->rs1.reg = node->rd.reg;

    return store_node;
}


IR_node* store_everything(register_pool *rp, ht *table, IR_node **head)
{
    IR_node* temp = head;

    for(int i = t1; i <= t2; i++)
    {
        while(get_register_count(rp, i) != 0)
        {  
            temp = store_from_register(rp, table, temp, i); 
        }
    }

    for(int i = t3; i <= t6; i++)
    {
        while(get_register_count(rp, i) != 0)
        {  
            temp = store_from_register(rp, table, temp, i); 
        }
    }

    return temp;
}

IR_node* store_from_register(register_pool *rp, ht *table, IR_node **head, IR_register reg)
{
    IR_node* sw_temp = (IR_node*)malloc(sizeof(IR_node));
    if(get_register_count(rp, reg) != 0)
    {             
        sw_temp->instr_type = SW;
        sw_temp->ir_type = IR_STORE;
        sw_temp->instruction = "sw";
        sw_temp->rs1.name = get_id_from_register(rp, reg);
        remove_id_from_register(rp, reg, sw_temp->rs1.name);
        sw_temp->sf_offset = get_sf_offset(table, sw_temp->rs1.name)*(-1);
        sw_temp->rd.reg = s0;
        sw_temp->rs1.reg = reg;
    
        sw_temp->next = *head;
        sw_temp->prev = NULL;
        (*head)->prev = sw_temp;
    }

    return sw_temp;
}

void store_19(register_pool *rp, ht *table)
{
    while(find_ID(rp, "19"))
    {
        remove_id_from_register(rp, find_ID(rp, "19"), "19");
    }

}

void store_argument(register_pool *rp, ht *table, IR_node **node, IR_node **head, IR_register holding_reg)
{
    /* Find empty a0-a7 reg */
    IR_register empty_reg = find_empty_argument_register(rp);

    if(empty_reg == 0)
    {
        /* Deal with it later */
    }
    else
    {
        /* Store contents of holding_reg into empty_reg */
        (*node)->ir_type = IR_OP_IMM;
        (*node)->instr_type = ADDI;
        (*node)->instruction = "addi";
        (*node)->rd.reg = empty_reg;
        (*node)->rs1.reg = holding_reg;
        (*node)->rs2.int_constant = 0;

        add_id_to_register(rp, empty_reg, "19");
    }
}

IR_node* load_parameter(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    /* Load argument from argument reg into allocated register for said ID */
    node = get_reg(rp, table, root, node, &head);
    node->ir_type = IR_OP_IMM;
    node->instr_type = ADDI;
    node->instruction = "addi";
    node->rd.reg = node->rs1.reg;
    node->rs1.reg = argument_register_usage++;
    node->rs2.int_constant = 0;
    
    return node;
}