#include "ir.h"

/*
* Function that walks the AST and creates IR tree based on it
*/

int register_counter = 0;

IR_node* populate_IR(ASTnode *root, IR_node *head)
{
    IR_node *this_node = NULL;

    if(root->right != NULL) 
    {
        this_node = populate_IR(root->right, head);
    }

    if(root->left != NULL) 
    {
        if(this_node == NULL)
            this_node = populate_IR(root->left, head);
        else
            this_node = populate_IR(root->left, this_node);
    }

    if(this_node != NULL)
        head = this_node;

    return insert_IR(root, head);
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

IR_node* insert_IR(ASTnode *root, IR_node *head)
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
                    
                    // Delete LW node from list, it is not needed
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
                    node->ir_type = BEQ;
                break;

                case LOGIC_GET_OP:

                break;

                case LOGIC_GT_OP:
                    
                break;

                case LOGIC_LET_OP:

                break;

                case LOGIC_LT_OP:

                break;

                case LOGIC_NEQU_OP:

                break;

                case LOGIC_NOT_OP:

                break;

                case LOGIC_OR_OP:

                break;
            }
        break;

        case FUNCTION_NODE:
            node->ir_type = LABEL;
            node->rd.label = root->left->name;

            // Remove ugly ID of function from list
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
