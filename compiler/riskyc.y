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
	extern int sp_offset;
    int multiline_declaration_cnt = 0;
	ASTnode* set_right_init_to_null(ASTnode *root);
	int calculate_sp_offset(int sp_offset, id_type type);

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
%type <ast> selection_statement
%type <ast> iteration_statement

%type <op> assignment_operator
%type <op> unary_operator
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
	| INC_OP unary_expression {
		ASTnode *add_node = new_ASTnode_OPERATION(ADD_OP, $2, new_ASTnode_INT(1)); // possible bug for other data types
		$$ = new_ASTnode_OPERATION(EQU_OP, $2, add_node);	
	} 
	| DEC_OP unary_expression {
		ASTnode *add_node = new_ASTnode_OPERATION(SUB_OP, $2, new_ASTnode_INT(1)); // possible bug for other data types
		$$ = new_ASTnode_OPERATION(EQU_OP, $2, add_node);	
	} 
	| unary_operator cast_expression {$$ = new_ASTnode_OPERATION($1, $2, NULL);}
	| SIZEOF unary_expression {}
	| SIZEOF '(' type_name ')' {}
	;

unary_operator
	: '&' {}
	| '*' {}
	| '+' {}
	| '-' {}
	| '~' {$$ = BITWISE_NOT_OP;}
	| '!' {$$ = LOGIC_NOT_OP;}
	;

cast_expression
	: unary_expression {$$ = $1;}
	| '(' type_name ')' cast_expression {}
	;

multiplicative_expression
	: cast_expression {$$ = $1;}
	| multiplicative_expression '*' cast_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(MUL_OP, $3, $1);}
	| multiplicative_expression '/' cast_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(DIV_OP, $3, $1);}
	| multiplicative_expression '%' cast_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(MOD_OP, $3, $1);}
	;

additive_expression
	: multiplicative_expression {$$ = $1;}
	| additive_expression '+' multiplicative_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(ADD_OP, $1, $3); }
	| additive_expression '-' multiplicative_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(SUB_OP, $1, $3);}
	;

shift_expression
	: additive_expression {$$ = $1;}
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression {$$ = $1;}
	| relational_expression '<' shift_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(LOGIC_LT_OP, $3, $1);}
	| relational_expression '>' shift_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(LOGIC_GT_OP, $3, $1);}
	| relational_expression LE_OP shift_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(LOGIC_LET_OP, $3, $1);}
	| relational_expression GE_OP shift_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(LOGIC_GET_OP, $3, $1);}
	;

equality_expression
	: relational_expression {$$ = $1;}
	| equality_expression EQ_OP relational_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(LOGIC_EQU_OP, $3, $1);}
	| equality_expression NE_OP relational_expression {type_check($1, $3); $$ = new_ASTnode_OPERATION(LOGIC_NEQU_OP, $3, $1);}
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
	| logical_and_expression AND_OP inclusive_or_expression {
		type_check($1, $3);
		if($1->operation == LOGIC_AND_OP)
		{
			type_check($1->left, $3);
			$$ = new_ASTnode_OPERATION(LOGIC_AND_OP, $3, $1);
		}
		else
		{
			type_check($1, $3);
			$$ = new_ASTnode_OPERATION(LOGIC_AND_OP, $1, $3);
		}
	}
	;

logical_or_expression
	: logical_and_expression {$$ = $1;}
	| logical_or_expression OR_OP logical_and_expression {
		
		if($1->operation == LOGIC_OR_OP || $1->operation == LOGIC_AND_OP)
		{
			type_check($1->left, $3);
			$$ = new_ASTnode_OPERATION(LOGIC_OR_OP, $3, $1);
		}
		else
		{
			type_check($1, $3);
			$$ = new_ASTnode_OPERATION(LOGIC_OR_OP, $1, $3);
		}
	}
	;

conditional_expression
	: logical_or_expression {$$ = $1;}
	| logical_or_expression '?' expression ':' conditional_expression
	;

