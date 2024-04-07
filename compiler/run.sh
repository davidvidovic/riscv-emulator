#! /usr/bin/bash

yacc -d riskyc.y 
lex riskyc.l
gcc y.tab.c lex.yy.c symboltable.c ast.c riskyc.c -ll -lm -w
./a.out
