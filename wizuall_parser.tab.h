/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_YY_WIZUALL_PARSER_TAB_H_INCLUDED
# define YY_YY_WIZUALL_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    NUMBER = 258,
    STRING = 259,
    ID = 260,
    IF = 261,
    ELSE = 262,
    WHILE = 263,
    FOR = 264,
    BEGIN_AUX = 265,
    END_AUX = 266,
    SORT = 267,
    REVERSE = 268,
    SLICE = 269,
    AVG = 270,
    TRANSPOSE = 271,
    RUNNING_SUM = 272,
    PAIRWISE_COMPARE = 273,
    PARETO_SET = 274,
    PLOT = 275,
    HISTOGRAM = 276,
    HEATMAP = 277,
    BARCHART = 278,
    PIECHART = 279,
    SCATTER = 280,
    BOXPLOT = 281,
    TIMELINE = 282,
    PLUS = 283,
    MINUS = 284,
    TIMES = 285,
    DIVIDE = 286,
    ASSIGN = 287,
    COMMA = 288,
    SEMICOLON = 289,
    LPAREN = 290,
    RPAREN = 291,
    LBRACE = 292,
    RBRACE = 293,
    LBRACKET = 294,
    RBRACKET = 295
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 13 "grammar/wizuall_parser.y"

    int num;
    char* str;
    struct ASTNode* ast;

#line 104 "wizuall_parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_WIZUALL_PARSER_TAB_H_INCLUDED  */