assignment_expression
	: conditional_expression {$$ = $1;}
	| unary_expression assignment_operator assignment_expression {
		/* Type check */
		if($3->nodetype == OPERATION_NODE)
		{
			type_check($1, $3->left);
		}
		else
		{
			type_check($1, $3);
		}
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
	| declaration_specifiers init_declarator_list ';' {
		$$ = $2; 
		if($$->nodetype == ID_NODE)
		{
			$$->type = $1;
			sp_offset = calculate_sp_offset(sp_offset, $1);
			ht_set_type_sp_offset($$->name, $1, sp_offset);
		}
		else if($$->nodetype == EXPRESSION_NODE)
		{
			$$->left->left->type = $1;
			sp_offset = calculate_sp_offset(sp_offset, $1);
			ht_set_type_sp_offset($$->left->left->name, $1, sp_offset);
		}
		
		/* 
		* Multi-declarations in one line are hard to keep track of data type 
		* This is the work around:
		* multiline_declaration_cnt is a counter that increments each time an identifier is found
		* and it resets when single-declaration of id is found, else program gets to here and variable holds information of how many ID ASTnodes
		* are to the right of a current ASTnode (root) that need to have the same type
		* Simply loop thorugh all of them and finally cut the right hand side of tree with NULL (no need for ID nodes floating in the air)
		*/
		if($$->right != NULL)
		{
			ASTnode *temp = $$->right;
			for(int i = 0; i < multiline_declaration_cnt; i++)
			{
				if(temp->nodetype == ID_NODE)
				{
					temp->type = $1;
					sp_offset = calculate_sp_offset(sp_offset, $1);
					ht_set_type_sp_offset(temp->name, $1, sp_offset);
				}
				else if(temp->nodetype == EXPRESSION_NODE)
				{
					temp->left->left->type = $1;
					sp_offset = calculate_sp_offset(sp_offset, $1);
					ht_set_type_sp_offset(temp->left->left->name, $1, sp_offset);
				}
				temp = temp->right;
			}
			multiline_declaration_cnt = 0;

			$$ = set_right_init_to_null($$);
			free(temp);
		}
		
	}
	;

declaration_specifiers
	: storage_class_specifier {}
	| storage_class_specifier declaration_specifiers {}
	| type_specifier {$$ = $1;}
	| type_specifier declaration_specifiers {$$ = $1;}
	| type_qualifier {}
	| type_qualifier declaration_specifiers {}
	;

init_declarator_list
	: init_declarator {
		$$ = $1; 
		multiline_declaration_cnt = 0;
	}
	| init_declarator_list ',' init_declarator {
		$$ = $3;
		$$->right = $1;
	}
	;

init_declarator
	: declarator {$$ = $1;}
	| declarator '=' initializer {
		ASTnode *temp = new_ASTnode_OPERATION(EQU_OP, $1, $3);
		$$ = new_ASTnode_EXPRESSION(temp, NULL);
	}
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
	| struct_or_union_specifier {}
	| enum_specifier {}
	| TYPE_NAME {}
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
		multiline_declaration_cnt++;
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
	| compound_statement {$$ = $1;}
	| expression_statement {$$ = $1;}
	| selection_statement {$$ = $1;}
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
	| '{' statement_list '}' {$$ = new_ASTnode_SCOPE($2, NULL);} 
	| '{' declaration_list '}' {$$ = $2;} 
	| '{' declaration_list statement_list '}' {	
		$$ = new_ASTnode_SCOPE($3, $2);
	}
	;

declaration_list
	: declaration {$$ = $1;}
	| declaration_list declaration {	
		if($2->nodetype == EXPRESSION_NODE)
		{
			$$ = $2;
			if($1->nodetype == EXPRESSION_NODE)
			{
				$$->right = $1;
			}
		}
		else if($1->nodetype == EXPRESSION_NODE)
		{
			$$ = $1;
		}
		else
		{
			$$ = $2;
		}
	}
	;

statement_list
	: statement {
		$$ = $1;
		if($$->nodetype == WHILE_NODE)
		{
			ASTnode *temp = new_ASTnode_LABEL(NULL, NULL);
			$$->right->right = temp;
			ASTnode *temp1 = new_ASTnode_LABEL(NULL, $$->right);
			$$->right = temp1;
		}
		else if($$->nodetype == FOR_NODE) 
		{	
			/* Inserting label node between init_exp and bool_exp in for statement */
			ASTnode *temp = new_ASTnode_LABEL(NULL, $$->right->right);
			$$->right->right = temp;
		}
	}
	| statement_list statement {
		$$ = $2; 
		if($$->nodetype == IF_NODE)
		{
			$$->right->right = $1;
		}
		else if($$->nodetype == ELSE_NODE)
		{
			$$->right->right->right = $1;
		}
		else if($$->nodetype == WHILE_NODE)
		{
			ASTnode *temp = new_ASTnode_LABEL(NULL, $1);
			$$->right->right = temp;
			ASTnode *temp1 = new_ASTnode_LABEL(NULL, $$->right);
			$$->right = temp1;
		}
		else if($$->nodetype == FOR_NODE)
		{
			$$->right->right->right = $1;
			/* Inserting label node between init_exp and bool_exp in for statement */
			ASTnode *temp = new_ASTnode_LABEL(NULL, $$->right->right);
			$$->right->right = temp;
		}
		else
			$$->right = $1;
		

		if($$->right != NULL)
		{
			if($$->right->nodetype == IF_NODE)
			{
				$$->value.label_count = $$->right->value.label_count;
				$$->right->right->value.label_count = $$->right->value.label_count;
			}
		}
	}
	;

expression_statement
	: ';' {}
	| expression ';' {$$ = $1;}
	;

selection_statement
	: IF '(' expression ')' statement {
		$$ = new_ASTnode_IF($5, $3);
	}
	| IF '(' expression ')' statement ELSE statement {
		ASTnode* temp = new_ASTnode_IF($5, $3);
		$$ = new_ASTnode_ELSE($7, temp);
	}
	| SWITCH '(' expression ')' statement {}
	;

iteration_statement
	: WHILE '(' expression ')' statement {
		$$ = new_ASTnode_WHILE($5, $3);
	}
	| DO statement WHILE '(' expression ')' ';' {}
	| FOR '(' expression_statement expression_statement ')' statement {
		$4->right = $3;
		$$ = new_ASTnode_FOR($6, $4);
	}
	| FOR '(' expression_statement expression_statement expression ')' statement {
		$4->right = $3;
		$5->right = $7;
		$$ = new_ASTnode_FOR($5, $4);
	}
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
	| declaration_specifiers declarator compound_statement {
		$2->type = $1; 
		$$ = new_ASTnode_FUNCTION($2, $3); 
		$$->type = $1;
	}
	| declarator declaration_list compound_statement {$$ = $3;}
	| declarator compound_statement {$$ = $2;}
	;


%%

void yyerror(const char* msg) {
    fprintf(stderr, "%s at line %d\n", msg, lineno);
}

ASTnode* set_right_init_to_null(ASTnode *root)
{
	ASTnode *temp = NULL;
	if(root->right != NULL)
		temp = set_right_init_to_null(root->right);

		
	if(temp != NULL && temp->nodetype == EXPRESSION_NODE)
	{
		if(root->nodetype == EXPRESSION_NODE)
		{
			root->right = temp;
			return root;
		}
		else
		{
			root->right = NULL;
			return temp;
		}
	}
	else
	{
		root->right = NULL;
	}
	
	return root;	
}

int calculate_sp_offset(int sp_offset, id_type type)
{
	switch(type)
	{
		case TYPE_INT:
			sp_offset += SIZE_INT;
		break;

		case TYPE_CHAR:
			sp_offset += SIZE_CHAR;
		break;

		case TYPE_FLOAT:
			sp_offset += SIZE_FLOAT;
		break;

		default:
			printf("[calculate_sp_offset]\tUnrecognized data type.");
		break;
	}

	return sp_offset;
}

