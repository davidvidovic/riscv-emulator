%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <ctype.h>

    #include"lex.yy.c"

    void yyerror(const char *s);
    int yylex();
    int yywrap();
%}

%union {
    int value_int;
}

%token VOID CHARACTER INT FLOAT CHAR FLOAT_NUM ID RETURN STR EQUAL
%token <value_int> NUMBER

%%

/* Grammar section */

statement: declaration init     {printf("Statement: initialized ID of type to value\n");}
;

declaration: datatype ID        {printf("Declaration: declared ID of type\n");}
;

init: eq value {printf("Initialized to value\n");}
;

datatype: INT   {printf("Data type: INT\n");}
| FLOAT         {printf("Data type: FLOAT\n");}
| CHAR          {printf("Data type: CHAR\n");}
| VOID          {printf("Data type: VOID\n");}
;

value: NUMBER   {printf("Value type: INT, value: %d\n", $1);}
| FLOAT_NUM     {printf("Value type: FLOAT_NUM\n");}
| CHARACTER     {printf("Value type: CHARACTER\n");}
| ID            {printf("Value type: ID\n");}
;

eq: EQUAL
;



%%

int main() {
    yyparse();
}

void yyerror(const char* msg) {
    fprintf(stderr, "%s\n", msg);
}
