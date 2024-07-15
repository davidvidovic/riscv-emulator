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

    //int len = strlen(name);
    //n->name = malloc(len * sizeof(char));
    n->name = name;
    n->type = type;
    n->structure = VARIABLE;
    n->element_number = 1;

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

ASTnode* new_ASTnode_CHAR(char value)
{
    ASTnode *n = new_AST_leaf();
    n->nodetype = CONSTANT_NODE;
    n->type = TYPE_CHAR;
    n->value.value_CHAR = value;
    return (n);
}

ASTnode* new_ASTnode_OPERATION(operation_type operation, ASTnode *left, ASTnode *right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = OPERATION_NODE;
    n->operation = operation;
    n->line = line; 
    return (n);
}


int expression_counter = 0;

ASTnode* new_ASTnode_EXPRESSION(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = EXPRESSION_NODE;

    // To give a name to expression I will use simple global counter
    // This is not necessary but in order to keep the tree printable and readable, all nodes should be named

    n->name = malloc(10 * sizeof(char));
    snprintf((n->name), 10, "exp%d", expression_counter++);    
    n->line = line;

    return (n);
}


int scope_counter = 0;

ASTnode* new_ASTnode_SCOPE(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = SCOPE_NODE;

    n->name = malloc(12 * sizeof(char));
    snprintf((n->name), 10, "scope%d", scope_counter++);    
    n->line = line;

    return (n);
}



ASTnode* new_ASTnode_IF(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = IF_NODE;
    //n->value.label_count = label_counter++;
    n->line = line;
    return (n);
}


