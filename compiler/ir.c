#include "ir.h"

/*
* Function that walks the AST and creates IR tree based on it
*/

int register_counter = 1;
int label_counter = 1;
extern int sp_offset;

IR_node* populate_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack, register_pool *rp, ht* table)
{
    IR_node *this_node = NULL;

    if(root->nodetype == FUNCTION_NODE)
    { 
        this_node = insert_IR(root, head, stack, secondary_stack, rp, table);

        if(root->right != NULL) 
        {
            this_node = populate_IR(root->right, this_node, stack, secondary_stack, rp, table);
        }
    }
    else
    {
        if(root->right != NULL) 
        {
            this_node = populate_IR(root->right, head, stack, secondary_stack, rp, table);
        }
    }



    /* Prio execution */

    if(root->nodetype == IF_NODE || root->nodetype == ELSE_NODE)
    { 
        this_node = insert_IR(root, this_node, stack, secondary_stack, rp, table);
    }

    if(root->right != NULL)
    {
        if((root->nodetype != ELSE_NODE && root->right->nodetype == IF_NODE) ||root->right->nodetype == ELSE_NODE || root->right->nodetype == WHILE_NODE || root->right->nodetype == FOR_NODE)
        {    
            this_node = insert_IR(root, this_node, stack, secondary_stack, rp, table);
        }
    }


    /* Normal execution */
    if(root->left != NULL && root->nodetype != FUNCTION_NODE) // watch out when added parameters to non-main function
    {
        if(this_node == NULL)
            this_node = populate_IR(root->left, head, stack, secondary_stack, rp, table);
        else
            this_node = populate_IR(root->left, this_node, stack, secondary_stack, rp, table);
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
            else
                help->rd.label = ls;
        }

        return this_node;
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
        return insert_IR(root, head, stack, secondary_stack, rp, table);
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

IR_node* insert_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack, register_pool *rp, ht* table)
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

        case OPERATION_NODE:
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
                                node->rs1.name = root->right->name;
                            }
                            else
                            {
                                while(get_register_count(rp, operand2_reg) != 0)
                                {
                                    IR_node *sw_temp = (IR_node*)malloc(sizeof(IR_node));
                                    sw_temp->ir_type = IR_STORE;
                                    sw_temp->instr_type = SW;
                                    sw_temp->instruction = "sw";
                                    sw_temp->rd.name = get_id_from_register(rp, operand2_reg);
                                    sw_temp->rs1.reg = operand2_reg;
                                    sw_temp->prev = NULL;
                                    sw_temp->next = head;
                                    head->prev = sw_temp;

                                    head = sw_temp;
                                    remove_id_from_register(rp, operand2_reg, sw_temp->rd.name);
                                }
                                
                                node->next = head;
                                head->prev = node;
                                add_id_to_register(rp, operand2_reg, root->left->name);
                            }
                            return node;
                        }

                        if(root->right->nodetype == CONSTANT_NODE)
                        {            
                            temp = get_reg(rp, table, root, node, &head);
                            
                            temp->ir_type = IR_LOAD_IMM;
                            temp->instr_type = LUI;
                            temp->instruction = "lui";
                            //temp->rd.reg = temp->rs1.reg;
                            temp->rs1.int_constant = root->right->value.value_INT;                
                            return temp;
                        }

                        if(root->left->nodetype == CONSTANT_NODE)
                        {
                            temp = get_reg(rp, table, root, node, &head);
                            temp->ir_type = IR_LOAD_IMM;
                            temp->instr_type = LUI;
                            temp->instruction = "lui";
                            //temp->rd.reg = temp->rs1.reg;
                            temp->rs1.int_constant = root->left->value.value_INT;
                            return temp;
                        }

                        if(root->left == root->right->left)
                        {
                            head->prev = NULL;
                            head->rd.reg = head->rs1.reg;
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
                            return head;
                        }
                        
                	    /* Assign new register to hold resulting value of past OPERATION */
                        
                	    temp = get_reg(rp, table, root, head, &(head->next));    

		                return temp;
                    }
                    else
                    {
                        /* There is an operation to the right so just edit it's rd register */
                        if(root->right->nodetype != CONSTANT_NODE && root->right->nodetype != ID_NODE)
                        {
                            head->prev = NULL;
                            head->rd.reg = holding_reg;
                            return head;
                        }
                        else if(root->right->nodetype == ID_NODE)
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
                                node->rs1.name = root->right->name;
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
                                    sw_temp->rd.name = get_id_from_register(rp, operand2_reg);
                                    sw_temp->rs1.reg = operand2_reg;
                                    sw_temp->prev = NULL;
                                    sw_temp->next = head;
                                    head->prev = sw_temp;

                                    head = sw_temp;
                                    remove_id_from_register(rp, operand2_reg, sw_temp->rd.name);
                                }

                                add_id_to_register(rp, operand2_reg, root->left->name);
                            }
                        }
                        else 
                        {
                            node->ir_type = IR_LOAD_IMM;
                            node->instr_type = LUI;
                            node->instruction = "lui";
                            node->rd.reg = holding_reg;
                            node->rs1.int_constant = root->right->value.value_INT;
                        }
                    }
                break;

                case ADD_OP:

                    node = get_reg(rp, table, root, node, &head);
                    
                    if(root->right->nodetype == CONSTANT_NODE)
                    {
                        node->ir_type = IR_OP_IMM;
                        node->instr_type = ADDI;
                        node->instruction = "addi";

                        node->rs2.int_constant = root->right->value.value_INT; // INT only

                        // Delete load const IR_node, two nodes ago
                        //node->next->next->next->prev = node->next;
                        //node->next->next = node->next->next->next->next;
                        
                    }
                    else if(root->left->nodetype == CONSTANT_NODE)
                    {
                        node->ir_type = IR_OP_IMM;
                        node->instr_type = ADDI;
                        node->instruction = "addi";

                        if(root->right->nodetype == ID_NODE)
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
                break;

                case SUB_OP:
                    node->instr_type = SUB;
                    node->ir_type = IR_OP;
                    node->instruction = "sub";
                    node->rd.reg = node->reg;
                    node->rs1.reg = node->next->rd.reg;
                    node->rs2.reg = (node->next)->next->rd.reg;
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
                    node->instr_type = BLE;
                    node->ir_type = IR_BRANCH;
                    node->instruction = "bge";
                    push(stack, node); 
                break;

                case LOGIC_LET_OP:
                    node = get_OP_node(rp, table, root, node, &head);
                    
                /*
                * BGT - branch if greater than => is BLT when operand are reversed
                */
                    node->instr_type = BGT;
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
                    node->rs2.int_constant = 1;

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

                    return nxt;
                break;

                case LOGIC_AND_OP:
                    IR_node *id_left = (IR_node*)malloc(sizeof(IR_node));
                    IR_node *id_right = (IR_node*)malloc(sizeof(IR_node));

                    if(root->right->nodetype == ID_NODE)
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
                    
                    return id_left;
                    break;


                case LOGIC_OR_OP:
                    
                    id_left = (IR_node*)malloc(sizeof(IR_node));
                    id_right = (IR_node*)malloc(sizeof(IR_node));

                    if(root->right->nodetype == ID_NODE)
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
                    
                    return id_left;
                break;

                case BITWISE_NOT_OP:
                    /* Pseudo NOT by ISA docs */
                    node->instr_type = XORI;
                    node->ir_type = IR_OP_IMM;
                    node->instruction = "xori";
                    node->rd.reg = node->next->reg;
                    node->rs1.reg = node->next->reg;
                    node->rs2.int_constant = -1;
                break;
            }

            /* Implement other operations */    
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
                else
                    help->rd.label = tmp;
            }
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
            else
                help->rd.label = tmp;

            ASTnode *tail = root->right->right;
            tail->right = NULL;
            int num_AND = count_subtree_AND_OPs(tail);

            for(int i = 0; i < num_AND; i++)
            {
                help = pop(stack);
                if(help->ir_type == IR_JUMP)
                    help->rs1.label = tmp;
                else
                    help->rd.label = tmp;
            }
            
            push(stack, jmp);
        break;

        case EXPRESSION_NODE:
            if(root->right != NULL)
            {
                char *tmp = malloc(5 * sizeof(char));
                if(root->right->nodetype == IF_NODE)
                {
                    // //node->instr_type = LABEL;
                    // root->value.label_count = root->right->value.label_count;
                    // //root->value.label_count = label_counter++;
                    
                    // sprintf(tmp, "L%d", root->value.label_count);
                    // node->instruction = tmp;                    

                    // IR_node *help = pop(stack);
                    // if(help->ir_type == IR_JUMP)
                    //     help->rs1.label = tmp;
                    // else
                    //     help->rd.label = tmp;

                    // ASTnode *tail = root->right->right;
                    // tail->right = NULL;

                    node->instr_type = LABEL;
                    node->ir_type = IR_LABEL;
                    root->value.label_count = label_counter++;
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;                    

                    IR_node *help = pop(stack);
                    if(help->ir_type == IR_JUMP)
                        help->rs1.label = tmp;
                    else
                        help->rd.label = tmp;

                    /* 
                    * When there are multiple subexpressions or operands in logical expressions, for each branching
                    * there will be and instruction node created and it will be pushed to stack, so I make a temporary sub-tree
                    * and cut sub-tree from the rest of AST by inserting NULL pointer to its tail, so I traverse only subtree from current above-IF node
                    * down to boolean expression of if node and count how many times I need to pop nodes off the stack 
                    */

                    ASTnode *tail = root->right->right;
                    tail->right = NULL;
                    int num_AND = count_subtree_AND_OPs(tail);

                    for(int i = 0; i < num_AND; i++)
                    {
                        help = pop(stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else
                            help->rd.label = tmp;
                    }

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
                    else
                        help->rd.label = tmp;
                        
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
                    else
                        help->rd.label = tmp;

                    int num_AND = count_subtree_AND_OPs(tail);

                    for(int i = 0; i < num_AND; i++)
                    {
                        help = pop(stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else
                            help->rd.label = tmp;
                    }
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
                    else
                        help->rd.label = tmp;

                    ASTnode *tail = root->right->right;
                    tail->right = NULL;

                    int num_AND = count_subtree_AND_OPs(tail);

                    for(int i = 0; i < num_AND; i++)
                    {
                        help = pop(stack);
                        if(help->ir_type == IR_JUMP)
                            help->rs1.label = tmp;
                        else
                            help->rd.label = tmp;
                    }
                }
            }
        break;

        case SCOPE_NODE:
            if(root->left != NULL)
            {
                if(root->left->nodetype != EXPRESSION_NODE) // when something other than exp is to the left of scope
                {
                    node->instr_type = LABEL; 
                    node->ir_type = IR_LABEL;
                    char *tmp = malloc(5 * sizeof(char));
                    root->value.label_count = label_counter++;
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;     
                }
                else
                {
                    root->value.label_count = root->left->value.label_count;
                }
            }      
        break;

        case FUNCTION_NODE:
            node->instr_type = LABEL;
            node->ir_type = IR_LABEL;
            node->instruction = root->left->name;
            
            /* Stack frame allocation time */

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
                else
                    help->rd.label = tmp;
            }
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
    FILE *asm_file = fopen("../output.s", "w");

    while(IR_head != IR_tail)
    {
      IR_head = IR_head->prev;
      switch(IR_head->ir_type)
      {
        case IR_LABEL:
          printf(".%s:\n", IR_head->instruction);
          fprintf(asm_file, ".%s:\n", IR_head->instruction);
        break;

        case IR_OP_IMM:
          printf("\t%s x%d,x%d,%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.int_constant);
          fprintf(asm_file, "\t%s x%d,x%d,%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.int_constant);
        break;

        case IR_STORE:
          printf("\t%s %s,x%d\n", IR_head->instruction, IR_head->rd.name, IR_head->rs1.reg);
          fprintf(asm_file, "\t%s %s,x%d\n", IR_head->instruction, IR_head->rd.name, IR_head->rs1.reg);
        break;

        case IR_LOAD:
          printf("\t%s x%d,%s\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.name);
          fprintf(asm_file, "\t%s x%d,%s\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.name);
        break;

        case IR_LOAD_IMM:
          printf("\t%s x%d,%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.int_constant);
          fprintf(asm_file, "\t%s x%d,%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.int_constant);
        break;        

        case IR_OP:
          printf("\t%s x%d,x%d,x%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
          fprintf(asm_file, "\t%s x%d,x%d,x%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
        break;

        case IR_BRANCH:
          printf("\t%s x%d,x%d,.%s\n", IR_head->instruction, IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
          fprintf(asm_file, "\t%s x%d,x%d,.%s\n", IR_head->instruction, IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
        break;

        case IR_JUMP:
          printf("\t%s x%d,.%s\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.label);
          fprintf(asm_file, "\t%s x%d,.%s\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.label);
        break;

        case NONE:
        break;
      }
    }


    fclose(asm_file);
}


IR_node* get_reg(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    IR_register holding_reg;

    if(root->nodetype == CONSTANT_NODE)
    {
        node->ir_type = IR_LOAD_IMM;
        node->instr_type = LUI;
        node->instruction = "lui";
        node->rs1.int_constant = root->value.value_INT;

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
                sw_temp->rd.name = get_id_from_register(rp, holding_reg);
                remove_id_from_register(rp, holding_reg, sw_temp->rd.name);
                sw_temp->rs1.reg = holding_reg;
            
                sw_temp->next = *head;
                sw_temp->prev = NULL;
                (*head)->prev = sw_temp;
                //*head = sw_temp;    

                node->prev = NULL;
                node->next = sw_temp;
                sw_temp->prev = node;
            }
        }
        
        node->rd.reg = holding_reg; 

        return node;
    }


    if(root->left != NULL && root->left->nodetype == ID_NODE)
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
                    if(root->right != NULL && root->right->nodetype != CONSTANT_NODE && root->right->nodetype != ID_NODE)
                    {
                        /* Some operation is to the right and is in head */
                        holding_reg = min_count_register_without_REGs(rp, node->rs1.reg, node->rs2.reg);
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
                    sw_temp->rd.name = get_id_from_register(rp, holding_reg);
                    remove_id_from_register(rp, holding_reg, sw_temp->rd.name);
                    sw_temp->rs1.reg = holding_reg;
                
                    sw_temp->next = *head;
                    sw_temp->prev = NULL;
                    (*head)->prev = sw_temp;
                    //*head = sw_temp;    
 
                    if(root->right != NULL && root->right->nodetype == ID_NODE)
                    {
                        /* Issue a load word instruction because operand is ID */
                        
                        IR_node *lw = (IR_node*)malloc(sizeof(IR_node)); 
                        add_id_to_register(rp, holding_reg, root->left->name);
                        
                        lw->ir_type = IR_LOAD;
                        lw->instr_type = LW;
                        lw->instruction = "lw";
                        lw->rd.reg = holding_reg;
                        lw->rs1.name = root->left->name;       

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
                        add_id_to_register(rp, holding_reg, root->left->name);
                        
                        return node;
                    }
                    
                }
                
                add_id_to_register(rp, holding_reg, root->left->name);
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
                    lui_insert->instr_type = LW;
                    lui_insert->instruction = "lw";
                    lui_insert->rd.reg = holding_reg;
                    lui_insert->rs1.name = root->left->name;
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
                add_id_to_register(rp, holding_reg, root->left->name);
                //ht_set_AD_reg(table, root->left->name, holding_reg);
            }
        }
        else
        {
            if(root->operation == EQU_OP)
            {
                if(root->right->nodetype == CONSTANT_NODE && root->right != NULL)
                {
                    node->rs1.reg = holding_reg; 
                }
                else if(root->right->nodetype == ID_NODE && root->right != NULL)
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


    if(root->right != NULL && root->right->nodetype == ID_NODE)
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
                    sw_temp->rd.name = get_id_from_register(rp, holding_reg);
                    remove_id_from_register(rp, holding_reg, sw_temp->rd.name);
                    sw_temp->rs1.reg = holding_reg;
                
                    sw_temp->next = *head;
                    sw_temp->prev = NULL;
                    (*head)->prev = sw_temp;
                    //*head = sw_temp;    

                    if(root->right->nodetype == ID_NODE)
                    {
                        /* Issue a load word instruction because operand is ID */
                        IR_node *lw = (IR_node*)malloc(sizeof(IR_node)); 
                        add_id_to_register(rp, holding_reg, root->right->name);
                        
                        lw->ir_type = IR_LOAD;
                        lw->instr_type = LW;
                        lw->instruction = "lw";
                        lw->rd.reg = holding_reg;
                        lw->rs1.name = root->right->name;       

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
                        add_id_to_register(rp, holding_reg, root->right->name);
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
                lui_insert->rs1.name = root->right->name;
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
        }
    }



    return node;
}


IR_node* get_reg_single_ID(register_pool *rp, ht *table, ASTnode *root, IR_node *node, IR_node **head)
{
    IR_register holding_reg;
    if(root->nodetype == ID_NODE)
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
                    sw_temp->rd.name = get_id_from_register(rp, holding_reg);
                    remove_id_from_register(rp, holding_reg, sw_temp->rd.name);
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
                lw->instr_type = LW;
                lw->instruction = "lw";
                lw->rd.reg = holding_reg;
                lw->rs1.name = root->name;       

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
                lui_insert->instr_type = LW;
                lui_insert->instruction = "lw";
                lui_insert->rd.reg = holding_reg;
                lui_insert->rs1.name = root->name;
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
    if((root->left->nodetype == ID_NODE && root->left != NULL) || (root->right->nodetype == ID_NODE && root->right != NULL))
    {     
        if(root->left->nodetype == CONSTANT_NODE && root->left != NULL)
        {
            node->next = temp_left;
            temp_left->prev = node;
            temp_left->next = head;
            (*head)->prev = temp_left;
            temp_left = get_reg(rp, table, root->left, temp_left, head);
            node->rs1.reg = temp_left->rd.reg;
        }
        else if(root->right->nodetype == CONSTANT_NODE && root->right != NULL)
        {
            node->next = temp_right;
            temp_right->prev = node;
            temp_right->next = head;
            (*head)->prev = temp_right;
            temp_right = get_reg(rp, table, root->right, temp_right, head);
            node->rs2.reg = temp_right->rd.reg;
        }

        node = get_reg(rp, table, root, node, &(node->next));    
        node->prev = NULL;
    }
    else
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