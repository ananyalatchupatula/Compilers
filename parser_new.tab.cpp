/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



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




/* Copy the first part of user declarations.  */
#line 1 "parser_new.y"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ast_new.h"
#include "tac_str.h"
#include "tac_generator.h"
#include "rtl_generator.h"
#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <cfloat>
#include <list>

using std::string;
using std::vector;
using std::cout;
using std::endl;
using std::list;

int yylex();
void yyerror(const char *s)
{
    printf("parse error\n");
}

extern int show_ast;
extern int show_tac;
extern int show_rtl;
extern FILE *ast_file;
extern FILE *tac_file;
extern FILE *rtl_file;

list<Statement_Ast*> main_stmt_list;
bool main_seen = false;
bool main_defined = false;

/* Main function parameter tracking */
struct MainParam {
    string name;
    int type;
};

/* Function parameter tracking for parameter list printing */
struct FuncParam {
    string name;
    int type;
};
vector<FuncParam> current_func_params;

class FunctionInfo {
public:
    string name;
    int return_type;
    bool is_defined;
    int return_label_id;  // Pre-allocated label ID (-1 if void or not allocated)

    FunctionInfo(string n, int rt, bool def=false)
        : name(n), return_type(rt), is_defined(def), return_label_id(-1) {}
};

vector<FunctionInfo> function_table;
string current_function_name;
vector<MainParam> main_decl_params;
vector<MainParam> main_def_params;
bool parsing_main_declaration = false;

/* Store function bodies for deferred TAC generation */
struct DeferredFunction {
    string name;
    int return_type;
    Statement_Ast* body;
    int return_label_id;  // Pre-allocated label ID
};
vector<DeferredFunction> deferred_functions;

/* TYPE DEFINITIONS */
#define TYPE_INT    1
#define TYPE_FLOAT  2
#define TYPE_STRING 3
#define TYPE_BOOL   4
#define TYPE_CHAR   5
#define TYPE_VOID   6
#define TYPE_ERROR -1

bool isNumeric(int t)
{
    return t==TYPE_INT || t==TYPE_FLOAT;
}

int numericResult(int t1,int t2)
{
    if(t1==TYPE_FLOAT || t2==TYPE_FLOAT)
        return TYPE_FLOAT;
    return TYPE_INT;
}

DataType int_to_datatype(int t)
{
    switch(t) {
        case TYPE_INT:    return INT_DATA_TYPE;
        case TYPE_FLOAT:  return FLOAT_DATA_TYPE;
        case TYPE_BOOL:   return BOOL_DATA_TYPE;
        case TYPE_STRING: return STRING_DATA_TYPE;
        case TYPE_CHAR:   return CHAR_DATA_TYPE;
        case TYPE_VOID:   return VOID_DATA_TYPE;
        default:          return ERROR_DATA_TYPE;
    }
}

int datatype_to_int(DataType t)
{
    switch(t) {
        case INT_DATA_TYPE:    return TYPE_INT;
        case FLOAT_DATA_TYPE:  return TYPE_FLOAT;
        case BOOL_DATA_TYPE:   return TYPE_BOOL;
        case STRING_DATA_TYPE: return TYPE_STRING;
        case CHAR_DATA_TYPE:   return TYPE_CHAR;
        case VOID_DATA_TYPE:   return TYPE_VOID;
        default:               return TYPE_ERROR;
    }
}

/* SYMBOL CLASS */
class Symbol
{
public:
    string name;
    int type;

    Symbol(string n, int t) : name(n), type(t) {}
};

/* SYMBOL TABLE CLASS */
class SymbolTable
{
public:
    vector<Symbol> table;

    void add(string name, int type)
    {
        for (size_t i = 0; i < table.size(); i++)
        {
            if (table[i].name == name)
            {
                cout << "Semantic error: redeclaration of " << name << endl;
                exit(1);
            }
        }
        table.push_back(Symbol(name, type));
    }

    int get(string name)
    {
        for (size_t i = 0; i < table.size(); i++)
        {
            if (table[i].name == name)
                return table[i].type;
        }
        cout << "Semantic error: undeclared variable " << name << endl;
        exit(1);
    }
};

/* GLOBAL SYMBOL TABLE */
SymbolTable global_symtab;
SymbolTable local_symtab;
bool in_function = false;
int current_decl_type;

