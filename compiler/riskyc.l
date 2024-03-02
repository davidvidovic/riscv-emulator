%{
    #include "y.tab.h"
    int countn = 0;
%}

%option yylineno

alpha [a-zA-Z]
digit [0-9]

%%

"int"                       { return INT; }
"float"                     { return FLOAT; }
"char"                      { return CHAR; }
"void"                      { return VOID; }
"return"                    { return RETURN; }
"="                         { return EQUAL; }
[-]?{digit}+                { return NUMBER; }
[-]?{digit}+\.{digit}{1,6}  { return FLOAT_NUM; }
{alpha}({alpha}|{digit})*   { return ID; }

[ \t]*                      { ; }
[\n]                        { countn++; }

["].*["]                    { return STR; }
['].[']                     { return CHARACTER; }

%%

int yywrap() {
    return 1;
}
