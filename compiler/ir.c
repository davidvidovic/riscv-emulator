#include "ir.h"


/*
* Function that walks the AST and creates IR tree based on it
*/

void create_IR(ASTnode *root, FILE *asm_file)
{
    switch(root->nodetype)
    {
        case FUNCTION_NODE:
            fprintf(asm_file, ".%s:\n", root->name);
            break;

        case OPERATION_NODE:
            switch(root->operation)
            {
            case EQU_OP:
                fprintf(asm_file, "LOAD %s\n", root->left->name);
                
                if(root->right->nodetype == OPERATION_NODE)
                {
                    switch(root->right->operation)
                    {
                        case ADD_OP:
                            fprintf(asm_file, "ADD %s,", root->left->name);
                            break;
                    }     

                    if(root->right->right->nodetype == CONSTANT_NODE)
                    {
                        switch(root->right->right->type)
                        {
                            case TYPE_INT:
                                fprintf(asm_file, "%d,", root->right->right->value.value_INT);
                            break;

                            case TYPE_FLOAT:
                                fprintf(asm_file, "%f,", root->right->right->value.value_FLOAT);
                            break;

                            case TYPE_CHAR:
                                fprintf(asm_file, "%c,", root->right->right->value.value_CHAR);
                            break;
                        }
                    }
                    else
                        fprintf(asm_file, "%s,", root->right->right->name);    

                    if(root->right->left->nodetype == CONSTANT_NODE)
                    {
                        switch(root->right->left->type)
                        {
                            case TYPE_INT:
                                fprintf(asm_file, "%d\n", root->right->left->value.value_INT);
                            break;

                            case TYPE_FLOAT:
                                fprintf(asm_file, "%f\n", root->right->left->value.value_FLOAT);
                            break;

                            case TYPE_CHAR:
                                fprintf(asm_file, "%c\n", root->right->left->value.value_CHAR);
                            break;
                        }
                    }
                    else
                        fprintf(asm_file, "%s\n", root->right->left->name);
                }
                else
                {
                    fprintf(asm_file, "ADDI %s,%s,%d\n", root->left->name, root->left->name, root->right->value.value_INT);
                }
                
                fprintf(asm_file, "STORE %s\n", root->left->name);
                break;

            break;
                
            
            default:
                break;
            }
        break;

        default:
            break;
    }

    if(root->right != NULL) {
        create_IR(root->right, asm_file);
    }

    if(root->left != NULL) {
        create_IR(root->left, asm_file);
    }
}


