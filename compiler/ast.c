#include "ast.h"

ASTnode* mkASTnode(ASTnode *left, ASTnode *right)
{
    ASTnode *n;
    n = (struct ASTnode *)malloc(sizeof(struct ASTnode));
    if(n == NULL) printf("Error allocating memory in mkASTnode\n");

    n->left = left;
    n->right = right;
    n->type = NO_TYPE;
    return (n);
}

ASTnode* new_AST_leaf()
{
    return mkASTnode(NULL, NULL); 
}

ASTnode* new_ASTnode_ID(const char* name, id_type type, ASTnode *left, ASTnode *right)
{
    ASTnode *n = new_AST_leaf();
    n->nodetype = ID_NODE;
    n->name = name;
    n->type = type;
    return (n);
}


ASTnode* new_ASTnode_INT(int value)
{
    ASTnode *n = new_AST_leaf();
    n->nodetype = CONSTANT_NODE;
    n->type = TYPE_INT;
    n->value.value_INT = value;
    return (n);
}

ASTnode* new_ASTnode_FLOAT(float value)
{
    ASTnode *n = new_AST_leaf();
    n->nodetype = CONSTANT_NODE;
    n->type = TYPE_FLOAT;
    n->value.value_FLOAT = value;
    return (n);
}

ASTnode* new_ASTnode_ARITH_OPERATION(operation_type operation, ASTnode *left, ASTnode *right)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = OPERATION_NODE;
    n->operation = operation;
    return (n);
}

int expression_counter = 0;

ASTnode* new_ASTnode_EXPRESSION(ASTnode* left, ASTnode* right)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = EXPRESSION_NODE;

    // To give a name to expression I will use simple global counter
    // This is not necessary but in order to keep the tree printable and readable, all nodes should be named

    n->name = malloc(10 * sizeof(char));
    snprintf((n->name), 10, "exp%d", expression_counter++);    

    return (n);
}


void print_value(ASTnode* n)
{
    switch(n->nodetype)
    {
    case ID_NODE:
        if(n->name != NULL) 
        {
            if(n->name != NULL) printf("Node ID:\t name = %s \ttype = %s\n", n->name, print_type(n->type));
        }
        break;

    case CONSTANT_NODE:
        if(n->type != NO_TYPE)
        {
            printf("Node CONST:\t type = %s \tvalue = ", print_type(n->type));
            switch(n->type)
            {
                case NO_TYPE:
                    break;

                case TYPE_INT:
                    printf("%d\n", n->value.value_INT);
                    break;

                case TYPE_FLOAT:
                    printf("%f\n", n->value.value_FLOAT);
                    break;
            }
        }
        break;

    case OPERATION_NODE:
        printf("Node OP:\t type = %s\n", print_operation(n->operation));
        break;

    case EXPRESSION_NODE:
        printf("Node EXP:\t name = %s\n", n->name);
        break;
    }
}




char* print_operation(operation_type op)
{
    switch(op)
    {
    case ADD_OP:
        return "ADD_OP";

    case SUB_OP:
        return "SUB_OP";

    case MUL_OP:
        return "MUL_OP";

    case DIV_OP:
        return "DIV_OP";

    case EQU_OP:
        return "EQ_OP";

    default:
        return "NOT VALID OPERATION";
    }
}

char* print_type(id_type ty)
{
    switch(ty)
    {
    case TYPE_INT:
        return "TYPE_INT";

    case TYPE_FLOAT:
        return "TYPE_FLOAT";

    case TYPE_DOUBLE:
        return "TYPE_DOUBLE";

    case TYPE_CHAR:
        return "TYPE_CHAR";

    case TYPE_STRING:
        return "TYPE_STRING";

    default:
        //return "NOT VALID TYPE";
        break;
    }
}


