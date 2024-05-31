#include "ir.h"

/*
* Function that walks the AST and creates IR tree based on it
*/

int register_counter = 1;
int label_counter = 1;

IR_node* populate_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack)
{
    IR_node *this_node = NULL;

    if(root->right != NULL) 
    {
        this_node = populate_IR(root->right, head, stack, secondary_stack);
    }



    if(root->nodetype == IF_NODE || root->nodetype == ELSE_NODE)
    { 
        this_node = insert_IR(root, this_node, stack, secondary_stack);
    }

    if(root->right != NULL)
    {
        if(root->right->nodetype == ELSE_NODE)
        {    
            this_node = insert_IR(root, this_node, stack, secondary_stack);
        }
    }

    if(root->left != NULL) 
    {
        if(this_node == NULL)
            this_node = populate_IR(root->left, head, stack, secondary_stack);
        else
            this_node = populate_IR(root->left, this_node, stack, secondary_stack);
    }

    if(this_node != NULL)
        head = this_node; 

    if(root->nodetype == ELSE_NODE || (root->right != NULL && root->right->nodetype == ELSE_NODE))
        return this_node;
    else
        return insert_IR(root, head, stack, secondary_stack);
}

/*
* Create head
*/

IR_node* create_IR()
{
    IR_node *IR_head = (IR_node *)malloc(sizeof(IR_node));
    IR_head->ir_type = HEAD;
    IR_head->next = NULL;
    IR_head->prev = NULL;
    IR_head->reg = 0;
    return IR_head;
}

/*
* Create base IR node
*/

