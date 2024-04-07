%{
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include "riskyc.h"
    #include "y.tab.h"
    #include "symboltable.h"
    #include "ast.h"

    extern int lineno;
    extern ASTnode *root;
    int main_counter = 0;

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

    // AST stuff
    ASTnode* ast;
    operation_type op;
}

%token RETURN MAIN STAR COMMA NEWLINE SEMICOLON 
%token EQUAL PLUS MINUS DIVIDE LESS_THAN GREATER_THAN LESS_EQ_THAN GREATER_EQ_THAN EQUAL_TRUTH MOD
%token CURLY_OPEN CURLY_CLOSED BRACKET_OPEN BRACKET_CLOSED
%token LOGIC_AND LOGIC_OR NOT_EQUAL BITWISE_NOT BITWISE_AND BITWISE_OR
%token IF ELSE ELSE_IF FOR WHILE
%token <value_string> TYPE_INT TYPE_FLOAT TYPE_CHAR TYPE_VOID
%token <value_int> INT
%token <value_float> FLOAT
%token <value_char> CHARACTER
%token <id_obj> ID
%token <value_string> STRING
%token EOF_TOKEN

%type <id_obj> id
%type <value_string> datatype
%type <op> arith_operator
%type <ast> declaration
%type <ast> value arith_statement


%start program

%%

/* Grammar section */


/* IF STATEMENT */

if_body: if_body else_statement
| if_statement scope
;

else_statement: ELSE scope
| ELSE if_statement scope
;

if_statement: IF BRACKET_OPEN logic_expression BRACKET_CLOSED
;

/* FOR LOOP */

for_body: FOR for_statement scope
;

for_statement: BRACKET_OPEN statement SEMICOLON logic_expression SEMICOLON statement BRACKET_CLOSED
;

/* WHILE LOOP */

while_body: while_statement scope
;

while_statement: WHILE BRACKET_OPEN logic_expression BRACKET_CLOSED
;


/* PROGRAM */

program: program function
| function
;


function: datatype id params scope 
| datatype MAIN params scope 
  {
    if(main_counter != 0)
    {
      yyerror("ERROR\tMore than one main declared.\n");
      exit(1);
    }
    main_counter++;
  }
;


params: BRACKET_OPEN BRACKET_CLOSED
| BRACKET_OPEN declaration BRACKET_CLOSED
;


scope: CURLY_OPEN scope_body CURLY_CLOSED
;

scope_body: scope_body return
| return
| statements
;


return: RETURN value SEMICOLON         // ne mora da bude samo value
;


statements : statements full_statement
| statements if_body
| statements for_body
| statements while_body
| full_statement
| if_body
| for_body
| while_body
;

full_statement: statement SEMICOLON
;

statement: declaration assign_value 
| assign_value 
| declaration assign_expression   
| assign_expression                 
| declaration                
;


declaration: declaration COMMA id    
  {
    declare($3.name, $1, $3.src.line);
  }  
| datatype id 
  {
    if(declare($2.name, $1, $2.src.line))
      $$ = $1;
  }
;


assign_expression: EQUAL arith_expression     
| id EQUAL arith_expression   
  {
    check_declaration($1.name);
  }
;


assign_value: EQUAL value      
| id EQUAL value        
  {
    check_declaration($1.name);
  }
| id EQUAL id       
  {
    check_declaration($1.name);
    check_declaration($3.name);
  }
;

arith_expression: arith_operator arith_expression
| arith_statement
;



arith_statement: arith_statement arith_operator value
  {
    if($2 == MUL_OP)
    {
      $$ = new_ASTnode_ARITH_OPERATION($2, $3, root->right);
      root->right = $$;
    }
    else
    {
      $$ = new_ASTnode_ARITH_OPERATION($2, $3, $1);
      root = $$;
    }
  }
| arith_statement arith_operator id
  {
    check_declaration($3.name);
  }
| value arith_operator value 
  {
    $$ = new_ASTnode_ARITH_OPERATION($2, $1, $3);   
    if($2 == ADD_OP) root = $$;
  } 
| value arith_operator id               
  {
    check_declaration($3.name);
  }
| id arith_operator value               
  {
    check_declaration($1.name);
  }
| id arith_operator id                  
  {
    check_declaration($1.name);
    check_declaration($3.name);
  }
; 


logic_expression: logic_operator logic_expression
| logic_statement
;


logic_statement: id logic_operator id 
  {
    check_declaration($1.name);
    check_declaration($3.name);
  }
| id logic_operator value
  {
    check_declaration($1.name);
  }
| value logic_operator id
  {
    check_declaration($3.name);
  }
| value logic_operator value
;


logic_operator: EQUAL_TRUTH 
| GREATER_THAN
| LESS_THAN
| GREATER_EQ_THAN
| LESS_EQ_THAN
| NOT_EQUAL
;

arith_operator:  PLUS { $$ = ADD_OP; }
| MINUS               { $$ = SUB_OP; }
| DIVIDE              { $$ = DIV_OP; }
| STAR                { $$ = MUL_OP; }
;

datatype: TYPE_INT      
| TYPE_FLOAT            
| TYPE_CHAR             
| TYPE_CHAR STAR        
| TYPE_VOID             
;

value: INT   { $$ = new_ASTnode_VALUE($1); }           
| FLOAT      { $$ = new_ASTnode_VALUE($1); }              
| CHARACTER    { $$ = new_ASTnode_VALUE($1); }            
| STRING          { $$ = new_ASTnode_VALUE($1); }         
;

id: ID                  
;


%%


void yyerror(const char* msg) {
    fprintf(stderr, "%s at line %d\n", msg, lineno);
}
