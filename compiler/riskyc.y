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
%token EQUAL PLUS MINUS DIVIDE LESS_THAN GREATER_THAN LESS_EQ_THAN GREATER_EQ_THAN EQUAL_TRUTH
%token CURLY_OPEN CURLY_CLOSED BRACKET_OPEN BRACKET_CLOSED
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

program: datatype MAIN BRACKET_OPEN BRACKET_CLOSED CURLY_OPEN statements return CURLY_CLOSED {}
;

return: RETURN value SEMICOLON         {}
;

statements : statements statement
| statement
;

statement: declaration init SEMICOLON   {}
| init SEMICOLON                        {}
| declaration SEMICOLON                 {}
;

declaration: declaration COMMA id       {}
| datatype id {
    if(ht_get_key(table, $2.name) != NULL) 
    {
      char error[100];
      strcpy(error, "\033[31mERROR \t\tIdentifier ");
      strcat(error, $2.name);
      strcat(error, " already declared.\033[0m");
      //yyerror(error);

      strcat(error, "\nPrevious declaration at line ");
      int error_line = ht_get_line(table, $2.name);
      char temp[10];
      sprintf(temp, "%d", error_line);
      strcat(error, temp);
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

init: EQUAL value       {}
| id EQUAL value        {
    if(ht_get_key(table, $1.name) == NULL) 
    {
      char error[100];
      strcpy(error, "\033[31mERROR \t\tIdentifier ");
      strcat(error, $1.name);
      strcat(error, " not declared.\033[0m");
      //yyerror(error);

      strcat(error, "\nError on line ");
      char temp[10];
      sprintf(temp, "%d", lineno);
      strcat(error, temp);
      yyerror(error);

      exit(1);
    }
  }
| EQUAL expression      {}
| id EQUAL expression   {
  if(ht_get_key(table, $1.name) == NULL) 
    {
      char error[100];
      strcpy(error, "\033[31mERROR \t\tIdentifier ");
      strcat(error, $1.name);
      strcat(error, " not declared.\033[0m");
      //yyerror(error);

      strcat(error, "\nError on line ");
      char temp[10];
      sprintf(temp, "%d", lineno);
      strcat(error, temp);
      yyerror(error);

      exit(1);
    }
}
;

expression: value arith_operator value  //{ printf("Expression 1\n"); }
| value arith_operator id               //{ printf("Expression 2\n"); }
| id arith_operator value               //{ printf("Expression 3\n"); }
| id arith_operator id                  //{ printf("Expression 4\n"); }
;

arith_operator:  PLUS
| MINUS
| DIVIDE
| STAR
;

datatype: TYPE_INT      {}
| TYPE_FLOAT            {}
| TYPE_CHAR             {}
| TYPE_CHAR STAR        {}
| TYPE_VOID             {}
;

value: INT              {}
| FLOAT                 {}
| CHARACTER             {}
// | ID                    {}
| STRING                {}
;

id: ID                  {}
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
    fprintf(stderr, "%s\n", msg);
}
