//File main.c
#include <stdio.h>
#include <stdlib.h>
#include "symbol.h"
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
  while ((token = yylex()) != EOF_TOKEN)
  {
    printf("Token: %d: '%s'\n", token, yytext);
  }
  return 0;
}