int lookup(string name){
    for(auto &s:local_symtab.table){
        if(s.name == name){
            return s.type;
        }
    }
    for(auto &s:global_symtab.table){
        if(s.name == name){
            return s.type;
        }
    }
    cout << "Semantic error: undeclared variable " << name << endl;
    exit(1);
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

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
/* Line 193 of yacc.c.  */
#line 378 "parser_new.tab.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 391 "parser_new.tab.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  16
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   215

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  76
/* YYNRULES -- Number of states.  */
#define YYNSTATES  137

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   297

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    18,    20,
      22,    26,    28,    30,    32,    34,    36,    38,    40,    44,
      45,    52,    55,    56,    60,    62,    63,    65,    69,    72,
      74,    75,    77,    81,    86,    89,    90,    93,    94,    97,
     100,   103,   105,   107,   109,   112,   115,   117,   123,   131,
     137,   145,   149,   152,   155,   158,   163,   167,   171,   175,
     179,   183,   187,   191,   195,   199,   203,   207,   211,   214,
     217,   223,   227,   232,   234,   236,   238
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      44,     0,    -1,    45,    -1,    45,    46,    -1,    46,    -1,
      47,    -1,    48,    -1,    50,    51,    22,    -1,    54,    -1,
      55,    -1,    50,    51,    22,    -1,     3,    -1,     4,    -1,
       8,    -1,     9,    -1,    10,    -1,     5,    -1,    16,    -1,
      51,    21,    16,    -1,    -1,    50,    16,    23,    53,    57,
      24,    -1,    52,    22,    -1,    -1,    52,    56,    62,    -1,
      58,    -1,    -1,    59,    -1,    58,    21,    59,    -1,    50,
      16,    -1,    61,    -1,    -1,    74,    -1,    61,    21,    74,
      -1,    25,    63,    64,    26,    -1,    63,    49,    -1,    -1,
      64,    65,    -1,    -1,    69,    22,    -1,    70,    22,    -1,
      71,    22,    -1,    66,    -1,    67,    -1,    68,    -1,    72,
      22,    -1,    73,    22,    -1,    62,    -1,    11,    23,    74,
      24,    65,    -1,    11,    23,    74,    24,    65,    12,    65,
      -1,    13,    23,    74,    24,    65,    -1,    14,    65,    13,
      23,    74,    24,    22,    -1,    16,    20,    74,    -1,     6,
      16,    -1,     7,    74,    -1,    15,    74,    -1,    16,    23,
      60,    24,    -1,    74,    27,    74,    -1,    74,    28,    74,
      -1,    74,    29,    74,    -1,    74,    30,    74,    -1,    74,
      31,    74,    -1,    74,    32,    74,    -1,    74,    33,    74,
      -1,    74,    34,    74,    -1,    74,    35,    74,    -1,    74,
      36,    74,    -1,    74,    37,    74,    -1,    74,    38,    74,
      -1,    41,    74,    -1,    28,    74,    -1,    74,    39,    74,
      40,    74,    -1,    23,    74,    24,    -1,    16,    23,    60,
      24,    -1,    16,    -1,    17,    -1,    18,    -1,    19,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   244,   244,   254,   255,   259,   260,   264,   268,   269,
     273,   277,   278,   279,   280,   281,   282,   286,   297,   312,
     311,   325,   359,   358,   464,   465,   469,   470,   480,   497,
     498,   502,   508,   518,   525,   526,   530,   540,   546,   547,
     548,   549,   550,   551,   552,   553,   554,   558,   566,   577,
     588,   599,   623,   633,   645,   652,   686,   702,   718,   734,
     750,   766,   782,   798,   814,   830,   846,   857,   868,   878,
     888,   903,   907,   943,   950,   959,   963
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "STRING", "VOID", "READ",
  "WRITE", "BOOL", "FLOAT", "CHAR", "IF", "ELSE", "WHILE", "DO", "RETURN",
  "NAME", "INT_NUM", "FLOAT_NUM", "STR_CONST", "ASSIGN_OP", "COMMA",
  "SEMICOLON", "LEFT_ROUND_BRACKET", "RIGHT_ROUND_BRACKET",
  "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", "PLUS", "MINUS", "MULT",
  "DIV", "GREATER_THAN", "LESS_THAN", "GREATER_THAN_EQUAL",
  "LESS_THAN_EQUAL", "EQUAL", "NOT_EQUAL", "AND", "OR", "QUESTION_MARK",
  "COLON", "NOT", "UMINUS", "$accept", "program", "program_items",
  "program_item", "global_var_group", "func_item", "var_decl", "type",
  "id_list", "func_header", "@1", "func_decl", "func_def", "@2",
  "param_list_opt", "param_list", "param", "arg_list_opt", "arg_list",
  "block", "decl_list_opt", "stmt_list", "stmt", "if_stmt", "while_stmt",
  "do_while_stmt", "assign_stmt", "read_stmt", "write_stmt", "return_stmt",
  "func_call_stmt", "expr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    47,    48,    48,
      49,    50,    50,    50,    50,    50,    50,    51,    51,    53,
      52,    54,    56,    55,    57,    57,    58,    58,    59,    60,
      60,    61,    61,    62,    63,    63,    64,    64,    65,    65,
      65,    65,    65,    65,    65,    65,    65,    66,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     3,     1,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     3,     0,
       6,     2,     0,     3,     1,     0,     1,     3,     2,     1,
       0,     1,     3,     4,     2,     0,     2,     0,     2,     2,
       2,     1,     1,     1,     2,     2,     1,     5,     7,     5,
       7,     3,     2,     2,     2,     4,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     2,     2,
       5,     3,     4,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,    11,    12,    16,    13,    14,    15,     0,     2,     4,
       5,     6,     0,    22,     8,     9,     1,     3,    17,     0,
      21,     0,    19,     0,     7,    35,    23,    25,    18,    37,
       0,     0,    24,    26,    34,     0,     0,    28,    20,     0,
      17,     0,     0,     0,     0,     0,     0,     0,     0,    33,
      46,    36,    41,    42,    43,     0,     0,     0,     0,     0,
      27,    10,    52,    73,    74,    75,    76,     0,     0,     0,
      53,     0,     0,     0,    54,     0,    30,    38,    39,    40,
      44,    45,    30,     0,    69,    68,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    51,     0,    29,    31,     0,    71,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
       0,     0,     0,     0,    55,     0,    72,     0,    47,    49,
       0,    32,    70,     0,     0,    48,    50
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     7,     8,     9,    10,    11,    34,    12,    19,    13,
      27,    14,    15,    21,    31,    32,    33,   103,   104,    50,
      29,    36,    51,    52,    53,    54,    55,    56,    57,    58,
      59,   105
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -47
static const yytype_int16 yypact[] =
{
      12,   -47,   -47,   -47,   -47,   -47,   -47,     3,    12,   -47,
     -47,   -47,    -4,    -3,   -47,   -47,   -47,   -47,     5,   -11,
     -47,    39,   -47,    16,   -47,   -47,   -47,    12,   -47,    12,
      57,    50,    58,   -47,   -47,    64,    52,   -47,   -47,    12,
     -47,     2,    66,   -10,    61,    63,    82,   -10,    37,   -47,
     -47,   -47,   -47,   -47,   -47,    68,    69,    70,    72,    77,
     -47,   -47,   -47,    78,   -47,   -47,   -47,   -10,   -10,   -10,
     156,   -10,   -10,    87,   156,   -10,   -10,   -47,   -47,   -47,
     -47,   -47,   -10,    81,   -47,     7,   -10,   -10,   -10,   -10,
     -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,   -10,    97,
     113,    79,   156,    80,    85,   156,    98,   -47,    32,    32,
     -47,   -47,    42,    42,    42,    42,   180,   180,     7,   169,
     142,    82,    82,   -10,   -47,   -10,   -47,   -10,    91,   -47,
     129,   156,   156,    82,   101,   -47,   -47
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -47,   -47,   -47,   130,   -47,   -47,   -47,   -25,   104,   -47,
     -47,   -47,   -47,   -47,   -47,   -47,   115,    73,   -47,   194,
     -47,   -47,   -46,   -47,   -47,   -47,   -47,   -47,   -47,   -47,
     -47,   -42
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      73,    70,    30,    16,    35,    74,    63,    64,    65,    66,
      23,    24,    18,    67,    30,     1,     2,     3,    68,    20,
       4,     5,     6,    23,    61,    83,    84,    85,    22,    99,
     100,    69,    28,   102,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,    75,    42,    43,
      76,    88,    89,    44,    25,    45,    46,    47,    48,    86,
      87,    88,    89,    37,    38,   128,   129,    25,    49,    39,
      40,   130,    62,   131,    71,   132,    72,   135,    42,    43,
      77,    78,    79,    44,    80,    45,    46,    47,    48,    81,
     101,    82,   123,   133,   124,   107,   125,    25,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,   121,   126,   136,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,   122,    17,    41,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,   134,    60,   106,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,   127,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    86,    87,    88,
      89,    90,    91,    92,    93,    26
};

static const yytype_uint8 yycheck[] =
{
      46,    43,    27,     0,    29,    47,    16,    17,    18,    19,
      21,    22,    16,    23,    39,     3,     4,     5,    28,    22,
       8,     9,    10,    21,    22,    67,    68,    69,    23,    71,
      72,    41,    16,    75,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    20,     6,     7,
      23,    29,    30,    11,    25,    13,    14,    15,    16,    27,
      28,    29,    30,    16,    24,   121,   122,    25,    26,    21,
      16,   123,    16,   125,    23,   127,    23,   133,     6,     7,
      22,    22,    22,    11,    22,    13,    14,    15,    16,    22,
      13,    23,    23,    12,    24,    24,    21,    25,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    24,    24,    22,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    24,     8,    35,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    24,    39,    82,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    27,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    27,    28,    29,
      30,    31,    32,    33,    34,    21
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     8,     9,    10,    44,    45,    46,
      47,    48,    50,    52,    54,    55,     0,    46,    16,    51,
      22,    56,    23,    21,    22,    25,    62,    53,    16,    63,
      50,    57,    58,    59,    49,    50,    64,    16,    24,    21,
      16,    51,     6,     7,    11,    13,    14,    15,    16,    26,
      62,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      59,    22,    16,    16,    17,    18,    19,    23,    28,    41,
      74,    23,    23,    65,    74,    20,    23,    22,    22,    22,
      22,    22,    23,    74,    74,    74,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    74,
      74,    13,    74,    60,    61,    74,    60,    24,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    24,    24,    23,    24,    21,    24,    40,    65,    65,
      74,    74,    74,    12,    24,    65,    22
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 245 "parser_new.y"
    {
        if(!main_defined){
            cout << "Semantic error: main fn not defined" << endl;
            exit(1);
        }
    ;}
    break;

  case 11:
#line 277 "parser_new.y"
    { (yyval.type) = TYPE_INT; current_decl_type = TYPE_INT; ;}
    break;

  case 12:
#line 278 "parser_new.y"
    { (yyval.type) = TYPE_STRING; current_decl_type = TYPE_STRING; ;}
    break;

  case 13:
#line 279 "parser_new.y"
    { (yyval.type) = TYPE_BOOL; current_decl_type = TYPE_BOOL; ;}
    break;

  case 14:
#line 280 "parser_new.y"
    { (yyval.type) = TYPE_FLOAT; current_decl_type = TYPE_FLOAT; ;}
    break;

  case 15:
#line 281 "parser_new.y"
    { (yyval.type) = TYPE_CHAR; current_decl_type = TYPE_CHAR; ;}
    break;

  case 16:
#line 282 "parser_new.y"
    { (yyval.type) = TYPE_VOID; current_decl_type = TYPE_VOID; ;}
    break;

  case 17:
#line 287 "parser_new.y"
    {
          if(strcmp((yyvsp[(1) - (1)].name), "main") == 0) {
              cout << "Semantic error: variable cannot be named main" << endl;
              exit(1);
          }
          if(in_function)
              local_symtab.add((yyvsp[(1) - (1)].name),current_decl_type);
          else
              global_symtab.add((yyvsp[(1) - (1)].name),current_decl_type);
      ;}
    break;

  case 18:
#line 298 "parser_new.y"
    {
          if(strcmp((yyvsp[(3) - (3)].name), "main") == 0) {
              cout << "Semantic error: variable cannot be named main" << endl;
              exit(1);
          }
          if(in_function)
              local_symtab.add((yyvsp[(3) - (3)].name),current_decl_type);
          else
              global_symtab.add((yyvsp[(3) - (3)].name),current_decl_type);
      ;}
    break;

  case 19:
#line 312 "parser_new.y"
    {
        current_function_name = string((yyvsp[(2) - (3)].name));
        in_function = true;
        local_symtab.table.clear();
        current_func_params.clear();  /* Clear parameters for new function */
    ;}
    break;

  case 20:
#line 319 "parser_new.y"
    {
        (yyval.type) = (yyvsp[(1) - (6)].type);
    ;}
    break;

  case 21:
#line 326 "parser_new.y"
    {
        string fname = current_function_name;

        for(auto &f : function_table){
            if(f.name == fname){
                cout << "Semantic error: multiple declaration of function "
                     << fname << endl;
                exit(1);
            }
        }

        FunctionInfo func_info(fname, (yyvsp[(1) - (2)].type), false);
        
        // Pre-allocate return label for non-void functions at declaration time
        // This ensures labels are allocated in declaration order
        if((yyvsp[(1) - (2)].type) != TYPE_VOID && (show_tac || show_rtl)) {
            Label_TAC_Opd* ret_label = TAC_Generator::get_instance()->create_new_label();
            func_info.return_label_id = ret_label->get_label_id();
            delete ret_label;
        }
        
        function_table.push_back(func_info);

        if(fname == "main"){
            main_seen = true;
        }
        
        in_function = false;  /* Reset after function declaration */
    ;}
    break;

  case 22:
#line 359 "parser_new.y"
    {
        string fname = current_function_name;
        bool found = false;

        for(auto &f : function_table){
            if(f.name == fname){
                if(f.is_defined){
                    cout << "Semantic error: multiple definition of function "
                         << fname << endl;
                    exit(1);
                }
                f.is_defined = true;
                found = true;
                break;
            }
        }

        if(!found){
            function_table.push_back(FunctionInfo(fname, (yyvsp[(1) - (1)].type), true));
        }
    ;}
    break;

  case 23:
#line 381 "parser_new.y"
    {
        if(current_function_name == "main"){
            if((yyvsp[(1) - (3)].type) != TYPE_VOID && (yyvsp[(1) - (3)].type) != TYPE_INT){
                cout << "Semantic error: main must be void or int" << endl;
                exit(1);
            }

            if(main_defined){
                cout << "Semantic error: multiple main definitions" << endl;
                exit(1);
            }

            main_seen = true;
            main_defined = true;
        }

        /* For now, just print the AST of the function body */
        if(show_ast && ast_file) {
            if(current_function_name == "main")
    fprintf(ast_file, "**PROCEDURE: %s\n", current_function_name.c_str());
else
    fprintf(ast_file, "**PROCEDURE: %s_\n", current_function_name.c_str());
            fprintf(ast_file, "  Return Type: <%s>\n", data_type_to_string(int_to_datatype((yyvsp[(1) - (3)].type))));
            fprintf(ast_file, "  Formal Parameters:\n");
            /* Print formal parameters */
            for(auto &param : current_func_params) {
                fprintf(ast_file, "      %s_  Type:<%s>\n", param.name.c_str(), data_type_to_string(int_to_datatype(param.type)));
            }
            fprintf(ast_file, "**BEGIN: Abstract Syntax Tree\n");
            if((yyvsp[(3) - (3)].block)) (yyvsp[(3) - (3)].block)->print(2);
            fprintf(ast_file, "\n**END: Abstract Syntax Tree\n");
        }

        /* Generate  for the function body */
        if((yyvsp[(3) - (3)].block)) {
            list<TAC_Stmt*> tac_stmts;
            int ret_type = (yyvsp[(1) - (3)].type);
            DataType ret_data_type = int_to_datatype(ret_type);
            
            if(show_tac || show_rtl) {
                TAC_Generator::get_instance()->reset_counters();
            }
            
            // For non-void functions, find or allocate return label
            int ret_label_id = -1;
            if(ret_data_type != VOID_DATA_TYPE) {
                // Find the pre-allocated label ID from function_table
                for(auto &f : function_table) {
                    if(f.name == current_function_name) {
                        ret_label_id = f.return_label_id;
                        break;
                    }
                }
                
                // If no pre-allocated label (shouldn't happen), allocate new one
                if(ret_label_id == -1) {
                    Label_TAC_Opd* ret_label = TAC_Generator::get_instance()->create_new_label();
                    ret_label_id = ret_label->get_label_id();
                    delete ret_label;
                }
                
                // Set the label ID on the compound statement
                (yyvsp[(3) - (3)].block)->set_return_label_id(ret_label_id);
            }
            
            if(show_tac || show_rtl) {
                // Store function body for deferred TAC generation (after all pre_allocate_temps)
                DeferredFunction df;
                df.name = current_function_name;
                df.return_type = ret_type;
                df.body = (yyvsp[(3) - (3)].block);
                df.return_label_id = ret_label_id;
                deferred_functions.push_back(df);
            } else {
                delete (yyvsp[(3) - (3)].block);
            }
        }

        in_function = false;
    ;}
    break;

  case 24:
#line 464 "parser_new.y"
    { (yyval.ast) = (yyvsp[(1) - (1)].ast); ;}
    break;

  case 25:
#line 465 "parser_new.y"
    { (yyval.ast) = NULL; ;}
    break;

  case 26:
#line 469 "parser_new.y"
    { (yyval.ast) = (yyvsp[(1) - (1)].ast); ;}
    break;

  case 27:
#line 471 "parser_new.y"
    {
          // For new AST, params are handled differently
          // Just return the first param for now
          // Full param list support requires compound statement structure
          (yyval.ast) = (yyvsp[(1) - (3)].ast);
      ;}
    break;

  case 28:
#line 481 "parser_new.y"
    {
          // Check for char parameters - this is not allowed
          if((yyvsp[(1) - (2)].type) == TYPE_CHAR) {
              cout << "Semantic error: cant parse" << endl;
              exit(1);
          }
          if(in_function) {
              local_symtab.add((yyvsp[(2) - (2)].name),(yyvsp[(1) - (2)].type));
              /* Also add to current_func_params for AST printing */
              current_func_params.push_back(FuncParam{string((yyvsp[(2) - (2)].name)), (yyvsp[(1) - (2)].type)});
          }
          (yyval.ast) = NULL;  /* Placeholder for now */
      ;}
    break;

  case 29:
#line 497 "parser_new.y"
    { (yyval.ast) = (yyvsp[(1) - (1)].ast); ;}
    break;

  case 30:
#line 498 "parser_new.y"
    { (yyval.ast) = NULL; ;}
    break;

  case 31:
#line 503 "parser_new.y"
    {
          FunctionCall_Expr_Ast *tmp = new FunctionCall_Expr_Ast("__tmp__");
          tmp->add_argument((yyvsp[(1) - (1)].expr));
          (yyval.ast) = tmp;
      ;}
    break;

  case 32:
#line 509 "parser_new.y"
    {
          FunctionCall_Expr_Ast *tmp =
              dynamic_cast<FunctionCall_Expr_Ast*>((yyvsp[(1) - (3)].ast));
          tmp->add_argument((yyvsp[(3) - (3)].expr));
          (yyval.ast) = tmp;
      ;}
    break;

  case 33:
#line 519 "parser_new.y"
    {
        (yyval.block) = (yyvsp[(3) - (4)].block);  /* Return the statement list as a compound statement */
    ;}
    break;

  case 36:
#line 531 "parser_new.y"
    {
        if((yyvsp[(1) - (2)].block) == NULL) {
            (yyval.block) = new Compound_Stmt();
            if((yyvsp[(2) - (2)].stmt)) (yyval.block)->add_stmt((yyvsp[(2) - (2)].stmt));
        } else {
            (yyval.block)->add_stmt((yyvsp[(2) - (2)].stmt));
        }
    ;}
    break;

  case 37:
#line 540 "parser_new.y"
    {
        (yyval.block) = new Compound_Stmt();
    ;}
    break;

  case 38:
#line 546 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (2)].stmt); ;}
    break;

  case 39:
#line 547 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (2)].stmt); ;}
    break;

  case 40:
#line 548 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (2)].stmt); ;}
    break;

  case 41:
#line 549 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 42:
#line 550 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 43:
#line 551 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 44:
#line 552 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (2)].stmt); ;}
    break;

  case 45:
#line 553 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (2)].stmt); ;}
    break;

  case 46:
#line 554 "parser_new.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].block); ;}
    break;

  case 47:
#line 559 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(3) - (5)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: if condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new If_Stmt((yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].stmt), NULL);
    ;}
    break;

  case 48:
#line 567 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(3) - (7)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: if condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new If_Stmt((yyvsp[(3) - (7)].expr), (yyvsp[(5) - (7)].stmt), (yyvsp[(7) - (7)].stmt));
    ;}
    break;

  case 49:
