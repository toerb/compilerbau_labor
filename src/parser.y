 /* 
 * parser.y - Parser utility for the DHBW compiler
 */
 
%{	
	// Project-specific includes
	#include "symboltabelle.h"
	#include "diag.h"
%}

%union {
  int i;
  char *id;
  variableDefinition *varDef;
  variableIdentifikation *varID;
  variablen *vliste;
  int fnct_typ=-1;
}
 
// Verbose error messages
%error-verbose

%locations
%start program

// Compiler in debugging information
%debug

// Enable debug output
%initial-action
{
	yydebug = 0;
};

/*
 * One shift/reduce conflict is expected for the "dangling-else" problem. This
 * conflict however is solved by the default behavior of bison for shift/reduce 
 * conflicts (shift action). The default behavior of bison corresponds to what
 * we want bison to do: SHIFT if the lookahead is 'ELSE' in order to bind the 'ELSE' to
 * the last open if-clause. 
 */
%expect 1

%token DO WHILE
%token IF ELSE
%token INT VOID
%token RETURN

%token COLON COMMA SEMICOLON

%token BRACE_OPEN BRACE_CLOSE
%token BRACKET_OPEN BRACKET_CLOSE
%token PARA_OPEN PARA_CLOSE

%token ID
%token NUM

%right ASSIGN 
%left LOGICAL_OR
%left LOGICAL_AND
%left EQ NE     
%left LS LSEQ GTEQ GT 
%left SHIFT_LEFT SHIFT_RIGHT
%left PLUS MINUS     
%left MUL DIV MOD
%right LOGICAL_NOT UNARY_MINUS UNARY_PLUS

%type <i> type expression NUM
%type <varDef> variable_declaration function_parameter stmt
%type <varID> identifier_declaration
%type <id> ID
%type <vliste> function_parameter_list stmt_list stmt_block
%%

program
     : program_element_list 
     ;

program_element_list
     : program_element_list program_element 
     | program_element 
     ;

program_element
     : variable_declaration SEMICOLON { integriereVariableGlobal($1); }
     | function_declaration SEMICOLON
     | function_definition
     | SEMICOLON
     ;
									
type
     : INT  { $$ = P_INT; }	
     | VOID { $$ = P_VOID; }
     ;

variable_declaration
     : variable_declaration COMMA identifier_declaration { $$ = erzeugeVariableDefinition($1,$1->typ,$3); } 
     | type identifier_declaration { $$ = erzeugeVariableDefinition(NULL,$1,$2); }
     ;

identifier_declaration
     : ID BRACKET_OPEN NUM BRACKET_CLOSE { $$ = erzeugeVariableIdentifikation($1,$3); }
     | ID { $$ = erzeugeVariableIdentifikation($1,0); } 
     ;

function_definition
     : type ID PARA_OPEN PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE { fnct_typ = $1; integriereFunktion($1,$2,NULL,$6,DEFINIERT); }
     | type ID PARA_OPEN function_parameter_list PARA_CLOSE BRACE_OPEN stmt_list BRACE_CLOSE { fnct_typ = $1; integriereFunktion($1,$2,$4,$7,DEFINIERT); }
     ;

function_declaration
     : type ID PARA_OPEN PARA_CLOSE { integriereFunktion($1,$2,NULL,NULL,UNDEFINIERT); }
     | type ID PARA_OPEN function_parameter_list PARA_CLOSE { integriereFunktion($1,$2,$4,NULL,UNDEFINIERT); }
     ;

function_parameter_list
     : function_parameter { $$ = integriereVariable($1,NULL); }
     | function_parameter_list COMMA function_parameter { $$ = integriereVariable($3,$1); }
     ;
	
function_parameter
     : type identifier_declaration { $$ = erzeugeVariableDefinition(NULL,$1,$2); }
     ;
									
stmt_list
     : /* empty: epsilon */ { $$ = NULL; }
     | stmt_list stmt { $$ = integriereVariable($2,$1); }
     ;

stmt
     : stmt_block { $$ = $1; }
     | variable_declaration SEMICOLON { $$ = $1; }
     | expression SEMICOLON { $$ = NULL; }
     | stmt_conditional { $$ = NULL; }
     | stmt_loop { $$ = NULL; }
     | RETURN expression SEMICOLON { $$ = NULL; }
     | RETURN SEMICOLON { $$ = NULL; }
     | SEMICOLON { $$ = NULL; } /* empty statement */
     ;

stmt_block
     : BRACE_OPEN stmt_list BRACE_CLOSE { $$ = $2; }
     ;
	
stmt_conditional
     : IF PARA_OPEN expression PARA_CLOSE stmt
     | IF PARA_OPEN expression PARA_CLOSE stmt ELSE stmt
     ;
									
stmt_loop
     : WHILE PARA_OPEN expression PARA_CLOSE stmt
     | DO stmt WHILE PARA_OPEN expression PARA_CLOSE SEMICOLON
     ;
									
expression
     : expression ASSIGN expression
     | expression LOGICAL_OR expression
     | expression LOGICAL_AND expression
     | LOGICAL_NOT expression
     | expression EQ expression
     | expression NE expression
     | expression LS expression 
     | expression LSEQ expression 
     | expression GTEQ expression 
     | expression GT expression
     | expression PLUS expression
     | expression MINUS expression
     | expression SHIFT_LEFT expression
     | expression SHIFT_RIGHT expression
     | expression MUL expression
     | expression DIV expression
     | expression MOD expression
     | MINUS expression %prec UNARY_MINUS
     | PLUS expression %prec UNARY_PLUS
     | ID BRACKET_OPEN primary BRACKET_CLOSE
     | PARA_OPEN expression PARA_CLOSE
     | function_call
     | primary
     ;

primary
     : NUM
     | ID
     ;

function_call
      : ID PARA_OPEN PARA_CLOSE
      | ID PARA_OPEN function_call_parameters PARA_CLOSE
      ;

function_call_parameters
     : function_call_parameters COMMA expression
     | expression
     ;

%%

void yyerror (const char *msg)
{
	FATAL_COMPILER_ERROR(INVALID_SYNTAX, 1, "(%d.%d-%d.%d): %s\n", yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column, msg);
}
