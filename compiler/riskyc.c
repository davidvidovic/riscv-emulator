#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "riskyc.h"
#include "y.tab.h"
#include "symboltable.h"
#include "ir.h"
#include "ast.h"
#include "control_flow.h"
#include "register_allocation.h"

extern int lineno;
extern FILE *yyin;
extern char *yytext;
ht* table;
ASTnode *root;
int sp_offset;

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}


int declare(const char* name, const char* datatype, int line, ASTnode* node)
{
  if(ht_get_key(table, name) != NULL) 
  {
    char error[100];
    strcpy(error, "\033[31mERROR \t\tIdentifier ");
    strcat(error, name);
    strcat(error, " already declared.\033[0m");

    strcat(error, "\nPrevious declaration at line ");
    int error_line = ht_get_line(table, name);
    char temp[10];
    sprintf(temp, "%d", error_line);
    strcat(error, temp);
    strcat(error, "\nError");
    yyerror(error);

    exit(1);
  }
  else
  {
    if(ht_set(table, name, datatype, line, node, NO_TYPE, 0) == NULL) 
    {
      exit_nomem();
    }
    else
    {
      //printf("Added to table %s with value %s\n", ht_get_key(table, $2.name), $1);
      return 1;
    }
  }

  return 0;
}

ASTnode* check_declaration(const char* name)
{
  if(ht_get_key(table, name) == NULL) 
  {
    char error[100];
    strcpy(error, "\033[31mERROR \t\tIdentifier ");
    strcat(error, name);
    strcat(error, " not declared.\033[0m");
    strcat(error, "\nError");
    yyerror(error);

    exit(1);
  }

  return ht_get_ASTnode(table, name);
}


void ht_set_type_sp_offset(const char* key, id_type type, int sp_offset)
{
  ht_entry *temp = get_ht_entry(table, key);
  if(temp == NULL)
  {
    printf("ERROR: ht_get_entry returned NULL for %s\n", key);
    return;
  }
  else
  {
    temp->type = type;
    temp->sp_offset = sp_offset;
    ht_set_sf_offset(table, key, sp_offset);
    ASTnode *n = ht_get_ASTnode(table, key);
    n->type = type;
  }
}

void type_check(ASTnode *op1, ASTnode *op2)
{
  id_type op1_type, op2_type;
  char *op1_name, *op2_name;
  ht_entry *temp, *temp2;

  /* Done over AST node for now, ST stuff doesn't work for some reason... */

	if(op1->nodetype == ID_NODE || op1->nodetype == POINTER_NODE)
	{
    op1_name = op1->name;
		//temp = get_ht_entry(table, op1_name);
    //op1_type = temp->type;
    op1_type = op1->type; 
	}
  else if(op1->nodetype == CONSTANT_NODE)
  {
    /* Constant node */
    op1_type = op1->type; 
    sprintf(op1_name, "%d", op1->value.value_INT);
  }
  else
  {
    /* Operation to the right */
    if(op1->left->nodetype != OPERATION_NODE)
      op1_type = op1->left->type; 
    else if(op1->right->nodetype != OPERATION_NODE)
      op1_type = op1->right->type; 
    op1_name = "OPERATION";
  }

  if(op2->nodetype == ID_NODE || op2->nodetype == POINTER_NODE)
	{ 
		op2_name = op2->name;
		//temp2 = get_ht_entry(table, op2_name);
    //op2_type = temp2->type;
    op2_type = op2->type; 
	}
  else if(op2->nodetype == CONSTANT_NODE)
  {
    /* Constant node */
    op2_type = op2->type; 
    sprintf(op2_name, "%d", op2->value.value_INT);
  }
  else
  {
    /* Operation to the right */
    if(op2->left->nodetype != OPERATION_NODE)
      op2_type = op2->left->type; 
    else if(op2->right->nodetype != OPERATION_NODE)
      op2_type = op2->right->type; 
    op2_name = "OPERATION";
  }

  if(op1_type != op2_type)
  {
    char error[100];
    strcpy(error, "\033[31mERROR \t\tTypes of operators ");
    strcat(error, op1_name);
    strcat(error, " and ");
    strcat(error, op2_name);
    strcat(error, " do not match.\n\t\t\033[0m");
    yyerror(error);
    
    exit(1);
  }
}


int depth = 0;

int walkAST(ASTnode *root, int depth)
{
  depth += 3;

  if(root->right != NULL) {
    walkAST(root->right, depth);
  }

  for(int i = 3; i < depth; i++) printf(" ");
  print_value(root);

  if(root->left != NULL) {
    walkAST(root->left, depth);
  }
  
  return 0;
}


void freeAST(ASTnode *root)
{
  if(root == NULL) return;

  freeAST(root->left);
  freeAST(root->right);

  // printf("Freeing: ");
  // print_value(root);

  // The problem here was with ID nodes in AST, it gets deallocated as soon as it is found in the tree
  // Any further access to this node results in seg fault because that data is already deallocated
  // Setting that pointer (memory space allocated for it) to NULL before free() seems to be a work around and a hot-fix for this issue
  root = NULL;
  free(root);
}


int main()
{
    table = ht_create();
    if (table == NULL) 
    {
        exit_nomem();
    }

    root = mkASTnode(NULL, NULL);

    sp_offset = 0;
    yyin = fopen("../input.c", "r");
    yyparse();

    for(int i = 0; i < table->capacity; i++) 
    { 
        if(table->entries[i].key != NULL) 
        {
            //printf("index %d:\t%s, ", i, table->entries[i].key);

            int* adr = table->entries[i].value;
            int off = 0;
            while(*adr != NULL) 
            {
            //printf("%c", (char)*adr);
            adr = table->entries[i].value + (++off);
            }

            //printf("\tat line %d", table->entries[i].line);
            //printf("\n");
        } 
        else
        {
            //printf("Index %d: Empty\n", i);
        } 
    }
    fclose(yyin);

    
    // Walk the AST
    
    /* Small bug, ID stays on the right if no init declarations are made */ 
    //if(root->right->right != EXPRESSION_NODE)
     // root->right->right = NULL;
    
    walkAST(root, depth);


    
    IR_node *IR_head = create_IR();
    Stack stack;
    Stack secondary_stack;
    Stack break_stack;
    Stack continue_stack;
    Stack return_stack;
    Stack array_element_stack;
    init_stack(&stack);
    init_stack(&secondary_stack);
    init_stack(&break_stack);
    init_stack(&continue_stack);
    init_stack(&return_stack);
    init_stack(&array_element_stack);
    register_pool *rp = init_register_pool();
  
    IR_node *IR_tail = populate_IR(root, IR_head, &stack, &secondary_stack, &break_stack, &continue_stack, &return_stack, &array_element_stack, rp, table);

    print_IR(IR_head, IR_tail);
    
    // control_flow_graph *cfg = populate_cfg(IR_head, IR_tail);
    // basic_block* temp = cfg->leader;
  
    // do
    // {
    //   printf("BLOCK %d: first instruction is %s and last instruction is %s\n", temp->bb_number, temp->leader->instruction, temp->last_instruction->instruction);
    //   temp = temp->bb_next;
    // } while(temp != NULL);


    assemble_binary_output(IR_head, IR_tail);

    ht_destroy(table);
    freeAST(root);

    return 0;
}
