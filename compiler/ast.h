#ifndef AST
#define AST

#include <stdlib.h>

typedef enum operation_type {ADD_OP = 1, SUB_OP, MUL_OP, DIV_OP, MOD_OP, EQU_OP} operation_type;
typedef enum id_type {TYPE_INT = 1, TYPE_FLOAT, TYPE_DOUBLE, TYPE_SHORT, TYPE_CHAR, TYPE_STRING, TYPE_VOID} id_type;

typedef struct ASTnode {
    struct ASTnode *left;
    struct ASTnode *right;

    // ID
    id_type type;
    char* name;

    // Constants
    int value; // add float, char...

    // Expressions
    operation_type operation;

} ASTnode;

ASTnode* mkASTnode(ASTnode *left, ASTnode *right);
ASTnode* new_AST_leaf();
ASTnode* new_ASTnode_ID(const char* name, id_type type, ASTnode *left, ASTnode *right);
ASTnode* new_ASTnode_INT(int value);
ASTnode* new_ASTnode_ARITH_OPERATION(operation_type operation, ASTnode *left, ASTnode *right);

#endif

