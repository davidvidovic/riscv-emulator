#! /usr/bin/bash

yacc -d riskyc.y
lex riskyc.l
gcc y.tab.c lex.yy.c symboltable.c ast.c riskyc.c ir.c control_flow.c register_allocation.c -ll -lm -w
rm lex.yy.c y.tab.c y.tab.h
./a.out
rm a.out
