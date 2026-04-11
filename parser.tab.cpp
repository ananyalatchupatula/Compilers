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




/* Copy the first part of user declarations.  */
#line 1 "parser.y"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "ast.h"
#include "tac.h"
#include <iostream>
#include <vector>
#include <string>
#include <climits>
#include <cfloat>

using std::string;
using std::vector;
using std::cout;
using std::endl;

int yylex();
void yyerror(const char *s)
{
    printf("parse error\n");
}

extern int show_ast;
bool main_seen = false;
bool main_defined = false;

/* Main function parameter tracking */
struct MainParam {
    string name;
    int type;
};
vector<MainParam> main_decl_params;  // params from declaration
vector<MainParam> main_def_params;   // params from definition
bool parsing_main_declaration = false;  // flag for which context we're in

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
const char* type_to_string(DataType t)
{
    switch(t)
    {
        case TYPE_INT:    return "int";
        case TYPE_FLOAT:  return "float";
        case TYPE_STRING: return "string";
        case TYPE_BOOL:   return "bool";
        default:          return "unknown";
    }
}


/* SYMBOL CLASS */

class Symbol
{
public:
    string name;
    int type;

    Symbol(string n, int t)
    {
        name = n;
        type = t;
    }
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
#line 150 "parser.y"
{
    int type;
    char* name;
    char* str;
    ASTNode* node;
}
/* Line 193 of yacc.c.  */
#line 324 "parser.tab.cpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 337 "parser.tab.cpp"

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   140

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  38
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  20
/* YYNRULES -- Number of rules.  */
#define YYNRULES  53
/* YYNRULES -- Number of states.  */
#define YYNSTATES  98

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   292

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
      35,    36,    37
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     8,    11,    14,    15,    19,    21,    23,
      25,    27,    29,    31,    33,    37,    44,    45,    53,    55,
      56,    58,    62,    65,    70,    73,    74,    77,    78,    81,
      84,    87,    91,    94,    97,   101,   105,   109,   113,   117,
     121,   125,   129,   133,   137,   141,   145,   151,   154,   157,
     159,   161,   163,   165
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      39,     0,    -1,    40,    44,    40,    45,    -1,    40,    45,
      -1,    40,    41,    -1,    -1,    42,    43,    17,    -1,     3,
      -1,     4,    -1,     8,    -1,     9,    -1,    10,    -1,     5,
      -1,    11,    -1,    43,    16,    11,    -1,    42,    11,    18,
      47,    19,    17,    -1,    -1,    42,    11,    18,    47,    19,
      46,    50,    -1,    48,    -1,    -1,    49,    -1,    48,    16,
      49,    -1,    42,    11,    -1,    20,    51,    52,    21,    -1,
      51,    41,    -1,    -1,    52,    53,    -1,    -1,    54,    17,
      -1,    55,    17,    -1,    56,    17,    -1,    11,    15,    57,
      -1,     6,    11,    -1,     7,    57,    -1,    57,    22,    57,
      -1,    57,    23,    57,    -1,    57,    24,    57,    -1,    57,
      25,    57,    -1,    57,    26,    57,    -1,    57,    27,    57,
      -1,    57,    28,    57,    -1,    57,    29,    57,    -1,    57,
      30,    57,    -1,    57,    31,    57,    -1,    57,    32,    57,
      -1,    57,    33,    57,    -1,    57,    34,    57,    35,    57,
      -1,    36,    57,    -1,    23,    57,    -1,    11,    -1,    12,
      -1,    13,    -1,    14,    -1,    18,    57,    19,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   202,   202,   209,   219,   220,   224,   228,   229,   230,
     231,   232,   233,   237,   248,   262,   306,   305,   416,   421,
     427,   431,   441,   463,   478,   479,   483,   495,   502,   503,
     504,   508,   583,   601,   619,   660,   688,   727,   770,   814,
     858,   902,   946,   989,  1037,  1076,  1118,  1188,  1204,  1228,
    1247,  1264,  1282,  1293
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INTEGER", "STRING", "VOID", "READ",
  "WRITE", "BOOL", "FLOAT", "CHAR", "NAME", "INT_NUM", "FLOAT_NUM",
  "STR_CONST", "ASSIGN_OP", "COMMA", "SEMICOLON", "LEFT_ROUND_BRACKET",
  "RIGHT_ROUND_BRACKET", "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET",
  "PLUS", "MINUS", "MULT", "DIV", "GREATER_THAN", "LESS_THAN",
  "GREATER_THAN_EQUAL", "LESS_THAN_EQUAL", "EQUAL", "NOT_EQUAL", "AND",
  "OR", "QUESTION_MARK", "COLON", "NOT", "UMINUS", "$accept", "program",
  "globals_var_decls", "var_decl", "type", "id_list", "func_decl",
  "func_def", "@1", "param_list_opt", "param_list", "param", "block",
  "decl_list_opt", "stmt_list", "stmt", "assign_stmt", "read_stmt",
  "write_stmt", "expr", 0
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
     285,   286,   287,   288,   289,   290,   291,   292
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    38,    39,    39,    40,    40,    41,    42,    42,    42,
      42,    42,    42,    43,    43,    44,    46,    45,    47,    47,
      48,    48,    49,    50,    51,    51,    52,    52,    53,    53,
      53,    54,    55,    56,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     4,     2,     2,     0,     3,     1,     1,     1,
       1,     1,     1,     1,     3,     6,     0,     7,     1,     0,
       1,     3,     2,     4,     2,     0,     2,     0,     2,     2,
       2,     3,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     5,     2,     2,     1,
       1,     1,     1,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,     0,     0,     1,     7,     8,    12,     9,    10,    11,
       4,     0,     5,     3,    13,     0,     0,    19,     0,     6,
       0,     2,     0,     0,    18,    20,    14,    13,    22,    16,
       0,    19,    15,     0,    21,     0,    25,    17,    16,    27,
      24,     0,     0,    13,     0,     0,     0,    23,    26,     0,
       0,     0,    32,    49,    50,    51,    52,     0,     0,     0,
      33,     0,    28,    29,    30,     0,    48,    47,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    53,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,     0,     0,    46
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     1,     2,    10,    22,    15,    12,    13,    33,    23,
      24,    25,    37,    39,    42,    48,    49,    50,    51,    60
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -58
static const yytype_int8 yypact[] =
{
     -58,     3,   117,   -58,   -58,   -58,   -58,   -58,   -58,   -58,
     -58,    -2,   -58,   -58,    13,    19,   117,   117,    22,   -58,
      27,   -58,    31,    24,    30,   -58,   -58,   105,   -58,    28,
     117,   117,   -58,   104,   -58,   109,   -58,   -58,   -58,   117,
     -58,    38,    23,   -58,   118,    14,   115,   -58,   -58,   114,
     116,   119,   -58,   -58,   -58,   -58,   -58,    14,    14,    14,
      56,    14,   -58,   -58,   -58,    29,   -58,    80,    14,    14,
      14,    14,    14,    14,    14,    14,    14,    14,    14,    14,
      14,    56,   -58,    16,    16,   -58,   -58,   -17,   -17,   -17,
     -17,    90,    90,    80,    69,    42,    14,    56
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -58,   -58,   120,    95,     8,   -58,   -58,   121,   -58,   107,
     -58,   110,   -58,   -58,   -58,   -58,   -58,   -58,   -58,   -57
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      65,    66,    67,     3,    81,    68,    69,    70,    71,    14,
      11,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    20,    53,    54,    55,    56,    44,
      45,    17,    57,    26,    46,    18,    19,    58,    27,    97,
      70,    71,    28,    29,    47,    32,    30,    41,    82,    43,
      59,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    96,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    68,    69,    70,    71,    72,    73,    74,    75,
       4,     5,     6,    31,    36,     7,     8,     9,    38,    52,
      61,    62,    16,    63,    40,     0,    64,    21,    35,     0,
      34
};

static const yytype_int8 yycheck[] =
{
      57,    58,    59,     0,    61,    22,    23,    24,    25,    11,
       2,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    16,    11,    12,    13,    14,     6,
       7,    18,    18,    11,    11,    16,    17,    23,    11,    96,
      24,    25,    11,    19,    21,    17,    16,    39,    19,    11,
      36,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    22,    23,    24,    25,    26,    27,    28,    29,
       3,     4,     5,    18,    20,     8,     9,    10,    19,    11,
      15,    17,    12,    17,    39,    -1,    17,    16,    31,    -1,
      30
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    39,    40,     0,     3,     4,     5,     8,     9,    10,
      41,    42,    44,    45,    11,    43,    40,    18,    16,    17,
      42,    45,    42,    47,    48,    49,    11,    11,    11,    19,
      16,    18,    17,    46,    49,    47,    20,    50,    19,    51,
      41,    42,    52,    11,     6,     7,    11,    21,    53,    54,
      55,    56,    11,    11,    12,    13,    14,    18,    23,    36,
      57,    15,    17,    17,    17,    57,    57,    57,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32,    33,
      34,    57,    19,    57,    57,    57,    57,    57,    57,    57,
      57,    57,    57,    57,    57,    57,    35,    57
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
#line 203 "parser.y"
    {
        if(!main_defined){
            cout << "Semantic error:main fn not defined" << endl;
            exit(1);
        }
    ;}
    break;

  case 3:
#line 210 "parser.y"
    {
        if(!main_defined){
            cout << "Semantic error:main fn not defined" << endl;
            exit(1);
        }
    ;}
    break;

  case 7:
#line 228 "parser.y"
    { (yyval.type) = TYPE_INT; current_decl_type = TYPE_INT; ;}
    break;

  case 8:
#line 229 "parser.y"
    { (yyval.type) = TYPE_STRING; current_decl_type = TYPE_STRING; ;}
    break;

  case 9:
#line 230 "parser.y"
    { (yyval.type) = TYPE_BOOL; current_decl_type = TYPE_BOOL; ;}
    break;

  case 10:
#line 231 "parser.y"
    { (yyval.type) = TYPE_FLOAT; current_decl_type = TYPE_FLOAT; ;}
    break;

  case 11:
#line 232 "parser.y"
    { (yyval.type) = TYPE_CHAR; current_decl_type = TYPE_CHAR; ;}
    break;

  case 12:
#line 233 "parser.y"
    { (yyval.type) = TYPE_VOID; current_decl_type = TYPE_VOID; ;}
    break;

  case 13:
#line 238 "parser.y"
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

  case 14:
#line 249 "parser.y"
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

  case 15:
#line 263 "parser.y"
    {
        if(strcmp((yyvsp[(2) - (6)].name),"main") != 0 || (yyvsp[(1) - (6)].type) != TYPE_VOID){
            cout << "Semantic error: only void main allowed" << endl;
            exit(1);
        }

        if(main_seen){
            cout << "Semantic error: multiple main decls" << endl;
            exit(1);
        }

        /* Check for char parameters in declaration and store params */
        if((yyvsp[(4) - (6)].node) != NULL) {
            ASTNode* param_node = (yyvsp[(4) - (6)].node);
            while(param_node) {
                int param_type = (int)param_node->type;
                
                if(param_type == TYPE_CHAR) {
                    cout << "Semantic error: cant parse" << endl;
                    exit(1);
                }
                
                /* Extract name from label (format: "name_     Type:<type>") */
                char* label = param_node->label;
                char param_name[128];
                sscanf(label, "%[^_]", param_name);
                
                MainParam mp;
                mp.name = string(param_name);
                mp.type = param_type;
                main_decl_params.push_back(mp);
                
                param_node = param_node->right;
            }
        }

        main_seen = true;
    ;}
    break;

  case 16:
#line 306 "parser.y"
    {
        in_function = true;
        main_def_params.clear();  /* Clear previous definition params */
        local_symtab.table.clear();  /* Clear local symbol table for new function */
        /* Parameters are already added to local_symtab in param rule */
    ;}
    break;

  case 17:
#line 313 "parser.y"
    {

        /* Procedure name */
        if(strcmp((yyvsp[(2) - (7)].name),"main") != 0 || (yyvsp[(1) - (7)].type) != TYPE_VOID){
            cout << "Semantic error: only void main allowed" << endl;
            exit(1);
        }

        if(main_defined){
            cout << "Semantic error:multiple main definitions" << endl;
            exit(1);
        }

        /* Check for char parameters in definition and store params */
        if((yyvsp[(4) - (7)].node) != NULL) {
            ASTNode* param_node = (yyvsp[(4) - (7)].node);
            while(param_node) {
                int param_type = (int)param_node->type;
                
                if(param_type == TYPE_CHAR) {
                    cout << "Semantic error: cant parse" << endl;
                    exit(1);
                }
                
                /* Extract name from label (format: "name_     Type:<type>") */
                char* label = param_node->label;
                char param_name[128];
                sscanf(label, "%[^_]", param_name);
                
                MainParam mp;
                mp.name = string(param_name);
                mp.type = param_type;
                main_def_params.push_back(mp);
                
                param_node = param_node->right;
            }
        }

        /* Validate main definition params match declaration if declaration exists */
        if(main_seen && main_decl_params.size() > 0) {
            if(main_def_params.size() != main_decl_params.size()) {
                cout << "Semantic error: main definition parameters do not match declaration" << endl;
                exit(1);
            }
            
            for(size_t i = 0; i < main_def_params.size(); i++) {
                if(main_def_params[i].name != main_decl_params[i].name) {
                    cout << "Semantic error: main definition parameter name mismatch" << endl;
                    exit(1);
                }
                if(main_def_params[i].type != main_decl_params[i].type) {
                    cout << "Semantic error: main definition parameter type mismatch" << endl;
                    exit(1);
                }
            }
        }

        main_seen = true;
        main_defined = true;

        char procbuf[128];
        snprintf(procbuf,sizeof(procbuf),"**PROCEDURE: %s",(yyvsp[(2) - (7)].name));

        /*Return type */
        const char* typestr =
            ((yyvsp[(1) - (7)].type)==TYPE_INT)?"<int>":
            ((yyvsp[(1) - (7)].type)==TYPE_FLOAT)?"<float>":
            ((yyvsp[(1) - (7)].type)==TYPE_BOOL)?"<bool>":
            ((yyvsp[(1) - (7)].type)==TYPE_STRING)?"<string>":
            ((yyvsp[(1) - (7)].type)==TYPE_CHAR)?"<char>":
            ((yyvsp[(1) - (7)].type)==TYPE_VOID)?"<void>":"";

    char retbuf[128];
    snprintf(retbuf,sizeof(retbuf),"Return Type: %s",typestr);

    ASTNode* returnNode = make_node(retbuf,(DataType)(yyvsp[(1) - (7)].type),NULL,NULL,NULL);

    ASTNode* paramNode = make_node("Formal Parameters:",(DataType)TYPE_VOID,NULL,NULL,NULL);

    /* Attach parameters to paramNode */
    if((yyvsp[(4) - (7)].node) != NULL) {
        paramNode->left = (yyvsp[(4) - (7)].node);
    }

    /*END NODE*/
    ASTNode* endNode = make_node("**END: Abstract Syntax Tree",(DataType)TYPE_VOID,NULL,NULL,NULL);

    ASTNode* beginNode = (yyvsp[(7) - (7)].node);
    returnNode->right = paramNode;
    paramNode->right = beginNode;

    ASTNode* t = beginNode;
    while(t->right) t = t->right;
    t->right = endNode;

    (yyval.node) = make_node(procbuf,(DataType)TYPE_VOID,returnNode,NULL,NULL);

    if(show_ast) print_ast((yyval.node),0);
    generate_TAC((yyval.node));
    in_function = false;
;}
    break;

  case 18:
#line 417 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 19:
#line 421 "parser.y"
    {
          (yyval.node) = NULL;
      ;}
    break;

  case 20:
#line 428 "parser.y"
    {
          (yyval.node) = (yyvsp[(1) - (1)].node);
      ;}
    break;

  case 21:
#line 432 "parser.y"
    {
          ASTNode* t = (yyvsp[(1) - (3)].node);
          while(t->right) t = t->right;
          t->right = (yyvsp[(3) - (3)].node);
          (yyval.node) = (yyvsp[(1) - (3)].node);
      ;}
    break;

  case 22:
#line 442 "parser.y"
    {
          /* Only add to symbol table if we're in a function definition, not a declaration */
          if(in_function) {
              local_symtab.add((yyvsp[(2) - (2)].name),(yyvsp[(1) - (2)].type));
          }
          
          /* Create AST node for this parameter */
          const char* typestr =
              ((yyvsp[(1) - (2)].type)==TYPE_INT)?"<int>":
              ((yyvsp[(1) - (2)].type)==TYPE_FLOAT)?"<float>":
              ((yyvsp[(1) - (2)].type)==TYPE_BOOL)?"<bool>":
              ((yyvsp[(1) - (2)].type)==TYPE_STRING)?"<string>":
              ((yyvsp[(1) - (2)].type)==TYPE_CHAR)?"<char>":"";
          
          char buf[128];
          snprintf(buf, sizeof(buf), "%s_     Type:%s", (yyvsp[(2) - (2)].name), typestr);
          (yyval.node) = make_node(buf, (DataType)(yyvsp[(1) - (2)].type), NULL, NULL, NULL);
      ;}
    break;

  case 23:
#line 464 "parser.y"
    {
    (yyval.node) = make_node(
            (char*)"**BEGIN: Abstract Syntax Tree",
            (DataType)TYPE_VOID,
            (yyvsp[(3) - (4)].node),
            NULL,
            NULL);


;}
    break;

  case 26:
#line 484 "parser.y"
    {
    if((yyvsp[(1) - (2)].node)==NULL) (yyval.node)=(yyvsp[(2) - (2)].node);
    else
    {
        ASTNode* t=(yyvsp[(1) - (2)].node);
        while(t->right) t=t->right;
        t->right=(yyvsp[(2) - (2)].node);
        (yyval.node)=(yyvsp[(1) - (2)].node);
    }
;}
    break;

  case 27:
#line 495 "parser.y"
    {
    (yyval.node) = NULL;
;}
    break;

  case 31:
#line 509 "parser.y"
    {
    int lhs = lookup((yyvsp[(1) - (3)].name));
    int rhs = (yyvsp[(3) - (3)].node)->type;

    if(lhs == TYPE_ERROR || rhs == TYPE_ERROR)
        exit(1);

    if(lhs == TYPE_FLOAT && rhs == TYPE_INT)
    {
        // allowed widening
    }
    else if(lhs != rhs)
    {
        cout << "Type error: cannot assign" << endl;
        exit(1);
    }

    char buf[128];
    const char* typestr =
    (lhs==TYPE_INT)?"<int>":
    (lhs==TYPE_FLOAT)?"<float>":
    (lhs==TYPE_BOOL)?"<bool>":
    (lhs==TYPE_STRING)?"<string>":
    (lhs==TYPE_CHAR)?"<char>":"";

    snprintf(buf,sizeof(buf),"Name : %s_%s", (yyvsp[(1) - (3)].name), typestr);



    ASTNode* lhsNode =
        make_node(buf,(DataType)lhs,NULL,NULL,NULL);


    ASTNode* closeRHS =
        make_node(")",
            (DataType)TYPE_VOID,
            NULL,NULL,NULL);
    
    ASTNode* tmp = (yyvsp[(3) - (3)].node);
    while(tmp->right) tmp = tmp->right;
    tmp->right = closeRHS;

    ASTNode* rhsWrapper = 
            make_node("RHS (", (DataType)(yyvsp[(3) - (3)].node)->type,(yyvsp[(3) - (3)].node),NULL,NULL);

        /* CREATE LHS NODE */
    ASTNode* closeLHS =
    make_node(")",
              (DataType)TYPE_VOID,
              NULL,NULL,NULL);

    lhsNode->right = closeLHS;

    ASTNode* lhsWrapper =
        make_node("LHS (",
                (DataType)lhs,
                lhsNode,
                NULL,
                NULL);


    /* FINAL ASSIGN NODE */
    (yyval.node) = make_node(
            "Asgn:",
            (DataType)TYPE_VOID,
            lhsWrapper,
            rhsWrapper,
            NULL);
;}
    break;

  case 32:
#line 584 "parser.y"
    {
          DataType t = (DataType)lookup((yyvsp[(2) - (2)].name));

          if(t != TYPE_INT && t != TYPE_FLOAT){
            cout << "Type error: read supports only int and float " << endl;
            exit(1);
        }

          char buf[128];
          snprintf(buf, sizeof(buf),"Read: Name : %s_<%s>", (yyvsp[(2) - (2)].name), type_to_string(t));

          (yyval.node) = make_node(buf, t, NULL, NULL, NULL);
      ;}
    break;

  case 33:
#line 602 "parser.y"
    {
    if((yyvsp[(2) - (2)].node)->type == TYPE_BOOL){
        cout << "Type error: write does not support boolean variables" << endl;
        exit(1);
    }

    (yyval.node) =
        make_node("Write:",
            (DataType)TYPE_VOID,
            (yyvsp[(2) - (2)].node),
            NULL,
            NULL);
;}
    break;

  case 34:
#line 620 "parser.y"
    {
    if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
    {
        cout<<"Type error in +"<<endl;
        exit(1);
    }

    DataType resultType = (DataType)numericResult((yyvsp[(1) - (3)].node)->type,(yyvsp[(3) - (3)].node)->type);

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL = (yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right = closeL;

    ASTNode* L =
        make_node("L_Opd (", (yyvsp[(1) - (3)].node)->type, (yyvsp[(1) - (3)].node),NULL,NULL);


    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR = (yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right = closeR;

    ASTNode* R =
        make_node("R_Opd (", (yyvsp[(3) - (3)].node)->type, (yyvsp[(3) - (3)].node),NULL,NULL);


    /* FIX: dynamic type */
    char buf[128];
    snprintf(buf, sizeof(buf), "Arith: Plus<%s>", type_to_string(resultType));

    (yyval.node) = make_node(buf, resultType, L, R, NULL);
;}
    break;

  case 35:
#line 661 "parser.y"
    {
    DataType resultType = (DataType)numericResult((yyvsp[(1) - (3)].node)->type, (yyvsp[(3) - (3)].node)->type);

    // Create brackets
    ASTNode* closeL = make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);
    ASTNode* closeR = make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);

    // FIX: Traverse to the ABSOLUTE end of the left child
    ASTNode* currL = (yyvsp[(1) - (3)].node);
    while(currL && currL->right) currL = currL->right;
    if(currL) currL->right = closeL;

    ASTNode* L = make_node("L_Opd (", (DataType)(yyvsp[(1) - (3)].node)->type, (yyvsp[(1) - (3)].node), NULL, NULL);

    // FIX: Traverse to the ABSOLUTE end of the right child
    ASTNode* currR = (yyvsp[(3) - (3)].node);
    while(currR && currR->right) currR = currR->right;
    if(currR) currR->right = closeR;

    ASTNode* R = make_node("R_Opd (", (DataType)(yyvsp[(3) - (3)].node)->type, (yyvsp[(3) - (3)].node), NULL, NULL);

    char buf[128];
    snprintf(buf, sizeof(buf), "Arith: Minus<%s>", type_to_string(resultType));
    (yyval.node) = make_node(buf, resultType, L, R, NULL);
;}
    break;

  case 36:
#line 689 "parser.y"
    {
    if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
    {
        cout<<"Type error in *"<<endl;
        exit(1);
    }

    DataType resultType = (DataType)numericResult((yyvsp[(1) - (3)].node)->type,(yyvsp[(3) - (3)].node)->type);

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL = (yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right = closeL;

    ASTNode* L =
        make_node("L_Opd (", (yyvsp[(1) - (3)].node)->type, (yyvsp[(1) - (3)].node),NULL,NULL);


    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR = (yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right = closeR;

    ASTNode* R =
        make_node("R_Opd (", (yyvsp[(3) - (3)].node)->type, (yyvsp[(3) - (3)].node),NULL,NULL);


    char buf[128];
    snprintf(buf, sizeof(buf),"Arith: Mult<%s>", type_to_string(resultType));

    (yyval.node) = make_node(buf, resultType, L, R, NULL);
;}
    break;

  case 37:
#line 728 "parser.y"
    {
    if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
    {
        cout<<"Type error in division"<<endl;
        exit(1);
    }

    DataType resultType = (DataType)numericResult((yyvsp[(1) - (3)].node)->type,(yyvsp[(3) - (3)].node)->type);

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL = (yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right = closeL;

    ASTNode* L =
        make_node("L_Opd (", (yyvsp[(1) - (3)].node)->type, (yyvsp[(1) - (3)].node),NULL,NULL);


    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR = (yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right = closeR;

    ASTNode* R =
        make_node("R_Opd (", (yyvsp[(3) - (3)].node)->type, (yyvsp[(3) - (3)].node),NULL,NULL);


    /* FIX: generate correct type string */
    char buf[128];
    snprintf(buf, sizeof(buf), "Arith: Div<%s>", type_to_string(resultType));

    (yyval.node) = make_node(buf, resultType, L, R, NULL);
;}
    break;

  case 38:
#line 771 "parser.y"
    {
    if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
    {
        cout<<"Type error in >"<<endl;
        exit(1);
    }
    if((yyvsp[(1) - (3)].node)->type != (yyvsp[(3) - (3)].node)->type)
    {
        cout<<"Type error: relational operations require same data types"<<endl;
        exit(1);
    }

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: GT<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);
;}
    break;

  case 39:
#line 815 "parser.y"
    {
        if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
        {
            cout<<"Type error in <"<<endl;
            exit(1);
        }
        if((yyvsp[(1) - (3)].node)->type != (yyvsp[(3) - (3)].node)->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: LT<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    ;}
    break;

  case 40:
#line 859 "parser.y"
    {
        if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
        {
            cout<<"Type error in >="<<endl;
            exit(1);
        }
        if((yyvsp[(1) - (3)].node)->type != (yyvsp[(3) - (3)].node)->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: GE<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    ;}
    break;

  case 41:
#line 903 "parser.y"
    {
        if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
        {
            cout<<"Type error in <="<<endl;
            exit(1);
        }
        if((yyvsp[(1) - (3)].node)->type != (yyvsp[(3) - (3)].node)->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: LE<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);


    ;}
    break;

  case 42:
#line 947 "parser.y"
    {
        if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
        {
            cout<<"Type error in =="<<endl;
            exit(1);
        }
        if((yyvsp[(1) - (3)].node)->type != (yyvsp[(3) - (3)].node)->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: EQ<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);
    ;}
    break;

  case 43:
#line 990 "parser.y"
    {
        if(!isNumeric((yyvsp[(1) - (3)].node)->type) || !isNumeric((yyvsp[(3) - (3)].node)->type))
        {
            cout<<"Type error in !="<<endl;
            exit(1);
        }
        if((yyvsp[(1) - (3)].node)->type != (yyvsp[(3) - (3)].node)->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: NE<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    ;}
    break;

  case 44:
#line 1038 "parser.y"
    {
        if((yyvsp[(1) - (3)].node)->type!=TYPE_BOOL || (yyvsp[(3) - (3)].node)->type!=TYPE_BOOL)
        {
            cout<<"Type error in &&"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: AND<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    ;}
    break;

  case 45:
#line 1077 "parser.y"
    {
        if((yyvsp[(1) - (3)].node)->type!=TYPE_BOOL || (yyvsp[(3) - (3)].node)->type!=TYPE_BOOL)
        {
            cout<<"Type error in ||"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=(yyvsp[(1) - (3)].node);
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)(yyvsp[(1) - (3)].node)->type,
            (yyvsp[(1) - (3)].node),NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=(yyvsp[(3) - (3)].node);
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)(yyvsp[(3) - (3)].node)->type,
            (yyvsp[(3) - (3)].node),NULL,NULL);

    (yyval.node)=
        make_node("Condition: OR<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);


    ;}
    break;

  case 46:
#line 1119 "parser.y"
    {
        if((yyvsp[(1) - (5)].node)->type != TYPE_BOOL)
        {
            cout<<"Type error in ternary condition"<<endl;
            exit(1);
        }

        DataType resultType;

        if(isNumeric((yyvsp[(3) - (5)].node)->type) && isNumeric((yyvsp[(5) - (5)].node)->type))
            resultType = (DataType)numericResult((yyvsp[(3) - (5)].node)->type,(yyvsp[(5) - (5)].node)->type);
        else if((yyvsp[(3) - (5)].node)->type == (yyvsp[(5) - (5)].node)->type)
            resultType = (DataType)(yyvsp[(3) - (5)].node)->type;
        else
        {
            cout<<"Type error in ternary branches"<<endl;
            exit(1);
        }

        ASTNode* closeTrue =
    make_node(")",
            (DataType)TYPE_VOID,
            NULL,NULL,NULL);

ASTNode* tempT=(yyvsp[(3) - (5)].node);
while(tempT->right) tempT=tempT->right;
tempT->right=closeTrue;

ASTNode* truePart =
    make_node("True_Part (",
            resultType,
            (yyvsp[(3) - (5)].node),
            NULL,
            NULL);


        ASTNode* closeFalse =
    make_node(")",
            (DataType)TYPE_VOID,
            NULL,NULL,NULL);

    ASTNode* tempF=(yyvsp[(5) - (5)].node);
    while(tempF->right) tempF=tempF->right;
    tempF->right=closeFalse;

    ASTNode* falsePart =
        make_node("False_Part (",
                resultType,
                (yyvsp[(5) - (5)].node),
                NULL,
                NULL);

        /* For AST this is giving correct result for tac it is givin wrong result like not correctly giving stemp 
        ASTNode* t = $1;
        while(t->right) t = t->right; // Find end of condition list
        
        t->right = truePart;        // Attach True_Part
        truePart->right = falsePart; // Attach False_Part

        $1->type = resultType;      // Update head type
        $$ = $1; */

        /* For TAC this is giving correctly but for ast this is printing "Ternary:" which shouldnt be printed and also it is giving only two brackets at the ed of ternary for some cases. */
        ASTNode* ternary_node = make_node("Ternary:", resultType, (yyvsp[(1) - (5)].node), truePart, falsePart);

        (yyval.node) = ternary_node; 

    ;}
    break;

  case 47:
#line 1189 "parser.y"
    {
        ASTNode* close =
            make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);

        ASTNode* tmp = (yyvsp[(2) - (2)].node);
        while(tmp->right) tmp = tmp->right;
        tmp->right = close;

        ASTNode* childWrapper =
            make_node("L_Opd (", (DataType)TYPE_BOOL, (yyvsp[(2) - (2)].node), NULL, NULL);

        (yyval.node) = make_node("Condition: NOT<bool>", (DataType)TYPE_BOOL, childWrapper, NULL, NULL);

    ;}
    break;

  case 48:
#line 1205 "parser.y"
    {
        if(!isNumeric((yyvsp[(2) - (2)].node)->type)){
            cout << "Type error in unary minus" << endl;
            exit(1);
        }

        ASTNode* closeL =
            make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);

        ASTNode* temp = (yyvsp[(2) - (2)].node);
        while(temp->right) temp = temp->right;
        temp->right = closeL;

        ASTNode* L =
            make_node("L_Opd (", (DataType)(yyvsp[(2) - (2)].node)->type, (yyvsp[(2) - (2)].node), NULL, NULL);

        char buf[128];
        snprintf(buf, sizeof(buf), "Arith: Uminus<%s>", type_to_string((DataType)(yyvsp[(2) - (2)].node)->type));

        (yyval.node) = make_node(buf, (DataType)(yyvsp[(2) - (2)].node)->type, L, NULL, NULL);
    ;}
    break;

  case 49:
#line 1229 "parser.y"
    {
        int t = lookup((yyvsp[(1) - (1)].name));

        char buf[128];
        const char* typestr =
            (t==TYPE_INT)?"<int>":
            (t==TYPE_FLOAT)?"<float>":
            (t==TYPE_BOOL)?"<bool>":
            (t==TYPE_STRING)?"<string>":"";

        snprintf(buf,sizeof(buf),"Name : %s_%s", (yyvsp[(1) - (1)].name), typestr);



        (yyval.node) = make_node(buf,(DataType)t,NULL,NULL,NULL);
    ;}
    break;

  case 50:
#line 1248 "parser.y"
    {
        char buf[128];
        long long val = strtoll((yyvsp[(1) - (1)].str), NULL, 10);
        
        /* Adjust for overflow using two's complement wrapping */
        int adjusted = (int)val;  /* This automatically wraps for out-of-range values */
        
        /* Display the adjusted value in the AST */
        snprintf(buf, sizeof(buf), "Num : %d<int>", adjusted);
        (yyval.node) = make_node(
                strdup(buf),
                (DataType)TYPE_INT,
                NULL,NULL,NULL);
    ;}
    break;

  case 51:
#line 1265 "parser.y"
    {
        char buf[128];
        double val = atof((yyvsp[(1) - (1)].str));
        
        /* Adjust for overflow by clamping to float range */
        float adjusted = (float)val;  /* Conversion handles overflow automatically */
        if(val > FLT_MAX) adjusted = FLT_MAX;
        else if(val < -FLT_MAX) adjusted = -FLT_MAX;
        
        snprintf(buf, sizeof(buf), "Num : %.2f<float>", adjusted);
        (yyval.node) = make_node(
                strdup(buf),
                (DataType)TYPE_FLOAT,
                NULL,NULL,NULL);
    ;}
    break;

  case 52:
#line 1283 "parser.y"
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "String : %s<string>", (yyvsp[(1) - (1)].str));
        (yyval.node) = make_node(
                strdup(buf),
                (DataType)TYPE_STRING,
                NULL,NULL,NULL);
    ;}
    break;

  case 53:
#line 1294 "parser.y"
    {
        (yyval.node)=(yyvsp[(2) - (3)].node);
    ;}
    break;


/* Line 1267 of yacc.c.  */
#line 2755 "parser.tab.cpp"
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


#line 1301 "parser.y"



