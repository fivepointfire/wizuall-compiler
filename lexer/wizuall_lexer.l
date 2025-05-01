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
NUMBER     {DIGIT}+\.{DIGIT}+|{DIGIT}+
STRING     \"([^\"\\]|\\.)*\"

%%

\r\n            { yylineno++; yycolumn = 1; }
\n              { yylineno++; yycolumn = 1; }
\r              { yylineno++; yycolumn = 1; }
[ \t]+          { yycolumn += yyleng; }

"if"            { yylval.str = strdup(yytext); yycolumn += yyleng; return IF; }
"else"          { yylval.str = strdup(yytext); yycolumn += yyleng; return ELSE; }
"while"         { yylval.str = strdup(yytext); yycolumn += yyleng; return WHILE; }
"for"           { yylval.str = strdup(yytext); yycolumn += yyleng; return FOR; }
"BEGIN_AUX"     { yylval.str = strdup(yytext); yycolumn += yyleng; return BEGIN_AUX; }
"END_AUX"       { yylval.str = strdup(yytext); yycolumn += yyleng; return END_AUX; }
"import"        { yylval.str = strdup(yytext); yycolumn += yyleng; return IMPORT; }

"plot"          { yylval.str = strdup(yytext); yycolumn += yyleng; return PLOT; }
"histogram"     { yylval.str = strdup(yytext); yycolumn += yyleng; return HISTOGRAM; }
"heatmap"       { yylval.str = strdup(yytext); yycolumn += yyleng; return HEATMAP; }
"barchart"      { yylval.str = strdup(yytext); yycolumn += yyleng; return BARCHART; }
"piechart"      { yylval.str = strdup(yytext); yycolumn += yyleng; return PIECHART; }
"scatter"       { yylval.str = strdup(yytext); yycolumn += yyleng; return SCATTER; }
"boxplot"       { yylval.str = strdup(yytext); yycolumn += yyleng; return BOXPLOT; }

{ID}            { yylval.str = strdup(yytext); yycolumn += yyleng; return ID; }
{NUMBER}        { yylval.num = atof(yytext); yycolumn += yyleng; return NUMBER; }
{STRING}        {
    int i;
    for (i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            yylineno++;
            yycolumn = 1;
        } else if (yytext[i] == '\r') {
            if (i+1 < yyleng && yytext[i+1] == '\n') {
                i++;
            }
            yylineno++;
            yycolumn = 1;
        } else {
            yycolumn++;
        }
    }
    yylval.str = strdup(yytext);
    return STRING;
}

"+"             { yylval.str = strdup(yytext); yycolumn += yyleng; return PLUS; }
"-"             { yylval.str = strdup(yytext); yycolumn += yyleng; return MINUS; }
"*"             { yylval.str = strdup(yytext); yycolumn += yyleng; return TIMES; }
"/"             { yylval.str = strdup(yytext); yycolumn += yyleng; return DIVIDE; }
"="             { yylval.str = strdup(yytext); yycolumn += yyleng; return ASSIGN; }
","             { yylval.str = strdup(yytext); yycolumn += yyleng; return COMMA; }
";"             { yylval.str = strdup(yytext); yycolumn += yyleng; return SEMICOLON; }
"("             { yylval.str = strdup(yytext); yycolumn += yyleng; return LPAREN; }
")"             { yylval.str = strdup(yytext); yycolumn += yyleng; return RPAREN; }
"{"             { yylval.str = strdup(yytext); yycolumn += yyleng; return LBRACE; }
"}"             { yylval.str = strdup(yytext); yycolumn += yyleng; return RBRACE; }
"["             { yylval.str = strdup(yytext); yycolumn += yyleng; return LBRACKET; }
"]"             { yylval.str = strdup(yytext); yycolumn += yyleng; return RBRACKET; }
"<"             { yylval.str = strdup(yytext); yycolumn += yyleng; return LT; }
">"             { yylval.str = strdup(yytext); yycolumn += yyleng; return GT; }

[ \t\n\r]+      ;         
"//".*          {
    int i;
    for (i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            yylineno++;
            yycolumn = 1;
        } else if (yytext[i] == '\r') {
            if (i+1 < yyleng && yytext[i+1] == '\n') {
                i++;
            }
            yylineno++;
            yycolumn = 1;
        } else {
            yycolumn++;
        }
    }
    /* skip comment */
}

.               {
    int i;
    for (i = 0; i < yyleng; i++) {
        if (yytext[i] == '\n') {
            yylineno++;
            yycolumn = 1;
        } else if (yytext[i] == '\r') {
            if (i+1 < yyleng && yytext[i+1] == '\n') {
                i++;
            }
            yylineno++;
            yycolumn = 1;
        } else {
            yycolumn++;
        }
    }
    /* Skip any unknown single characters */
}

%%

int yywrap() {
    return 1;
}   
