#include "ir.h"

IR_node* create_IR(ASTnode *root)
{
    IR_node* head = create_IR_node(NONE, NULL, NULL);
    insert_IR(root, head);
    return head;
}

/*
* Function that walks the AST and creates IR tree based on it
*/

void insert_IR(ASTnode *root, IR_node *node)
{
    IR_node *this_node;

    switch(root->nodetype)
    {
        case FUNCTION_NODE:
            this_node = create_IR_node(IR_PROCEDURE, node, NULL);
        break;


    }

    if(root->right != NULL) {
        insert_IR(root->right, this_node);
    }

    if(root->left != NULL) {
        insert_IR(root->left, this_node);
    }
}


/*
* Create base IR node
*/

IR_node* create_IR_node(IR_instruction_type ir_type, IR_node *prev, IR_node *next)
{
    IR_node *node = (IR_node *)malloc(sizeof(IR_node));
    node->ir_type = ir_type;
    node->next = next;
    node->prev = prev;

    return node;
}