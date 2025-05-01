%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir/ast.h"

ASTNode* final_ast = NULL;

// Add externs for line/column tracking
extern int yylineno;
extern int yycolumn;
extern char* yytext;
%}

/* ----------  UNION  ---------- */
%union {
    double num;
    char* str;
    struct ASTNode* ast;
    struct ASTList* list;
}

/* ----------  TOKENS ----------- */
%token <num> NUMBER
%token <str> ID
%token <str> STRING
%token LT GT

%token IF ELSE WHILE FOR
%token BEGIN_AUX END_AUX
%token SORT REVERSE SLICE AVG TRANSPOSE RUNNING_SUM PAIRWISE_COMPARE PARETO_SET
%token PLOT HISTOGRAM HEATMAP BARCHART PIECHART SCATTER BOXPLOT TIMELINE
%token PLUS MINUS TIMES DIVIDE ASSIGN
%token COMMA SEMICOLON
%token LPAREN RPAREN
%token LBRACE RBRACE
%token LBRACKET RBRACKET
%token IMPORT

/* ----------  PRECEDENCE -------- */
%left LT GT
%left PLUS MINUS
%left TIMES DIVIDE
%nonassoc LPAREN RPAREN  /* Make function calls non-associative */
%left LBRACKET RBRACKET

/* ----------  TYPES ------------ */
%type <ast>  Program Statement Assignment ControlStructure FunctionCall VisualizationCall Expression Term Factor VectorLiteral VizArg
%type <list> StatementList VectorElements ArgList ArgListOpt VizArgList VizArgListOpt
%type <ast> ImportStatement

%%   /* ---------- GRAMMAR ---------- */

Program
    : StatementList                { final_ast = createProgramNode($1); }
    ;

StatementList
    : Statement                        { $$ = createASTList($1); }          /* first stmt */
    | StatementList Statement          { $$ = appendASTList($1, $2); }      /* append   */
    ;

Statement
    : ImportStatement
    | Assignment SEMICOLON
    | ControlStructure
    | VisualizationCall SEMICOLON
    | FunctionCall SEMICOLON
    ;

ImportStatement
    : IMPORT STRING SEMICOLON { $$ = createImportNode($2); }
    ;

Assignment
    : ID ASSIGN Expression         { $$ = createAssignmentNode($1, $3); }
    ;

ControlStructure
    : IF LPAREN Expression RPAREN LBRACE StatementList RBRACE ELSE LBRACE StatementList RBRACE
        { $$ = createIfElseNode($3, $6, $10); }
    | WHILE LPAREN Expression RPAREN LBRACE StatementList RBRACE
        { $$ = createWhileNode($3, $6); }
    | FOR LPAREN Assignment SEMICOLON Expression SEMICOLON Assignment RPAREN LBRACE StatementList RBRACE
        { $$ = createForNode($3, $5, $7, $10); }
    ;

FunctionCall
    : ID LPAREN ArgListOpt RPAREN  { $$ = createFunctionCallNode($1, $3); }
    ;

VisualizationCall
    : PLOT      LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("plot",      $3); }
    | HISTOGRAM LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("histogram", $3); }
    | HEATMAP   LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("heatmap",   $3); }
    | BARCHART  LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("barchart",  $3); }
    | PIECHART  LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("piechart",  $3); }
    | SCATTER   LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("scatter",   $3); }
    | BOXPLOT   LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("boxplot",   $3); }
    | TIMELINE  LPAREN VizArgListOpt RPAREN { $$ = createVizCallNode("timeline",  $3); }
    ;

Expression
    : Expression PLUS Term         { $$ = createBinaryOpNode(OP_PLUS , $1, $3); }
    | Expression MINUS Term         { $$ = createBinaryOpNode(OP_MINUS, $1, $3); }
    | Expression LT Term            { $$ = createBinaryOpNode(OP_LT, $1, $3); }
    | Expression GT Term            { $$ = createBinaryOpNode(OP_GT, $1, $3); }
    | Term
    ;

Term
    : Term TIMES Factor             { $$ = createBinaryOpNode(OP_TIMES, $1, $3); }
    | Term DIVIDE Factor            { $$ = createBinaryOpNode(OP_DIVIDE, $1, $3); }
    | Factor
    ;

Factor
    : NUMBER                        { $$ = createNumberNode($1); }
    | ID                            { $$ = createIdNode($1); }
    | STRING                        { $$ = createStringNode($1); }
    | VectorLiteral                 { $$ = $1; }
    | FunctionCall                  { $$ = $1; }
    | LPAREN Expression RPAREN      { $$ = $2; }
    ;

VectorLiteral
    : LBRACKET VectorElements RBRACKET { $$ = createVectorNode($2); }
    ;

/* ▼▼  THE CORRECTED VECTOR-ELEMENT RULE  ▼▼ */
VectorElements
    : Expression                           { $$ = createASTList($1); }                 /* first element */
    | VectorElements COMMA Expression      { $$ = appendASTList($1, $3); }             /* append more */
    ;

ArgListOpt
    : ArgList                        { $$ = $1; }
    | /* empty */                    { $$ = NULL; }
    ;

ArgList
    : Expression                     { $$ = createASTList($1); }
    | ArgList COMMA Expression       { $$ = appendASTList($1, $3); }
    ;

VizArgListOpt
    : VizArgList                     { $$ = $1; }
    | /* empty */                    { $$ = NULL; }
    ;

VizArgList
    : VizArg                         { $$ = createASTList($1); }
    | VizArgList COMMA VizArg        { $$ = appendASTList($1, $3); }
    ;

VizArg
    : ID ASSIGN STRING
        {   ASTNode* key = createIdNode($1);
            ASTNode* val = createStringNode($3);
            $$ = createBinaryOpNode(OP_ASSIGN, key, val);
        }
    | ID ASSIGN Expression
        {   ASTNode* key = createIdNode($1);
            $$ = createBinaryOpNode(OP_ASSIGN, key, $3);
        }
    | Expression                     { $$ = $1; }
    ;

%%  /* ----------  C code section ---------- */

void yyerror(const char *s) {
    fprintf(stderr, "Parse error at line %d, column %d: %s\n", yylineno, yycolumn, s);
    if (yytext) {
        fprintf(stderr, "Near token: '%s'\n", yytext);
    }
}
    