ASTnode* new_ASTnode_ELSE(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = ELSE_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_WHILE(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = WHILE_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_DO(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = DO_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_FOR(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = FOR_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_LABEL(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = LABEL_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_FUNCTION(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = FUNCTION_NODE;
    n->name = left->name;
    n->line = line;

    return (n);
}

ASTnode* new_ASTnode_FUNCTION_HEAD(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = FUNCTION_HEAD_NODE;
    n->name = left->name;
    n->line = line;

    return (n);
}

ASTnode* new_ASTnode_BREAK(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = BREAK_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_CONTINUE(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = CONTINUE_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_DEFAULT(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = DEFAULT_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_SWITCH(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = SWITCH_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_CASE(ASTnode* left, ASTnode* right, ASTnode *const_exp, int lineno)
{
    ASTnode *case_body = mkASTnode(left, new_ASTnode_EXPRESSION(new_ASTnode_OPERATION(LOGIC_EQU_OP, const_exp, NULL, lineno), NULL, lineno));
    case_body->nodetype = CASE_BODY_NODE;
    case_body->line = lineno;

    ASTnode *n = mkASTnode(case_body, right);
    n->nodetype = CASE_NODE;
    n->line = lineno;
    return (n);
}

ASTnode* new_ASTnode_RETURN(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = RETURN_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_ARRAY_ELEMENT(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = ARRAY_ELEMENT_NODE;
    n->line = line;
    n->name = left->name;
    return (n);
}

ASTnode* new_ASTnode_FUNCTION_CALL_NODE(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = FUNCTION_CALL_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_ARGUMENT_NODE(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = ARGUMENT_NODE;
    n->line = line;
    return (n);
}

ASTnode* new_ASTnode_PARAMETER_NODE(ASTnode* left, ASTnode* right, int line)
{
    ASTnode *n = mkASTnode(left, right);
    n->nodetype = PARAMETER_NODE;
    n->line = line;
    return (n);
}



void print_value(ASTnode* n)
{
    switch(n->nodetype)
    {
    case ID_NODE:
        if(n->name != NULL) 
        {
            printf("Node ID(%s, %s)\n", n->name, print_type(n->type));
        }
        break;

    case CONSTANT_NODE:
        if(n->type != NO_TYPE)
        {
            printf("Node CONST(%s, ", print_type(n->type));
            switch(n->type)
            {
                case NO_TYPE:
                    break;

                case TYPE_INT:
                    printf("%d)\n", n->value.value_INT);
                    break;

                case TYPE_FLOAT:
                    printf("%f)\n", n->value.value_FLOAT);
                    break;

                case TYPE_CHAR:
                    printf("%c)\n", n->value.value_CHAR);
                break;
            }
        }
        break;

    case OPERATION_NODE:
        printf("Node OP(%s)\n", print_operation(n->operation));
        break;

    case EXPRESSION_NODE:
        printf("Node EXP(%s)\n", n->name);
        break;

    case SCOPE_NODE:
        printf("Node SCOPE(%s)\n", n->name);
        break;

    case IF_NODE:
        printf("Node IF\n");
        break;

    case ELSE_NODE:
        printf("Node ELSE\n");
        break;

    case WHILE_NODE:
        printf("Node WHILE\n");
        break;

    case DO_NODE:
        printf("Node DO\n");
        break;

    case FOR_NODE:
        printf("Node FOR\n");
        break;
    
    case LABEL_NODE:
        printf("Node LABEL\n");
        break;

    case FUNCTION_NODE:
        printf("Node FUNC(%s)\n", n->name);
        break;
    
    case BREAK_NODE:
        printf("Node BREAK\n");
        break;

    case SWITCH_NODE:
        printf("Node SWITCH\n");
        break;

    case CASE_NODE:
        printf("Node CASE\n");
        break;

    case CASE_BODY_NODE:
        printf("Node CASE BODY\n");
        break;

    case CONTINUE_NODE:
        printf("Node CONTINUE\n");
        break;

    case DEFAULT_NODE:
        printf("Node DEFAULT\n");
        break;

    case RETURN_NODE:
        printf("Node RETURN\n");
        break;

    case POINTER_NODE:
        printf("Node POINTER\n");
        break;

    case ARRAY_ELEMENT_NODE:
        printf("Node ARRAY ELEMENT\n");
        break;

    case FUNCTION_HEAD_NODE:
        printf("Node FUNCTION_HEAD_NODE\n");
        break;

    case FUNCTION_CALL_NODE:
        printf("Node FUNCTION_CALL_NODE\n");
        break;

    case ARGUMENT_NODE:
        printf("Node ARGUMENT_NODE\n");
        break;

    case PARAMETER_NODE:
        printf("Node PARAMETER_NODE\n");
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

    case MOD_OP:
        return "MOD_OP";

    case NEQU_OP:
        return "NEQU_OP";

    case MUL_ASSIGN_OP:
        return "MUL_ASSIGN_OP";

    case DIV_ASSIGN_OP:
        return "DIV_ASSIGN_OP";

    case MOD_ASSIGN_OP:
        return "MOD_ASSIGN_OP";

    case ADD_ASSIGN_OP:
        return "ADD_ASSIGN_OP";

    case SUB_ASSIGN_OP:
        return "SUB_ASSIGN_OP";

    case LEFT_ASSIGN_OP:
        return "LEFT_ASSIGN_OP";

    case RIGHT_ASSIGN_OP:
        return "RIGHT_ASSIGN_OP";

    case AND_ASSIGN_OP:
        return "AND_ASSIGN_OP";

    case XOR_ASSIGN_OP:
        return "XOR_ASSIGN_OP";

    case OR_ASSIGN_OP:
        return "OR_ASSIGN_OP";

    case LOGIC_EQU_OP:
        return "LOGIC_EQU_OP";

    case LOGIC_NEQU_OP:
        return "LOGIC_NEQU_OP";

    case LOGIC_OR_OP:
        return "LOGIC_OR_OP";

    case LOGIC_AND_OP:
        return "LOGIC_AND_OP";

    case LOGIC_NOT_OP:
        return "LOGIC_NOT_OP";

    case LOGIC_LT_OP:
        return "LOGIC_LT_OP";

    case LOGIC_GT_OP:
        return "LOGIC_GT_OP";

    case LOGIC_LET_OP:
        return "LOGIC_LET_OP";

    case LOGIC_GET_OP:
        return "LOGIC_GET_OP";

    case BITWISE_NOT_OP:
        return "BITWISE_NOT_OP";

    case BITWISE_SHIFT_LEFT:
        return "Node BITWISE_SHIFT_LEFT\n";

    case BITWISE_SHIFT_RIGHT:
        return "Node BITWISE_SHIFT_RIGHT\n";

    case BITWISE_AND:
        return "Node BITWISE_AND\n";

    case BITWISE_OR:
        return "Node BITWISE_OR\n";

    case BITWISE_XOR:
        return "Node BITWISE_XOR\n";
    
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


