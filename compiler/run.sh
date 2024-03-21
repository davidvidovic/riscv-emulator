#! /usr/bin/bash

yacc -d riskyc.y -Wcounterexamples
lex riskyc.l
gcc y.tab.c lex.yy.c symboltable.c -ll -lm -w
./a.out
