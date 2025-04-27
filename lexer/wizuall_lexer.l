%{
#include "../wizuall_parser.tab.h"

#include <string.h>
#include <stdlib.h>

// Declare yylval properly
extern YYSTYPE yylval;

// Add line and column tracking
int yycolumn = 1;
%}

DIGIT      [0-9]
LETTER     [a-zA-Z_]
ID         {LETTER}({LETTER}|{DIGIT})*
NUMBER     {DIGIT}+(\.{DIGIT}+)?
STRING     \"([^\"\\]|\\.)*\"

%%

\n              { yylineno++; yycolumn = 1; }
[ \t\r]+        { yycolumn += yyleng; }

"if"            { yycolumn += yyleng; return IF; }
"else"          { yycolumn += yyleng; return ELSE; }
"while"         { yycolumn += yyleng; return WHILE; }
"for"           { yycolumn += yyleng; return FOR; }
"BEGIN_AUX"     { yycolumn += yyleng; return BEGIN_AUX; }
"END_AUX"       { yycolumn += yyleng; return END_AUX; }


"plot"          { return PLOT; }
"histogram"     { return HISTOGRAM; }
"heatmap"       { return HEATMAP; }
"barchart"      { return BARCHART; }
"piechart"      { return PIECHART; }
"scatter"       { return SCATTER; }
"boxplot"       { return BOXPLOT; }
"timeline"      { return TIMELINE; }

{ID}            { yylval.str = strdup(yytext); yycolumn += yyleng; return ID; }
{NUMBER}        { yylval.num = atof(yytext); yycolumn += yyleng; return NUMBER; }
{STRING}        { yylval.str = strdup(yytext); yycolumn += yyleng; return STRING; }

"+"             { yycolumn += yyleng; return PLUS; }
"-"             { yycolumn += yyleng; return MINUS; }
"*"             { yycolumn += yyleng; return TIMES; }
"/"             { yycolumn += yyleng; return DIVIDE; }
"="             { yycolumn += yyleng; return ASSIGN; }
","             { yycolumn += yyleng; return COMMA; }
";"             { yycolumn += yyleng; return SEMICOLON; }
"("             { yycolumn += yyleng; return LPAREN; }
")"             { yycolumn += yyleng; return RPAREN; }
"{"             { yycolumn += yyleng; return LBRACE; }
"}"             { yycolumn += yyleng; return RBRACE; }
"["             { yycolumn += yyleng; return LBRACKET; }
"]"             { yycolumn += yyleng; return RBRACKET; }
"<"             { yycolumn += yyleng; return LT; }
">"             { yycolumn += yyleng; return GT; }

[ \t\r\n]+      ;         
"//".*          ;          

.               { yycolumn += yyleng; /* Skip any unknown single characters */ }

%%

int yywrap() {
    return 1;
}   
