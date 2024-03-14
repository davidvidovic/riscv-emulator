%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "riskyc.h"
    #include "y.tab.h"

    char *currentFileName;
    extern int lineno, col;

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

%token RETURN EQUAL MAIN STAR COMMA NEWLINE SEMICOLON CURLY_OPEN CURLY_CLOSED BRACKET_OPEN BRACKET_CLOSED
%token TYPE_INT TYPE_FLOAT TYPE_CHAR TYPE_VOID
%token <value_int> INT
%token <value_float> FLOAT
%token <value_char> CHARACTER
%token <id_obj> ID
%token <value_string> STRING
%token EOF_TOKEN

%type <id_obj> id

%start program

%%

/* Grammar section */

program: datatype MAIN BRACKET_OPEN BRACKET_CLOSED CURLY_OPEN statements return CURLY_CLOSED {printf("Program\n");}
;

return: RETURN value SEMICOLON         {printf("Return\n");} 
;

statements : statements statement
| statement
;

statement: declaration init SEMICOLON   {printf("Statement\n");}
| init SEMICOLON                        {printf("Assign value\n");}
| declaration SEMICOLON                 {printf("Declaration\n");}
;

declaration: declaration COMMA id     {printf("Declaration of IDs, name: %s\n", $3.name);}
| datatype id                         {printf("Declaration of ID, name: %s\n", $2.name);}
;


init: EQUAL value       {printf("Initialized to value\n");}
| id EQUAL value        {printf("Assigning to value\n");}
;

datatype: TYPE_INT      {printf("Data type: INT\n");}
| TYPE_FLOAT            {printf("Data type: FLOAT\n");}
| TYPE_CHAR             {printf("Data type: CHAR\n");}
| TYPE_CHAR STAR        {printf("Data type: STRING\n");}
| TYPE_VOID             {printf("Data type: VOID\n");}
;

value: INT              {printf("Value type: INT, value: %d\n", $1);}
| FLOAT                 {printf("Value type: FLOAT, value: %f\n", $1);}
| CHARACTER             {printf("Value type: CHARACTER, value: %c\n", $1);}
| ID                    {printf("Value type: ID, name: %s\n", $1.name);}
| STRING                {printf("Value type: STRING, value: %s\n", $1);}
;

id: ID                  { printf("ID, name %s\n", $1.name); }
;



%%

extern int lineno;
extern FILE *yyin;
extern char *yytext;

int main()
{
  yyin = fopen("input.txt", "r");
  int token;
  yyparse();
  /* while ((token = yylex()) != EOF_TOKEN)
   {
     printf("Line %d\tToken: %d: '%s'\n", lineno, token, yytext);
   }
  */
  return 0;
}

/*
int main() {
    yyparse();
}
*/

void yyerror(const char* msg) {
    fprintf(stderr, "%s\n", msg);
}