IR_node* insert_IR(ASTnode *root, IR_node *head, Stack *stack, Stack *secondary_stack)
{ 
    IR_node *node = (IR_node *)malloc(sizeof(IR_node));
    
    node->prev = NULL;
    node->next = head;
    head->prev = node;
    
    //node->instruction = (char*)malloc(3*sizeof(char));
    node->reg = register_counter++;
    node->ir_type = IR_NO_TYPE;

    char *tmp = malloc(5 * sizeof(char));
    IR_node *jmp = (IR_node *)malloc(sizeof(IR_node));

    switch(root->nodetype)
    {
        case CONSTANT_NODE:
            node->ir_type = LUI;
            node->instruction = "lui";
            node->rd.reg = node->reg;

            switch(root->type)
            {
                case TYPE_INT:
                    node->rs1.int_constant = root->value.value_INT;
                break;

                case TYPE_FLOAT:
                    /*
                    * NOT SUPPORTING FLOATING POINT SO NEEDS TO BE SEEN HOW IT IS IMPLEMENTED
                    */
                    node->rs1.float_constant = root->value.value_FLOAT;
                break;

                case TYPE_CHAR:
                    node->rs1.char_constant = root->value.value_CHAR;
                break;
            }

        break;

        case ID_NODE:
            node->ir_type = LW;
            node->instruction = "lw";
            node->rs1.name = root->name;
            node->rd.reg = node->reg;      

        break;

        case OPERATION_NODE:
            switch(root->operation)
            {
                case EQU_OP:
                    node->ir_type = SW;
                    node->instruction = "sw";
                    node->rd.name = node->next->rs1.name;
                    
                    // Delete LW node created by visiting right ID node from list, it is not needed
                    node->next = (node->next)->next;
                    (node->next)->prev = node;

                    node->rs1.reg = node->next->rd.reg;
                break;

                case ADD_OP:
                    node->ir_type = ADD;
                    node->instruction = "add";
                    node->rd.reg = node->reg;
                    node->rs1.reg = node->next->rd.reg;
                    node->rs2.reg = (node->next)->next->rd.reg;
                break;

                case SUB_OP:
                    node->ir_type = SUB;
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
                    node->ir_type = BNE;
                    node->instruction = "bne";
                    node->rs1.reg = node->next->reg;
                    node->rs2.reg = node->next->next->reg;    
                    push(stack, node);       
                break;

                case LOGIC_GET_OP:
                    node->ir_type = BLT;
                    node->instruction = "blt";
                    node->rs1.reg = node->next->next->reg; 
                    node->rs2.reg = node->next->reg; 
                    push(stack, node); 
                break;

                case LOGIC_GT_OP:
                /*
                * BLE - branch if less or equal => is BGE when operand are reversed
                */
                    node->ir_type = BLE;
                    node->instruction = "bge";
                    node->rs2.reg = node->next->next->reg; 
                    node->rs1.reg = node->next->reg; 
                    push(stack, node); 
                break;

                case LOGIC_LET_OP:
                /*
                * BGT - branch if greater than => is BLT when operand are reversed
                */
                    node->ir_type = BGT;
                    node->instruction = "blt";
                    node->rs2.reg = node->next->next->reg; 
                    node->rs1.reg = node->next->reg; 
                    push(stack, node); 
                break;

                case LOGIC_LT_OP:
                    // godbolt does optimization by reducing constant operand by one... and using diff instr
                    node->ir_type = BGE;
                    node->instruction = "bge";
                    node->rs1.reg = node->next->next->reg; 
                    node->rs2.reg = node->next->reg; 
                    push(stack, node);  
                break;

                case LOGIC_NEQU_OP:
                    node->ir_type = BEQ;
                    node->instruction = "beq";
                    node->rs1.reg = node->next->next->reg; 
                    node->rs2.reg = node->next->reg; 
                    push(stack, node); 
                break;

                case LOGIC_NOT_OP:
                /* Not yet supporeted by AST? */
                    // node->ir_type = BNE;
                    // node->instruction = "bne";
                    // node->rs1.reg = node->next->next->reg; 
                    // node->rs2.reg = node->next->reg; 
                    // push(stack, node); 
                break;

                case LOGIC_AND_OP:
                    if(root->right->nodetype != OPERATION_NODE)
                    {
                        IR_node *temp_node = (IR_node *)malloc(sizeof(IR_node));

                        temp_node->next = head->next;
                        head->next->prev = temp_node;       
                        head->next = temp_node;
                        temp_node->prev = head;
                        
                        temp_node->reg = register_counter++;
                        temp_node->ir_type = BEQ;
                        temp_node->instruction = "beq";
                        temp_node->rs1.reg = temp_node->next->reg; 
                        temp_node->rs2.reg = 0; 

                        push(stack, temp_node);
                    }

                    node->next = head;
                    node->prev = NULL;
                    head->prev = node;

                    node->ir_type = BEQ;
                    node->instruction = "beq";
                    node->rs1.reg = node->next->reg; 
                    node->rs2.reg = 0; 

                    push(stack, node); 
                    break;


                case LOGIC_OR_OP:
                    /* Checking if this is the root of logical sub-tree (boolean exp for if is deeper than 2 operands) */
                    if(root->right->nodetype != OPERATION_NODE) 
                    {
                        IR_node *temp_node = (IR_node *)malloc(sizeof(IR_node));

                        //printf("HEAD IS: %s R%d %s\n", head->instruction, head->rd.reg, head->rs1.name);

                        temp_node->next = head->next;
                        head->next->prev = temp_node;       
                        head->next = temp_node;
                        temp_node->prev = head;
                        
                        temp_node->reg = register_counter++;
                        temp_node->ir_type = BNE;
                        temp_node->instruction = "bne";
                        temp_node->rs1.reg = temp_node->next->reg; 
                        temp_node->rs2.reg = 0; 

                        push(secondary_stack, temp_node); 

                        node->next = head;
                        node->prev = NULL;
                        head->prev = node;

                        node->ir_type = BEQ;
                        node->instruction = "beq";
                        node->rs1.reg = node->next->reg; 
                        node->rs2.reg = 0; 

                        push(stack, node);
                    }
                    else if(root->right->operation == LOGIC_OR_OP)
                    {
                        head->next->ir_type = BNE;
                        head->next->instruction = "bne";
                        push(secondary_stack, pop(stack)); // holy shit

                        node->next = head;
                        node->prev = NULL;
                        head->prev = node;

                        node->ir_type = BEQ;
                        node->instruction = "beq";
                        node->rs1.reg = node->next->reg; 
                        node->rs2.reg = 0; 

                        push(stack, node);
                    }
                    else
                    {
                        // when AND op is on the right (mixed operations example)

                        node->ir_type = BNE;
                        node->instruction = "bne";
                        node->rs1.reg = node->next->reg; 
                        node->rs2.reg = 0;
                        push(secondary_stack, node);

                        /* First insert current head - lui, into a proper spot (4 instructions deep) */
                        IR_node *temp = head;
                        
                        head = head->next;
                        head->prev = NULL;

                        temp->next = head->next->next->next->next;
                        head->next->next->next->next->prev = temp;
                        temp->prev = head->next->next->next;
                        temp->prev->next = temp;

                        /* Next create this node of type bne and insert it again 4 instructions deep, and return head */
                        node->next = head->next->next->next->next;
                        head->next->next->next->next->prev = node;
                        node->prev = head->next->next->next;
                        node->prev->next = node;

                        return head;
                    }
                break;
            }
        break;

        case IF_NODE:
            root->value.label_count = label_counter++;
            node->ir_type = LABEL;
            
            sprintf(tmp, "L%d", root->value.label_count);
            node->instruction = tmp;
            root->left->value.label_count = root->value.label_count;

            while(secondary_stack->top != NULL)
            {
                IR_node* help = pop(secondary_stack);
                help->rd.label = tmp;
            }
        break;

        case ELSE_NODE:
            
            jmp->ir_type = JAL;
            jmp->instruction = "jal";
            jmp->rd.reg = 0; // by ISA docs - pseudo j (jamp) instruction is jal with rd set as x0
            jmp->next = head->next; // delete IFs node
            jmp->prev = NULL;
            head->next->prev = jmp;

            node->next = jmp;
            node->prev = NULL;
            jmp->prev = node;


            root->value.label_count = root->right->value.label_count;

            node->ir_type = LABEL;
            
            sprintf(tmp, "L%d", root->value.label_count);
            node->instruction = tmp;
            root->left->value.label_count = root->value.label_count;

            IR_node *help = pop(stack);
            help->rd.label = tmp;

            ASTnode *tail = root->right->right;
            tail->right = NULL;
            int num_AND = count_subtree_AND_OPs(tail);

            for(int i = 0; i < num_AND; i++)
            {
                help = pop(stack);
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
                    //node->ir_type = LABEL;
                    root->value.label_count = root->right->value.label_count;
                    //root->value.label_count = label_counter++;
                    
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;                    

                    IR_node *help = pop(stack);
                    help->rd.label = tmp;

                    ASTnode *tail = root->right->right;
                    tail->right = NULL;

                    /* 
                    * When there are multiple subexpressions or operands in logical expressions, for each branching
                    * there will be and instruction node created and it will be pushed to stack, so I make a temporary sub-tree
                    * and cut sub-tree from the rest of AST by inserting NULL pointer to its tail, so I traverse only subtree from current above-IF node
                    * down to boolean expression of if node and count how many times I need to pop nodes off the stack 
                    */

                    int num_AND = count_subtree_AND_OPs(tail);

                    for(int i = 0; i < num_AND; i++)
                    {
                        help = pop(stack);
                        help->rd.label = tmp;
                    }
                }
                else if(root->right->nodetype == ELSE_NODE)
                {
                    node->ir_type = LABEL;
                    root->value.label_count = label_counter++;
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;                    

                    IR_node *help = pop(stack);
                    help->rs1.label = tmp;
                }
            }
        break;

        case SCOPE_NODE:
            if(root->left != NULL)
            {
                if(root->left->nodetype == IF_NODE) // ??? i think this solves bug when if is the first AST node after main's scope, should get looked at
                {
                    //node->ir_type = LABEL;
                    char *tmp = malloc(5 * sizeof(char));
                    sprintf(tmp, "L%d", root->left->value.label_count);
                    node->instruction = tmp;     

                    IR_node *help = pop(stack);
                    help->rd.label = tmp;
                }
                else
                {
                    char *tmp = malloc(5 * sizeof(char));
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->instruction = tmp;     

                    // if(secondary_stack->top != NULL)
                    // {
                    //     IR_node *help = pop(secondary_stack);
                    //     help->rd.label = tmp;
                    // }
                }
            }      
        break;

        case FUNCTION_NODE:
            node->ir_type = LABEL;
            node->instruction = root->left->name;

            // Remove ugly ID of function from list (LW Rx,main)
            node->next = (node->next)->next;
            node->next->prev = node;

            IR_node *rtn = node->next;
            rtn->prev = NULL;

            // This is going to be so ugly so keep in mind to fix this
            // I need info at this point where head (I mean tail) is so that I can insert
            // This node between tail and its next node...
            // So I am going to do the stupidest thing ever and loop the entire list until I find tail
            IR_node *temp = node;

            while(temp->next->ir_type != HEAD)
            {
                temp = temp->next;
            }
            temp->next->prev = node;
            node->next = temp->next;

            temp->next = node;
            node->prev = temp;
            return rtn;
        break;
    }

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
