#ifndef AST
#define AST

#include <stdlib.h>

typedef enum operation_type {ADD_OP = 1, SUB_OP, MUL_OP, DIV_OP} operation_type;

typedef struct ASTnode {
    struct ASTnode *left;
    struct ASTnode *right;

    
        operation_type operation;
        int value;
    

} ASTnode;


ASTnode* mkASTnode(ASTnode *left, ASTnode *right);
ASTnode* new_AST_leaf();
ASTnode* new_ASTnode_VALUE(int value);
ASTnode* new_ASTnode_ARITH_OPERATION(operation_type operation, ASTnode *left, ASTnode *right);


#endif