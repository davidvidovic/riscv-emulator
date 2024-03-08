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
    char* value_string;
    ID_struct id_obj;
}

%token RETURN EQUAL MAIN STAR COMMA
%token TYPE_INT TYPE_FLOAT TYPE_CHAR TYPE_VOID
%token <value_int> INT
%token <value_float> FLOAT
%token <value_char> CHARACTER
%token <id_obj> ID
%token <value_string> STRING
%token EOF_TOKEN

%type <id_obj> id

%%

/* Grammar section */

statement: declaration init     {printf("Statement\n");}
;

declaration: declaration COMMA id    {printf("Declaration of IDs, name: %s\n", $3.name);}
| datatype id    {printf("Declaration of ID, name: %s\n", $2.name);}
;

init: EQUAL value       {printf("Initialized to value\n");}
| id EQUAL value        {printf("Initialized to value\n");}
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

id: ID                  { printf("ID\n"); }
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
