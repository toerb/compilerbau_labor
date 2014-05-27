/* 
 * scanner.l - Scanner utility for the DHBW compiler
 */

%{
	#include "symboltabelle.h"
	#include "parser.y.h"
%}

/* 
 * Default setting for flex which indicates that yytext, which is one of the main connections
 * between the scanner and parser, is handled as pointer of char instead of an array of char. 
*/
%pointer	

/* save the line number of terminals */
%option yylineno

/* finish after scanning the provided file */
%option noyywrap		

%%
\/\/[[:print:]]{-}[\n]*[\n]
do { return DO; }
while { return WHILE; }
if { return IF; }
else { return ELSE; }
int { return INT; }
void { return VOID; }
return { return RETURN; }
: { return COLON; }
, { return COMMA; }
; { return SEMICOLON; }
\{ { return BRACE_OPEN; }
\} { return BRACE_CLOSE; }
\[ { return BRACKET_OPEN; }
\] { return BRACKET_CLOSE; }
\( { return PARA_OPEN; }
\) { return PARA_CLOSE; }
0|[1-9][0-9]* { sscanf(yytext, "%d", &yylval.i); return NUM; }
[_a-zA-Z][_a-zA-Z0-9]* { yylval.id = strdup(yytext); return ID; }
\= { return ASSIGN; }
\|\| { return LOGICAL_OR; }
\&\& { return LOGICAL_AND; }
\=\= { return EQ; }
\!\= { return NE; }
\< { return LS; }
\<= { return LSEQ; }
\>= { return GTEQ; }
\> { return GT; }
\<\< { return SHIFT_LEFT; }
\>\> { return SHIFT_RIGHT; }
\+ { return PLUS; }
\- { return MINUS; }
\* { return MUL; }
\\ { return DIV; }
\% { return MOD; }
\! { return LOGICAL_NOT; }
. { }
[\n] { }
