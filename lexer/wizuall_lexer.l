%{
#include "../wizuall_parser.tab.h"

#include <string.h>
#include <stdlib.h>

// Declare yylval properly
extern YYSTYPE yylval;
%}

DIGIT      [0-9]
LETTER     [a-zA-Z_]
ID         {LETTER}({LETTER}|{DIGIT})*
NUMBER     {DIGIT}+(\.{DIGIT}+)?
STRING     \"([^\"\\]|\\.)*\"

%%


"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"for"           { return FOR; }
"BEGIN_AUX"     { return BEGIN_AUX; }
"END_AUX"       { return END_AUX; }


"sort"          { return SORT; }
"reverse"       { return REVERSE; }
"slice"         { return SLICE; }
"avg"           { return AVG; }
"transpose"     { return TRANSPOSE; }
"runningSum"    { return RUNNING_SUM; }
"pairwiseCompare" { return PAIRWISE_COMPARE; }
"paretoSet"     { return PARETO_SET; }


"plot"          { return PLOT; }
"histogram"     { return HISTOGRAM; }
"heatmap"       { return HEATMAP; }
"barchart"      { return BARCHART; }
"piechart"      { return PIECHART; }
"scatter"       { return SCATTER; }
"boxplot"       { return BOXPLOT; }
"timeline"      { return TIMELINE; }


{ID}            { yylval.str = strdup(yytext); return ID; }
{NUMBER}        { yylval.num = atof(yytext); return NUMBER; }
{STRING}        { yylval.str = strdup(yytext); return STRING; }


"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"="             { return ASSIGN; }
","             { return COMMA; }
";"             { return SEMICOLON; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
"<"             { return LT; }
">"             { return GT; }



[ \t\r\n]+      ;         
"//".*          ;          


.               { /* Skip any unknown single characters */ }

%%

int yywrap() {
    return 1;
}   