#line 578 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(3) - (5)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: while condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new While_Stmt((yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].stmt));
    ;}
    break;

  case 50:
#line 589 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(5) - (7)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: do-while condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new Do_While_Stmt((yyvsp[(2) - (7)].stmt), (yyvsp[(5) - (7)].expr));
    ;}
    break;

  case 51:
#line 600 "parser_new.y"
    {
        int lhs_type = lookup((yyvsp[(1) - (3)].name));
        int rhs_type = datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type());

        if(lhs_type == TYPE_ERROR || rhs_type == TYPE_ERROR) {
            cout << "Semantic error: type error in assignment" << endl;
            exit(1);
        }

        if(lhs_type == TYPE_FLOAT && rhs_type == TYPE_INT) {
            /* Implicit conversion allowed */
        } else if(lhs_type != rhs_type) {
            cout << "Semantic error: type mismatch in assignment" << endl;
            exit(1);
        }

        string lhs_name_with_underscore = string((yyvsp[(1) - (3)].name)) + "_";
        (yyval.stmt) = new Assignment_Stmt(lhs_name_with_underscore, (yyvsp[(3) - (3)].expr));
        (yyval.stmt)->set_data_type(int_to_datatype(lhs_type));
    ;}
    break;

  case 52:
#line 624 "parser_new.y"
    {
        int var_type = lookup((yyvsp[(2) - (2)].name));
        string var_name_with_underscore = string((yyvsp[(2) - (2)].name)) + "_";
        (yyval.stmt) = new Read_Stmt(var_name_with_underscore);
        (yyval.stmt)->set_data_type(int_to_datatype(var_type));
    ;}
    break;

  case 53:
