#include "ast.h"

ASTnode* mkASTnode(int operation, ASTnode *left, ASTnode *right, int value)
{
    ASTnode *n;
    n = (struct ASTnode *)malloc(sizeof(struct ASTnode));
    if(n == NULL) printf("Error allocating memory in mkASTnode\n");

    n->operation = operation;
    n->left = left;
    n->right = right;
    n->value = value;
    return (n);
}