/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     STRING = 259,
     VOID = 260,
     READ = 261,
     WRITE = 262,
     BOOL = 263,
     FLOAT = 264,
     CHAR = 265,
     IF = 266,
     ELSE = 267,
     WHILE = 268,
     DO = 269,
     RETURN = 270,
     NAME = 271,
     INT_NUM = 272,
     FLOAT_NUM = 273,
     STR_CONST = 274,
     ASSIGN_OP = 275,
     COMMA = 276,
     SEMICOLON = 277,
     LEFT_ROUND_BRACKET = 278,
     RIGHT_ROUND_BRACKET = 279,
     LEFT_CURLY_BRACKET = 280,
     RIGHT_CURLY_BRACKET = 281,
     PLUS = 282,
     MINUS = 283,
     MULT = 284,
     DIV = 285,
     GREATER_THAN = 286,
     LESS_THAN = 287,
     GREATER_THAN_EQUAL = 288,
     LESS_THAN_EQUAL = 289,
     EQUAL = 290,
     NOT_EQUAL = 291,
     AND = 292,
     OR = 293,
     QUESTION_MARK = 294,
     COLON = 295,
     NOT = 296,
     UMINUS = 297
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define STRING 259
#define VOID 260
#define READ 261
#define WRITE 262
#define BOOL 263
#define FLOAT 264
#define CHAR 265
#define IF 266
#define ELSE 267
#define WHILE 268
#define DO 269
#define RETURN 270
#define NAME 271
#define INT_NUM 272
#define FLOAT_NUM 273
#define STR_CONST 274
#define ASSIGN_OP 275
#define COMMA 276
#define SEMICOLON 277
#define LEFT_ROUND_BRACKET 278
#define RIGHT_ROUND_BRACKET 279
#define LEFT_CURLY_BRACKET 280
#define RIGHT_CURLY_BRACKET 281
#define PLUS 282
#define MINUS 283
#define MULT 284
#define DIV 285
#define GREATER_THAN 286
#define LESS_THAN 287
#define GREATER_THAN_EQUAL 288
#define LESS_THAN_EQUAL 289
#define EQUAL 290
#define NOT_EQUAL 291
#define AND 292
#define OR 293
#define QUESTION_MARK 294
#define COLON 295
#define NOT 296
#define UMINUS 297




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 190 "parser_new.y"
{
    int type;
    char* name;
    char* str;
    Ast* ast;
    Expression_Ast* expr;
    Statement_Ast* stmt;
    Compound_Stmt* block;
}
/* Line 1529 of yacc.c.  */
#line 143 "parser_new.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

