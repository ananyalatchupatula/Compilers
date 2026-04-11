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
     NAME = 266,
     INT_NUM = 267,
     FLOAT_NUM = 268,
     STR_CONST = 269,
     ASSIGN_OP = 270,
     COMMA = 271,
     SEMICOLON = 272,
     LEFT_ROUND_BRACKET = 273,
     RIGHT_ROUND_BRACKET = 274,
     LEFT_CURLY_BRACKET = 275,
     RIGHT_CURLY_BRACKET = 276,
     PLUS = 277,
     MINUS = 278,
     MULT = 279,
     DIV = 280,
     GREATER_THAN = 281,
     LESS_THAN = 282,
     GREATER_THAN_EQUAL = 283,
     LESS_THAN_EQUAL = 284,
     EQUAL = 285,
     NOT_EQUAL = 286,
     AND = 287,
     OR = 288,
     QUESTION_MARK = 289,
     COLON = 290,
     NOT = 291,
     UMINUS = 292
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
#define NAME 266
#define INT_NUM 267
#define FLOAT_NUM 268
#define STR_CONST 269
#define ASSIGN_OP 270
#define COMMA 271
#define SEMICOLON 272
#define LEFT_ROUND_BRACKET 273
#define RIGHT_ROUND_BRACKET 274
#define LEFT_CURLY_BRACKET 275
#define RIGHT_CURLY_BRACKET 276
#define PLUS 277
#define MINUS 278
#define MULT 279
#define DIV 280
#define GREATER_THAN 281
#define LESS_THAN 282
#define GREATER_THAN_EQUAL 283
#define LESS_THAN_EQUAL 284
#define EQUAL 285
#define NOT_EQUAL 286
#define AND 287
#define OR 288
#define QUESTION_MARK 289
#define COLON 290
#define NOT 291
#define UMINUS 292




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 150 "parser.y"
{
    int type;
    char* name;
    char* str;
    ASTNode* node;
}
/* Line 1529 of yacc.c.  */
#line 130 "parser.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

