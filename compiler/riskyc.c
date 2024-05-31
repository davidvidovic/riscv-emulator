#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "riskyc.h"
#include "y.tab.h"
#include "symboltable.h"
#include "ir.h"
#include "ast.h"

extern int lineno;
extern FILE *yyin;
extern char *yytext;
ht* table;
ASTnode *root;


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

	if(op1->nodetype == ID_NODE)
	{
    op1_name = op1->name;
		//temp = get_ht_entry(table, op1_name);
    //op1_type = temp->type;
    op1_type = op1->type; 
	}
  else
  {
    /* Constant node */
    op1_type = op1->type; 
    sprintf(op1_name, "%d", op1->value.value_INT);
  }

  if(op2->nodetype == ID_NODE)
	{ 
		op2_name = op2->name;
		//temp2 = get_ht_entry(table, op2_name);
    //op2_type = temp2->type;
    op2_type = op2->type; 
	}
  else
  {
    /* Constant node */
    op2_type = op2->type; 
    sprintf(op2_name, "%d", op2->value.value_INT);
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

    yyin = fopen("input.c", "r");
    int token;
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
    walkAST(root, depth);

    FILE *asm_file = fopen("output.s", "w");
    IR_node *IR_head = create_IR();
    Stack stack;
    Stack secondary_stack;
    init_stack(&stack);
    init_stack(&secondary_stack);
    IR_node *IR_tail = populate_IR(root, IR_head, &stack, &secondary_stack);

    printf("\n\nASM:\n\n");

    while(IR_head->prev != IR_tail)
    {
      IR_head = IR_head->prev;
      switch(IR_head->ir_type)
      {
        case LABEL:
          printf(".%s:\n", IR_head->instruction);
          fprintf(asm_file, ".%s:\n", IR_head->instruction);
        break;
        
        case SW:
          printf("\t%s %s,R%d\n", IR_head->instruction, IR_head->rd.name, IR_head->rs1.reg);
          fprintf(asm_file, "\t%s %s,R%d\n", IR_head->instruction, IR_head->rd.name, IR_head->rs1.reg);
        break;

        case LW:
          printf("\t%s R%d,%s\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.name);
          fprintf(asm_file, "\t%s R%d,%s\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.name);
        break;

        case LUI:
          printf("\t%s R%d,%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.int_constant);
          fprintf(asm_file, "\t%s R%d,%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.int_constant);
        break;        

        case ADD:
        case SUB:
          printf("\t%s R%d,R%d,R%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
          fprintf(asm_file, "\t%s R%d,R%d,R%d\n", IR_head->instruction, IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
        break;

        case BEQ:
        case BNE:
        case BLT:
        case BGE:
        case BLTU:
        case BGEU:
        case BGT:
        case BLE:
          printf("\t%s R%d,R%d,.%s\n", IR_head->instruction, IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
          fprintf(asm_file, "\t%s R%d,R%d,.%s\n", IR_head->instruction, IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
        break;

        case JAL:
          printf("\tjal R%d,.%s\n", IR_head->rd.reg, IR_head->rs1.label);
          fprintf(asm_file, "\tjal R%d,.%s\n", IR_head->rd.reg, IR_head->rs1.label);
        break;

        case IR_NO_TYPE:
        case HEAD:
        break;
      }
    }

    fclose(asm_file);

    ht_destroy(table);
    freeAST(root);

    return 0;
}