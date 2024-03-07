%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "symbol.h"

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
    IDstruct id;
}

%token RETURN STR EQUAL
%token TYPE_INT TYPE_FLOAT TYPE_CHAR TYPE_VOID
%token <value_int> INT
%token <value_float> FLOAT
%token <value_char> CHARACTER
%token <id> ID
%token EOF_TOKEN

%%

/* Grammar section */

statement: declaration init     {printf("Statement\n");}
;

declaration: datatype ID        {printf("Declaration of ID, name: %s\n", $2.name);}
;

init: EQUAL value        {printf("Initialized to value\n");}
| id_type EQUAL value         {printf("Initialized to value\n");}
;

datatype: TYPE_INT       {printf("Data type: INT\n");}
| TYPE_FLOAT             {printf("Data type: FLOAT\n");}
| TYPE_CHAR              {printf("Data type: CHAR\n");}
| TYPE_VOID              {printf("Data type: VOID\n");}
;

value: INT               {printf("Value type: INT, value: %d\n", $1);}
| FLOAT                  {printf("Value type: FLOAT, value: %f\n", $1);}
| CHARACTER              {printf("Value type: CHARACTER, value: %c\n", $1);}
| ID                     {printf("Value type: ID\n");}
;

id_type: ID             { printf("Found ID\n");}
;

%%

/*
int main() {
    yyparse();
}
*/

void yyerror(const char* msg) {
    fprintf(stderr, "%s\n", msg);
}
