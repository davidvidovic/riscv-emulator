#ifndef AST
#define AST

#include <stdlib.h>
#include <string.h>

typedef enum node_type {
    ID_NODE,
    CONSTANT_NODE,
    OPERATION_NODE,
    EXPRESSION_NODE,
    SCOPE_NODE,
    IF_NODE,
    ELSE_NODE,
    WHILE_NODE,
    FUNCTION_NODE,
    LABEL_NODE
} node_type;

typedef enum operation_type {
    // Arithmetic
    ADD_OP = 1, 
    SUB_OP, 
    MUL_OP, 
    DIV_OP, 
    MOD_OP, 
    EQU_OP,
    NEQU_OP,
    MUL_ASSIGN_OP, 
    DIV_ASSIGN_OP, 
    MOD_ASSIGN_OP, 
    ADD_ASSIGN_OP, 
    SUB_ASSIGN_OP,
    LEFT_ASSIGN_OP, 
    RIGHT_ASSIGN_OP, 
    AND_ASSIGN_OP, 
    XOR_ASSIGN_OP, 
    OR_ASSIGN_OP,
    // Logic
    LOGIC_EQU_OP,
    LOGIC_NEQU_OP,
    LOGIC_OR_OP,
    LOGIC_AND_OP,
    LOGIC_NOT_OP,
    LOGIC_LT_OP,
    LOGIC_GT_OP,
    LOGIC_LET_OP,
    LOGIC_GET_OP
} operation_type;

typedef enum id_type {
    NO_TYPE = 0, 
    TYPE_INT, 
    TYPE_FLOAT, 
    TYPE_DOUBLE, 
    TYPE_SHORT, 
    TYPE_CHAR, 
    TYPE_STRING, 
    TYPE_VOID,
    TYPE_LONG,
    TYPE_SIGNED,
    TYPE_UNSIGNED
} id_type;


typedef struct ASTnode {
    struct ASTnode *left;
    struct ASTnode *right;
    node_type nodetype;

    // ID
    id_type type;
    char* name;

    // Constants
    union value {
        int value_INT;
        float value_FLOAT;
        double value_DOUBLE;
        char value_CHAR;
        int label_count;
    } value;

    // Expressions
    operation_type operation;
} ASTnode;


ASTnode* mkASTnode(ASTnode *left, ASTnode *right);
ASTnode* new_AST_leaf();

ASTnode* new_ASTnode_ID(const char* name, id_type type, ASTnode *left, ASTnode *right);
ASTnode* new_ASTnode_INT(int value);
ASTnode* new_ASTnode_FLOAT(float value);
ASTnode* new_ASTnode_CHAR(char value);

ASTnode* new_ASTnode_OPERATION(operation_type operation, ASTnode *left, ASTnode *right);
ASTnode* new_ASTnode_EXPRESSION(ASTnode* left, ASTnode* right);
ASTnode* new_ASTnode_SCOPE(ASTnode* left, ASTnode* right);
ASTnode* new_ASTnode_IF(ASTnode* left, ASTnode* right);
ASTnode* new_ASTnode_ELSE(ASTnode* left, ASTnode* right);
ASTnode* new_ASTnode_WHILE(ASTnode* left, ASTnode* right);
ASTnode* new_ASTnode_LABEL(ASTnode* left, ASTnode* right);
ASTnode* new_ASTnode_FUNCTION(ASTnode* left, ASTnode* right);

// Helper functions
void print_value(ASTnode* n);
char* print_operation(operation_type op);
char* print_type(id_type ty);

#endif