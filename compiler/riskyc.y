%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "riskyc.h"
    #include "y.tab.h"
    #include "symboltable.h"

    char *currentFileName;
    extern int lineno, col;
    ht* table;
    int main_counter = 0;

    void yyerror(const char *s);
    int yylex();
    // int yywrap();
%}

%union {
    int value_int;
    float value_float;
    char value_char;
    char* value_string;
    ID_struct id_obj;
}

%token RETURN MAIN STAR COMMA NEWLINE SEMICOLON 
%token EQUAL PLUS MINUS DIVIDE LESS_THAN GREATER_THAN LESS_EQ_THAN GREATER_EQ_THAN EQUAL_TRUTH MOD
%token CURLY_OPEN CURLY_CLOSED BRACKET_OPEN BRACKET_CLOSED
%token LOGIC_AND LOGIC_OR NOT_EQUAL BITWISE_NOT BITWISE_AND BITWISE_OR
%token IF ELSE ELSE_IF FOR WHILE
%token <value_string> TYPE_INT TYPE_FLOAT TYPE_CHAR TYPE_VOID
%token <value_int> INT
%token <value_float> FLOAT
%token <value_char> CHARACTER
%token <id_obj> ID
%token <value_string> STRING
%token EOF_TOKEN

%type <id_obj> id
%type <value_string> datatype

%start program

%%

/* Grammar section */


/* IF STATEMENT */

if_body: if_body else_statement
| if_statement scope
;

else_statement: ELSE scope
| ELSE if_statement scope
;

if_statement: IF BRACKET_OPEN logic_expression BRACKET_CLOSED
;

/* FOR LOOP */

for_body: FOR for_statement scope
;

for_statement: BRACKET_OPEN statement SEMICOLON logic_expression SEMICOLON statement BRACKET_CLOSED
;

/* WHILE LOOP */

while_body: while_statement scope
;

while_statement: WHILE BRACKET_OPEN logic_expression BRACKET_CLOSED
;


/* PROGRAM */

program: program function
| function
;


function: datatype id params scope 
| datatype MAIN params scope {
  if(main_counter != 0)
  {
    yyerror("ERROR\tMore than one main declared.\n");
    exit(1);
  }
  main_counter++;
}
;


params: BRACKET_OPEN BRACKET_CLOSED
| BRACKET_OPEN declaration BRACKET_CLOSED
;


scope: CURLY_OPEN scope_body CURLY_CLOSED
;

scope_body: scope_body return
| return
| statements
;


return: RETURN value SEMICOLON         // ne mora da bude samo value
;


statements : statements full_statement
| statements if_body
| statements for_body
| statements while_body
| full_statement
| if_body
| for_body
| while_body
;

full_statement: statement SEMICOLON
;

statement: declaration assign_value 
| assign_value 
| declaration assign_expression   
| assign_expression                 
| declaration                
;


declaration: declaration COMMA id      
| datatype id {
    if(ht_get_key(table, $2.name) != NULL) 
    {
      char error[100];
      strcpy(error, "\033[31mERROR \t\tIdentifier ");
      strcat(error, $2.name);
      strcat(error, " already declared.\033[0m");

      strcat(error, "\nPrevious declaration at line ");
      int error_line = ht_get_line(table, $2.name);
      char temp[10];
      sprintf(temp, "%d", error_line);
      strcat(error, temp);
      strcat(error, "\nError");
      yyerror(error);

      exit(1);
    }
    else
    {
      if(ht_set(table, $2.name, $1, $2.src.line) == NULL) 
      {
        exit_nomem();
      }
      else
      {
        //printf("Added to table %s with value %s\n", ht_get_key(table, $2.name), $1);
      }
    }
  }
;


assign_expression: EQUAL arith_expression     
| id EQUAL arith_expression   
  {
    check_declaration($1.name);
  }
;


assign_value: EQUAL value       {}
| id EQUAL value        {
    if(ht_get_key(table, $1.name) == NULL) 
    {
      char error[100];
      strcpy(error, "\033[31mERROR \t\tIdentifier ");
      strcat(error, $1.name);
      strcat(error, " not declared.\033[0m");
      yyerror(error);

      exit(1);
    }
  }
| id EQUAL id       
  {
    if(ht_get_key(table, $1.name) == NULL) 
    {
      char error[100];
      strcpy(error, "\033[31mERROR \t\tIdentifier ");
      strcat(error, $1.name);
      strcat(error, " not declared.\033[0m");
      yyerror(error);

      exit(1);
    }
    if(ht_get_key(table, $3.name) == NULL) 
    {
      char error[100];
      strcpy(error, "\033[31mERROR \t\tIdentifier ");
      strcat(error, $3.name);
      strcat(error, " not declared.\033[0m");
      yyerror(error);

      exit(1);
    }
  }
;


arith_expression: value arith_operator value  
| value arith_operator id               
  {
    check_declaration($3.name);
  }
| id arith_operator value               
  {
    check_declaration($1.name);
  }
| id arith_operator id                  
  {
    check_declaration($1.name);
    check_declaration($3.name);
  }
;


logic_expression: logic_operator logic_expression
| logic_statement
;


logic_statement: id logic_operator id 
 {
    check_declaration($1.name);
    check_declaration($3.name);
  }
| id logic_operator value
  {
    check_declaration($1.name);
  }
| value logic_operator id
  {
    check_declaration($3.name);
  }
| value logic_operator value
;


logic_operator: EQUAL_TRUTH 
| GREATER_THAN
| LESS_THAN
| GREATER_EQ_THAN
| LESS_EQ_THAN
| NOT_EQUAL
;

arith_operator:  PLUS
| MINUS
| DIVIDE
| STAR
;

datatype: TYPE_INT      
| TYPE_FLOAT            
| TYPE_CHAR             
| TYPE_CHAR STAR        
| TYPE_VOID             
;

value: INT              
| FLOAT                 
| CHARACTER             
| STRING                
;

id: ID                  
;





%%

extern int lineno;
extern FILE *yyin;
extern char *yytext;

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

int main()
{
  table = ht_create();
  if (table == NULL) {
      exit_nomem();
  }

  yyin = fopen("input.txt", "r");
  int token;
  yyparse();
   /* while ((token = yylex()) != EOF_TOKEN)
   {
     printf("Line %d\tToken: %d: '%s'\n", lineno, token, yytext);
   } */

    for(int i = 0; i < table->capacity; i++) {
      
      if(table->entries[i].key != NULL) 
      {
          printf("index %d:\t%s, ", i, table->entries[i].key);

          int* adr = table->entries[i].value;
          int off = 0;
          while(*adr != NULL) 
          {
            printf("%c", (char)*adr);
            adr = table->entries[i].value + (++off);
          }

          printf("\tat line %d", table->entries[i].line);
          printf("\n");
      } 
      else
      {
        //printf("Index %d: Empty\n", i);
      }

      
  }
  ht_destroy(table);
  return 0;
}


void yyerror(const char* msg) {
    fprintf(stderr, "%s at line %d\n", msg, lineno);
}
