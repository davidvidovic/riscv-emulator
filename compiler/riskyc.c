#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "riskyc.h"
#include "y.tab.h"
#include "symboltable.h"

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
} item;

void exit_nomem(void) {
    fprintf(stderr, "out of memory\n");
    exit(1);
}


int declare(const char* name, const char* datatype, int line)
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
    if(ht_set(table, name, datatype, line) == NULL) 
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

void check_declaration(const char* name)
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
}

char* print_operation(operation_type op)
{
  switch(op)
  {
    case ADD_OP:
      return "ADD_OP";

    case SUB_OP:
      return "SUB_OP";

    case MUL_OP:
      return "MUL_OP";

    case DIV_OP:
      return "DIV_OP";

    default:
      return "NOT VALID OPERATION";
  }
}

int walkAST(ASTnode *root)
{
  if(root->operation != NULL) printf("%s\n", print_operation(root->operation));
  if(root->value != NULL) printf("%d\n", root->value);

  if(root->left != NULL) walkAST(root->left);
  if(root->right != NULL) walkAST(root->right);

  return 0;
}


void freeAST(ASTnode *root)
{
  if(root->left != NULL) freeAST(root->left);
  if(root->right != NULL) freeAST(root->right);

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

    yyin = fopen("input.txt", "r");
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

    
    // Walk the AST
    walkAST(root);

    ht_destroy(table);
    freeAST(root);
    return 0;
}