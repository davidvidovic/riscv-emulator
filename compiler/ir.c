#include "ir.h"

/*
* Function that walks the AST and creates IR tree based on it
*/

int register_counter = 0;

IR_node* populate_IR(ASTnode *root, IR_node *head, Stack *stack)
{
    IR_node *this_node = NULL;

    if(root->right != NULL) 
    {
        this_node = populate_IR(root->right, head, stack);
    }

    if(root->left != NULL) 
    {
        if(this_node == NULL)
            this_node = populate_IR(root->left, head, stack);
        else
            this_node = populate_IR(root->left, this_node, stack);
    }

    if(this_node != NULL)
        head = this_node; 

    return insert_IR(root, head, stack);
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
IR_node* insert_IR(ASTnode *root, IR_node *head, Stack *stack)
{ 
    IR_node *node = (IR_node *)malloc(sizeof(IR_node));
    
    node->prev = NULL;
    node->next = head;
    head->prev = node;

    node->reg = register_counter++;

    switch(root->nodetype)
    {
        case CONSTANT_NODE:
            node->ir_type = LUI;
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
            node->rs1.name = root->name;
            node->rd.reg = node->reg;      

        break;

        case OPERATION_NODE:
            switch(root->operation)
            {
                case EQU_OP:
                    node->ir_type = SW;
                    node->rd.name = node->next->rs1.name;
                    
                    // Delete LW node created by visiting right ID node from list, it is not needed
                    node->next = (node->next)->next;
                    (node->next)->prev = node;

                    node->rs1.reg = node->next->rd.reg;
                break;

                case ADD_OP:
                    node->ir_type = ADD;
                    node->rd.reg = node->reg;
                    node->rs1.reg = node->next->rd.reg;
                    node->rs2.reg = (node->next)->next->rd.reg;
                break;

                /*
                * If it is logical expression it should be followed by branch or jump instructions
                * No matter if it is expression evaluating if statement or value assigment to variable
                */
                case LOGIC_AND_OP:

                break;

                case LOGIC_EQU_OP:
                    node->ir_type = BNE;
                    node->rs1.reg = node->next->reg;
                    node->rs2.reg = node->next->next->reg;    
                    push(stack, node);       
                break;

                case LOGIC_GET_OP:

                break;

                case LOGIC_GT_OP:
                    
                break;

                case LOGIC_LET_OP:

                break;

                case LOGIC_LT_OP:
                    node->ir_type = BGE;
                    node->rs1.reg = node->next->reg;
                    node->rs2.reg = node->next->next->reg; 
                    push(stack, node);  
                break;

                case LOGIC_NEQU_OP:

                break;

                case LOGIC_NOT_OP:

                break;

                case LOGIC_OR_OP:

                break;
            }
        break;

        case IF_NODE:
            node->ir_type = LABEL;
            char *tmp = malloc(5 * sizeof(char));
            sprintf(tmp, "L%d", root->value.label_count);
            node->rd.label = tmp;
        break;

        case EXPRESSION_NODE:
            if(root->right != NULL)
            {
                if(root->right->nodetype == IF_NODE)
                {
                    //node->ir_type = LABEL;
                    char *tmp = malloc(5 * sizeof(char));
                    sprintf(tmp, "L%d", root->value.label_count);
                    node->rd.label = tmp;                    

                    // IR_node *help = node;
                    // while(help->next->ir_type != BNE)
                    // {
                    //     help = help->next;
                    // }
                    // help->next->rd.label = tmp;
                    IR_node *help = pop(stack);
                    help->rd.label = tmp;
                }
            }
        break;

        case FUNCTION_NODE:
            node->ir_type = LABEL;
            node->rd.label = root->left->name;

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