#line 634 "parser_new.y"
    {
        if((yyvsp[(2) - (2)].expr)->get_data_type() == BOOL_DATA_TYPE){
            cout << "Semantic error: cannot print bool" << endl;
            exit(1);
        }

        (yyval.stmt) = new Print_Stmt((yyvsp[(2) - (2)].expr));
    ;}
    break;

  case 54:
#line 646 "parser_new.y"
    {
        (yyval.stmt) = new Return_Stmt((yyvsp[(2) - (2)].expr));
    ;}
    break;

  case 55:
#line 653 "parser_new.y"
    {
        FunctionCall_Stmt *call = new FunctionCall_Stmt((yyvsp[(1) - (4)].name));

        if((yyvsp[(3) - (4)].ast)){
            FunctionCall_Expr_Ast *tmp =
                dynamic_cast<FunctionCall_Expr_Ast*>((yyvsp[(3) - (4)].ast));

            for(auto arg : tmp->get_arguments()){
                call->add_argument(arg);
            }

            tmp->get_arguments().clear();
            delete tmp;
        }

        bool found = false;
        for(auto &f : function_table){
            if(f.name == string((yyvsp[(1) - (4)].name))){
                found = true;
                break;
            }
        }

        if(!found){
            cout << "Semantic error: function not declared " << (yyvsp[(1) - (4)].name) << endl;
            exit(1);
        }

        (yyval.stmt) = call;
    ;}
    break;

  case 56:
