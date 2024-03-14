//File main.c
#include <stdio.h>
#include <stdlib.h>
#include "riskyc.h"
#include "y.tab.h"

// The current line number.
// This is from tokenizer.l.
extern int lineno;
extern FILE *yyin;
// Current token's lexeme
extern char *yytext;
// The source file name
extern char *currentFileName;
// From lex.yy.c, returns the next token.
// Ends with EOF
int yylex();

int main()
{
  yyin = fopen("input.txt", "r");
  int token;
  //yyparse();
  while ((token = yylex()) != EOF_TOKEN)
  {
    printf("Line %d\tToken: %d: '%s'\n", lineno, token, yytext);
  }
  return 0;
}
