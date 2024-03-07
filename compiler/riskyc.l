%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <math.h>
    #include "symbol.h"
    #include "y.tab.h"


    #define MAX_STR_CONST   1000
    #define YY_USER_ACTION col += yyleng;

    extern char *currentFileName;
    int col = 1;
    int lineno = 1;

%}


alpha [a-zA-Z]
digit [0-9]
nonZero [1-9]

%%

"int"                       { return TYPE_INT; }
"float"                     { return TYPE_FLOAT; }
"char"                      { return TYPE_CHAR; }
"void"                      { return TYPE_VOID; }
"return"                    { return RETURN; }
"="                         { return EQUAL; }

  /* Integers */

0                                           { yylval.value_int = 0; return INT; }
{nonZero}({digit})*([eE][+-]?[0-9]+)?       { yylval.value_int = (int)round(atof(yytext)); return INT; }


  /* Floats */

{nonZero}({digit})*"."({digit})*([eE][+-]?[0-9]+)?   { yylval.value_float = atof(yytext); return FLOAT; }


  /* Identifiers */

{alpha}({alpha}|{digit})*   {
    yylval.id.name = malloc(strlen(yytext)+1);
    yylval.id.src.line = lineno;
    yylval.id.src.col = col - yyleng;
    strcpy(yylval.id.name, yytext);
    return ID;
}


  /* Characters */

"\'"({alpha}|{digit})"\'"       { yylval.value_char = yytext[1]; return CHARACTER; }


[ \t]*                      { ; }
[\n]                        { ; }
[;,:()]                        { return *yytext; }
.                           { return EOF_TOKEN; }


%%

int yywrap() {
    return 1;
}