#line 687 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()),
            datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::PLUS_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    ;}
    break;

  case 57:
#line 703 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()),
            datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::MINUS_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    ;}
    break;

  case 58:
#line 719 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()),
            datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::MULT_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    ;}
    break;

  case 59:
#line 735 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()),
            datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::DIV_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    ;}
    break;

  case 60:
#line 751 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::GT_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 61:
#line 767 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::LT_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 62:
#line 783 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::GE_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 63:
#line 799 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::LE_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 64:
#line 815 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::EQ_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 65:
#line 831 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::NE_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 66:
#line 847 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != TYPE_BOOL || 
           datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical AND requires bool operands" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::AND_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 67:
#line 858 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(1) - (3)].expr)->get_data_type()) != TYPE_BOOL || 
           datatype_to_int((yyvsp[(3) - (3)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical OR requires bool operands" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[(1) - (3)].expr), Binary_Expr_Ast::OR_OP, (yyvsp[(3) - (3)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 68:
#line 869 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(2) - (2)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical NOT requires bool operand" << endl;
            exit(1);
        }

        (yyval.expr) = new Unary_Expr_Ast(Unary_Expr_Ast::NOT_OP, (yyvsp[(2) - (2)].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    ;}
    break;

  case 69:
#line 879 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[(2) - (2)].expr)->get_data_type()))) {
            cout << "Semantic error: unary minus requires numeric operand" << endl;
            exit(1);
        }

        (yyval.expr) = new Unary_Expr_Ast(Unary_Expr_Ast::UMINUS_OP, (yyvsp[(2) - (2)].expr));
        (yyval.expr)->set_data_type((yyvsp[(2) - (2)].expr)->get_data_type());
    ;}
    break;

  case 70:
#line 889 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[(1) - (5)].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: ternary condition must be bool" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[(3) - (5)].expr)->get_data_type()) != datatype_to_int((yyvsp[(5) - (5)].expr)->get_data_type())) {
            cout << "Semantic error: ternary branches must have same type" << endl;
            exit(1);
        }

        (yyval.expr) = new Ternary_Expr_Ast((yyvsp[(1) - (5)].expr), (yyvsp[(3) - (5)].expr), (yyvsp[(5) - (5)].expr));
        (yyval.expr)->set_data_type((yyvsp[(3) - (5)].expr)->get_data_type());
    ;}
    break;

  case 71:
