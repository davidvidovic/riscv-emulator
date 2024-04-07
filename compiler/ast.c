#include "ast.h"

ASTnode* mkASTnode(ASTnode *left, ASTnode *right)
{
    ASTnode *n;
    n = (struct ASTnode *)malloc(sizeof(struct ASTnode));
    if(n == NULL) printf("Error allocating memory in mkASTnode\n");

    n->left = left;
    n->right = right;
    return (n);
}


ASTnode* new_ASTnode_VALUE(int value)
{
    ASTnode *n = mkASTnode(NULL, NULL); // leaf
    n->value = value;
    return (n);
}


ASTnode* new_ASTnode_ARITH_OPERATION(operation_type operation, ASTnode *left, ASTnode *right)
{
    ASTnode *n = mkASTnode(left, right); 
    n->operation = operation;
    return (n);
}
