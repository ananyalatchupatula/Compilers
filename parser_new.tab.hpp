/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_NEW_TAB_HPP_INCLUDED
# define YY_YY_PARSER_NEW_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    STRING = 259,                  /* STRING  */
    VOID = 260,                    /* VOID  */
    READ = 261,                    /* READ  */
    WRITE = 262,                   /* WRITE  */
    BOOL = 263,                    /* BOOL  */
    FLOAT = 264,                   /* FLOAT  */
    CHAR = 265,                    /* CHAR  */
    IF = 266,                      /* IF  */
    ELSE = 267,                    /* ELSE  */
    WHILE = 268,                   /* WHILE  */
    DO = 269,                      /* DO  */
    NAME = 270,                    /* NAME  */
    INT_NUM = 271,                 /* INT_NUM  */
    FLOAT_NUM = 272,               /* FLOAT_NUM  */
    STR_CONST = 273,               /* STR_CONST  */
    ASSIGN_OP = 274,               /* ASSIGN_OP  */
    COMMA = 275,                   /* COMMA  */
    SEMICOLON = 276,               /* SEMICOLON  */
    LEFT_ROUND_BRACKET = 277,      /* LEFT_ROUND_BRACKET  */
    RIGHT_ROUND_BRACKET = 278,     /* RIGHT_ROUND_BRACKET  */
    LEFT_CURLY_BRACKET = 279,      /* LEFT_CURLY_BRACKET  */
    RIGHT_CURLY_BRACKET = 280,     /* RIGHT_CURLY_BRACKET  */
    PLUS = 281,                    /* PLUS  */
    MINUS = 282,                   /* MINUS  */
    MULT = 283,                    /* MULT  */
    DIV = 284,                     /* DIV  */
    GREATER_THAN = 285,            /* GREATER_THAN  */
    LESS_THAN = 286,               /* LESS_THAN  */
    GREATER_THAN_EQUAL = 287,      /* GREATER_THAN_EQUAL  */
    LESS_THAN_EQUAL = 288,         /* LESS_THAN_EQUAL  */
    EQUAL = 289,                   /* EQUAL  */
    NOT_EQUAL = 290,               /* NOT_EQUAL  */
    AND = 291,                     /* AND  */
    OR = 292,                      /* OR  */
    QUESTION_MARK = 293,           /* QUESTION_MARK  */
    COLON = 294,                   /* COLON  */
    NOT = 295,                     /* NOT  */
    UMINUS = 296                   /* UMINUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 160 "parser_new.y"

    int type;
    char* name;
    char* str;
    Ast* ast;
    Expression_Ast* expr;
    Statement_Ast* stmt;
    Compound_Stmt* block;

#line 115 "parser_new.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_NEW_TAB_HPP_INCLUDED  */
