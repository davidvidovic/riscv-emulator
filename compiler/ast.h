#include <stdlib.h>

enum {ADD_OP, SUB_OP, MULT_OP, DIV_OP};

typedef struct ASTnode {
    int operation;
    struct ASTnode *left;
    struct ASTnode *right;
    int value;
} ASTnode;

ASTnode* mkASTnode(int operation, ASTnode *left, ASTnode *right,  int value);