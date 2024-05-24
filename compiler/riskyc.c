#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "riskyc.h"
#include "y.tab.h"
#include "symboltable.h"
#include "ir.h"

extern int lineno;
extern FILE *yyin;
extern char *yytext;
ht* table;
ASTnode *root;

// Copied from ht.c
typedef struct {
    char* key;  // key is NULL if this slot is empty
    const char* value;
    int line;
    ASTnode* node;
} ht_entry;

struct ht {
    ht_entry* entries;  // hash slots
    size_t capacity;    // size of _entries array
    size_t length;      // number of items in hash table
};

typedef struct {
    char* key;
    const char* value;
    int line; 
    ASTnode* node;
} item;

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
    if(ht_set(table, name, datatype, line, node) == NULL) 
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
    init_stack(&stack);
    IR_node *IR_tail = populate_IR(root, IR_head, &stack);

    printf("\n\nASM:\n\n");

    while(IR_head->prev != IR_tail)
    {
      IR_head = IR_head->prev;

      switch(IR_head->ir_type)
      {
        case LUI:
          printf("\tLUI R%d,%d\n", IR_head->rd.reg, IR_head->rs1.int_constant);
          fprintf(asm_file, "\tLUI R%d,%d\n", IR_head->rd.reg, IR_head->rs1.int_constant);
        break;

        case LW:
          printf("\tLW R%d,%s\n", IR_head->rd.reg, IR_head->rs1.name);
          fprintf(asm_file, "\tLW R%d,%s\n", IR_head->rd.reg, IR_head->rs1.name);
        break;

        case ADD:
          printf("\tADD R%d,R%d,R%d\n", IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
          fprintf(asm_file, "\tADD R%d,R%d,R%d\n", IR_head->rd.reg, IR_head->rs1.reg, IR_head->rs2.reg);
        break;

        case SW:
          printf("\tSW %s,R%d\n", IR_head->rd.name, IR_head->rs1.reg);
          fprintf(asm_file, "\tSW %s,R%d\n", IR_head->rd.name, IR_head->rs1.reg);
        break;

        case LABEL:
          printf(".%s:\n", IR_head->rd.label);
          fprintf(asm_file, ".%s:\n", IR_head->rd.label);
        break;

        case BNE:
          printf("\tBNE R%d,R%d,.%s\n", IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
          fprintf(asm_file, "\tBNE R%d,R%d,.%s\n", IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
        break;

        case BGE:
          printf("\tBGE R%d,R%d,.%s\n", IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
          fprintf(asm_file, "\tBGE R%d,R%d,.%s\n", IR_head->rs1.reg, IR_head->rs2.reg, IR_head->rd.label);
        break;
      }
    }

    fclose(asm_file);

    ht_destroy(table);
    freeAST(root);

    return 0;
}