#line 904 "parser_new.y"
    {
        (yyval.expr) = (yyvsp[(2) - (3)].expr);
    ;}
    break;

  case 72:
#line 908 "parser_new.y"
    {
        FunctionCall_Expr_Ast *call = new FunctionCall_Expr_Ast((yyvsp[(1) - (4)].name));

        if((yyvsp[(3) - (4)].ast)){
            FunctionCall_Expr_Ast *tmp =
                dynamic_cast<FunctionCall_Expr_Ast*>((yyvsp[(3) - (4)].ast));

            for(auto arg : tmp->get_arguments()){
                call->add_argument(arg);
            }

            tmp->get_arguments().clear();
            delete tmp;
        }

        int ret_type = TYPE_VOID;
        bool found = false;

        for(auto &f : function_table){
            if(f.name == string((yyvsp[(1) - (4)].name))){
                ret_type = f.return_type;
                found = true;
                break;
            }
        }

        if(!found){
            cout << "Semantic error: function not declared " << (yyvsp[(1) - (4)].name) << endl;
            exit(1);
        }

        (yyval.expr) = call;
        (yyval.expr)->set_data_type(int_to_datatype(ret_type));
    ;}
    break;

  case 73:
#line 944 "parser_new.y"
    {
        int var_type = lookup((yyvsp[(1) - (1)].name));
        string name_with_underscore = string((yyvsp[(1) - (1)].name)) + "_";
        (yyval.expr) = new Name_Expr_Ast(name_with_underscore);
        (yyval.expr)->set_data_type(int_to_datatype(var_type));
    ;}
    break;

  case 74:
#line 951 "parser_new.y"
    {
        // Handle integer overflow with signed wrapping
        long long val = strtoll((yyvsp[(1) - (1)].str), NULL, 10);
        int adjusted = (int)val;  // Automatic two's complement wrapping
        char buf[128];
        snprintf(buf, sizeof(buf), "%d", adjusted);
        (yyval.expr) = new Const_Expr_Ast(string(buf), INT_DATA_TYPE);
    ;}
    break;

  case 75:
#line 960 "parser_new.y"
    {
        (yyval.expr) = new Const_Expr_Ast((yyvsp[(1) - (1)].str), FLOAT_DATA_TYPE);
    ;}
    break;

  case 76:
#line 964 "parser_new.y"
    {
        (yyval.expr) = new Const_Expr_Ast((yyvsp[(1) - (1)].str), STRING_DATA_TYPE);
    ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2571 "parser_new.tab.cpp"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


#line 969 "parser_new.y"


