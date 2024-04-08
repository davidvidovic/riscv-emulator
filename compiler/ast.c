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

ASTnode* new_AST_leaf()
{
    return mkASTnode(NULL, NULL); 
}

ASTnode* new_ASTnode_ID(const char* name, id_type type, ASTnode *left, ASTnode *right)
{
    ASTnode *n = new_AST_leaf();
    n->name = name;
    n->type = type;
    return (n);
}


ASTnode* new_ASTnode_INT(int value)
{
    ASTnode *n = new_AST_leaf();
    n->value = value;
    return (n);
}


ASTnode* new_ASTnode_ARITH_OPERATION(operation_type operation, ASTnode *left, ASTnode *right)
{
    ASTnode *n = mkASTnode(left, right);
    n->operation = operation;
    return (n);
}


// This func will go through entire AST just to find ID node, not sure if correct...
ASTnode* find_ID(const char* name, ASTnode* root)
{
    if(root->name == name) return root;

    if(root->left != NULL) find_ID(name, root->left);
    if(root->right != NULL) find_ID(name, root->right);

    return root;
}