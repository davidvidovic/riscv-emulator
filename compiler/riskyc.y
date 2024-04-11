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
	id_type ty;
}

%token <id_obj> IDENTIFIER 
%token <value_int> HEX_CONSTANT OCT_CONSTANT DEC_CONSTANT
%token <value_float> SCI_CONSTANT FLT_CONSTANT
%token <value_char> CHR_CONSTANT
%token <value_string> STRING_LITERAL
%token SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%start translation_unit

// AST
%type <ast> primary_expression 
%type <ast> postfix_expression 
%type <ast> unary_expression
%type <ast> cast_expression
%type <ast> multiplicative_expression
%type <ast> additive_expression
%type <ast> shift_expression
%type <ast> relational_expression
%type <ast> equality_expression
%type <ast> and_expression
%type <ast> exclusive_or_expression
%type <ast> inclusive_or_expression
%type <ast> logical_and_expression
%type <ast> logical_or_expression
%type <ast> conditional_expression
%type <ast> assignment_expression
%type <ast> expression
%type <ast> statement
%type <ast> compound_statement
%type <ast> statement_list
%type <ast> expression_statement
%type <ast> external_declaration
%type <ast> function_definition
%type <ast> translation_unit
%type <ast> direct_declarator 
%type <ast> declarator 
%type <ast> declaration_list
%type <ast> declaration
%type <ast> init_declarator
%type <ast> init_declarator_list
%type <ast> initializer
%type <ast> initializer_list

%type <op> assignment_operator
%type <ty> declaration_specifiers 
%type <ty> type_specifier


%%

primary_expression
	: IDENTIFIER {$$ = check_declaration($1.name);} 
	| HEX_CONSTANT {$$ = new_ASTnode_INT($1);}
  	| OCT_CONSTANT {$$ = new_ASTnode_INT($1);}
  	| DEC_CONSTANT {$$ = new_ASTnode_INT($1);}
  	| CHR_CONSTANT {$$ = new_ASTnode_CHAR($1);}
  	| SCI_CONSTANT {$$ = new_ASTnode_FLOAT($1);} // Should get looked at
  	| FLT_CONSTANT {$$ = new_ASTnode_FLOAT($1);}
	| STRING_LITERAL {$$ = new_ASTnode_INT($1);}
	| '(' expression ')' {}
	;

postfix_expression
	: primary_expression {$$ = $1;}
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression 
	: postfix_expression {$$ = $1;}
	| INC_OP unary_expression {}
	| DEC_OP unary_expression {}
	| unary_operator cast_expression {}
	| SIZEOF unary_expression {}
	| SIZEOF '(' type_name ')' {}
	;

unary_operator
	: '&' 
	| '*' 
	| '+' 
	| '-' 
	| '~' 
	| '!' 
	;

cast_expression
	: unary_expression {$$ = $1;}
	| '(' type_name ')' cast_expression {}
	;

multiplicative_expression
	: cast_expression {$$ = $1;}
	| multiplicative_expression '*' cast_expression {$$ = new_ASTnode_OPERATION(MUL_OP, $3, $1);}
	| multiplicative_expression '/' cast_expression {$$ = new_ASTnode_OPERATION(DIV_OP, $3, $1);}
	| multiplicative_expression '%' cast_expression {$$ = new_ASTnode_OPERATION(MOD_OP, $3, $1);}
	;

additive_expression
	: multiplicative_expression {$$ = $1;}
	| additive_expression '+' multiplicative_expression {$$ = new_ASTnode_OPERATION(ADD_OP, $3, $1); }
	| additive_expression '-' multiplicative_expression {$$ = new_ASTnode_OPERATION(SUB_OP, $3, $1);}
	;

shift_expression
	: additive_expression {$$ = $1;}
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression {$$ = $1;}
	| relational_expression '<' shift_expression {$$ = new_ASTnode_OPERATION(LOGIC_LT_OP, $3, $1);}
	| relational_expression '>' shift_expression {$$ = new_ASTnode_OPERATION(LOGIC_GT_OP, $3, $1);}
	| relational_expression LE_OP shift_expression {$$ = new_ASTnode_OPERATION(LOGIC_LET_OP, $3, $1);}
	| relational_expression GE_OP shift_expression {$$ = new_ASTnode_OPERATION(LOGIC_GET_OP, $3, $1);}
	;

equality_expression
	: relational_expression {$$ = $1;}
	| equality_expression EQ_OP relational_expression {$$ = new_ASTnode_OPERATION(LOGIC_EQU_OP, $3, $1);}
	| equality_expression NE_OP relational_expression {$$ = new_ASTnode_OPERATION(LOGIC_NEQU_OP, $3, $1);}
	;

and_expression
	: equality_expression {$$ = $1;}
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression {$$ = $1;}
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression {$$ = $1;}
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression {$$ = $1;}
	| logical_and_expression AND_OP inclusive_or_expression {$$ = new_ASTnode_OPERATION(LOGIC_AND_OP, $3, $1);}
	;

logical_or_expression
	: logical_and_expression {$$ = $1;}
	| logical_or_expression OR_OP logical_and_expression {$$ = new_ASTnode_OPERATION(LOGIC_OR_OP, $3, $1);}
	;

