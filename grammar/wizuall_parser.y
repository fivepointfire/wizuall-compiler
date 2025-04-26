%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// Forward declarations
int yylex();
void yyerror(const char *s);

%}

%union {
    int num;
    char* str;
    struct ASTNode* ast;
}

/* --- Token Declarations --- */

// Literals
%token <num> NUMBER
%token <str> STRING
%token <str> ID

// Keywords
%token IF ELSE WHILE FOR
%token BEGIN_AUX END_AUX

// Built-in Vector Functions
%token SORT REVERSE SLICE AVG TRANSPOSE RUNNING_SUM PAIRWISE_COMPARE PARETO_SET

// Visualization Functions
%token PLOT HISTOGRAM HEATMAP BARCHART PIECHART SCATTER BOXPLOT TIMELINE

// Operators and Punctuation
%token PLUS MINUS TIMES DIVIDE ASSIGN
%token COMMA SEMICOLON
%token LPAREN RPAREN
%token LBRACE RBRACE
%token LBRACKET RBRACKET

/* --- Operator Precedence Rules --- */
%left PLUS MINUS
%left TIMES DIVIDE
%left LBRACKET RBRACKET
%left LPAREN RPAREN

%%

/* --- Grammar Rules Start Here --- */

Program
    : StatementList
    ;

StatementList
    : Statement StatementList
    | Statement
    ;

Statement
    : Assignment
    | ControlStructure
    | FunctionCall
    | VisualizationCall
    | AuxBlock
    ;

Assignment
    : ID ASSIGN Expression
    ;

ControlStructure
    : IF LPAREN Expression RPAREN LBRACE StatementList RBRACE ELSE LBRACE StatementList RBRACE
    | WHILE LPAREN Expression RPAREN LBRACE StatementList RBRACE
    | FOR LPAREN Assignment SEMICOLON Expression SEMICOLON Assignment RPAREN LBRACE StatementList RBRACE
    ;

FunctionCall
    : ID LPAREN ArgListOpt RPAREN
    ;

VisualizationCall
    : PlotFunction LPAREN VizArgListOpt RPAREN
    ;

PlotFunction
    : PLOT | HISTOGRAM | HEATMAP | BARCHART | PIECHART | SCATTER | BOXPLOT | TIMELINE
    ;

AuxBlock
    : BEGIN_AUX AuxStatements END_AUX
    ;

AuxStatements
    : /* TODO: can extend to read raw text inside BEGIN_AUX */
    ;

Expression
    : Expression PLUS Term
    | Expression MINUS Term
    | Term
    ;

Term
    : Term TIMES Factor
    | Term DIVIDE Factor
    | Factor
    ;

Factor
    : MINUS Factor
    | LPAREN Expression RPAREN
    | VectorLiteral
    | ID
    | NUMBER
    ;

VectorLiteral
    : LBRACKET VectorElementsOpt RBRACKET
    ;

VectorElementsOpt
    : VectorElements
    | /* empty */
    ;

VectorElements
    : Expression
    | Expression COMMA VectorElements
    | VectorLiteral
    | VectorLiteral COMMA VectorElements
    ;

ArgListOpt
    : ArgList
    | /* empty */
    ;

ArgList
    : Expression
    | Expression COMMA ArgList
    ;

VizArgListOpt
    : VizArgList
    | /* empty */
    ;

VizArgList
    : VizArg
    | VizArg COMMA VizArgList
    ;

VizArg
    : Expression
    | ID ASSIGN STRING
    ;

%%

/* --- Error Handling --- */
void yyerror(const char *s) {
    fprintf(stderr, "Parse error: %s\n", s);
}