conditional_expression
	: logical_or_expression {$$ = $1;}
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression {$$ = $1;}
	| unary_expression assignment_operator assignment_expression {
		$$ = new_ASTnode_OPERATION($2, $1, $3); // maybe 3,1?
	}
	;

assignment_operator
	: '='	{$$ = EQU_OP;}
	| MUL_ASSIGN	{$$ = MUL_ASSIGN_OP;}
	| DIV_ASSIGN	{$$ = DIV_ASSIGN_OP;}
	| MOD_ASSIGN	{$$ = MOD_ASSIGN_OP;}
	| ADD_ASSIGN	{$$ = ADD_ASSIGN_OP;}
	| SUB_ASSIGN	{$$ = SUB_ASSIGN_OP;}
	| LEFT_ASSIGN	{$$ = LEFT_ASSIGN_OP;}
	| RIGHT_ASSIGN	{$$ = RIGHT_ASSIGN_OP;}
	| AND_ASSIGN	{$$ = AND_ASSIGN_OP;}
	| XOR_ASSIGN	{$$ = XOR_ASSIGN_OP;}
	| OR_ASSIGN		{$$ = OR_ASSIGN_OP;}
	;

expression
	: assignment_expression {
		$$ = new_ASTnode_EXPRESSION($1, NULL);
	}
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression 
	;

declaration
	: declaration_specifiers ';'  {}
	| declaration_specifiers init_declarator_list ';' {$$ = $2; $$->type = $1;}
	;

declaration_specifiers
	: storage_class_specifier
	| storage_class_specifier declaration_specifiers
	| type_specifier {$$ = $1;}
	| type_specifier declaration_specifiers {$$ = $1;}
	| type_qualifier 
	| type_qualifier declaration_specifiers
	;

init_declarator_list
	: init_declarator {$$ = $1;}
	| init_declarator_list ',' init_declarator {}
	;

init_declarator
	: declarator {$$ = $1;}
	| declarator '=' initializer {$$ = new_ASTnode_OPERATION(EQU_OP, $1, $3); }
	;

storage_class_specifier
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

type_specifier
	: VOID	{$$ = TYPE_VOID;}
	| CHAR	{$$ = TYPE_CHAR;}
	| SHORT	{$$ = TYPE_SHORT;}
	| INT	{$$ = TYPE_INT;}
	| LONG	{$$ = TYPE_LONG;}
	| FLOAT	{$$ = TYPE_FLOAT;}
	| DOUBLE	{$$ = TYPE_DOUBLE;}
	| SIGNED	{$$ = TYPE_SIGNED;}
	| UNSIGNED	{$$ = TYPE_UNSIGNED;}
	| struct_or_union_specifier
	| enum_specifier
	| TYPE_NAME
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	| type_specifier
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	| ':' constant_expression
	| declarator ':' constant_expression
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

type_qualifier
	: CONST
	| VOLATILE
	;

declarator
	: pointer direct_declarator {$$ = $2;};
	| direct_declarator {$$ = $1;}
	;

direct_declarator
	: IDENTIFIER {
		$$ = new_ASTnode_ID($1.name, NO_TYPE, NULL, NULL);
		declare($1.name, "NULL", lineno, $$);
	}
	| '(' declarator ')' {$$ = $2;}
	| direct_declarator '[' constant_expression ']'
	| direct_declarator '[' ']'
	| direct_declarator '(' parameter_type_list ')' 
	| direct_declarator '(' identifier_list ')'
	| direct_declarator '(' ')' {$$ = $1;}
	;

pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	| declaration_specifiers abstract_declarator
	| declaration_specifiers
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression {$$ = $1;}
	| '{' initializer_list '}' {}
	| '{' initializer_list ',' '}' {}
	;

initializer_list
	: initializer {$$ = $1;}
	| initializer_list ',' initializer
	;

statement
	: labeled_statement {}
	| compound_statement {}
	| expression_statement {$$ = $1;}
	| selection_statement {}
	| iteration_statement {}
	| jump_statement {}
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

compound_statement
	: '{' '}' {}
	| '{' statement_list '}' {$$ = new_ASTnode_SCOPE(NULL, $2);} 
	| '{' declaration_list '}' {$$ = $2;} 
	| '{' declaration_list statement_list '}' {$$ = new_ASTnode_SCOPE(NULL, $3);}
	;

declaration_list
	: declaration {$$ = $1; } // odradice se samo jednom za main, sve ostalo ispod?
	| declaration_list declaration {$$ = $2;}
	;

statement_list
	: statement {$$ = $1;}
	| statement_list statement {$2->right = $1; $$ = $2;}
	;

expression_statement
	: ';' {}
	| expression ';' {$$ = $1;}
	;

selection_statement
	: IF '(' expression ')' statement 
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

translation_unit
	: external_declaration {$$ = $1; root = $$;}
	| translation_unit external_declaration {}
	;

external_declaration
	: function_definition {$$ = $1;}
	| declaration {$$ = $1;}
	;

function_definition
	: declaration_specifiers declarator declaration_list compound_statement {$$ = $4;}
	| declaration_specifiers declarator compound_statement {$2->type = $1; $$ = new_ASTnode_FUNCTION($2, $3); $$->type = $1;}
	| declarator declaration_list compound_statement {$$ = $3;}
	| declarator compound_statement {$$ = $2;}
	;


%%

void yyerror(const char* msg) {
    fprintf(stderr, "%s at line %d\n", msg, lineno);
}
