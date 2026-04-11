/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
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


#line 259 "parser_new.tab.cpp"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "parser_new.tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_INTEGER = 3,                    /* INTEGER  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_VOID = 5,                       /* VOID  */
  YYSYMBOL_READ = 6,                       /* READ  */
  YYSYMBOL_WRITE = 7,                      /* WRITE  */
  YYSYMBOL_BOOL = 8,                       /* BOOL  */
  YYSYMBOL_FLOAT = 9,                      /* FLOAT  */
  YYSYMBOL_CHAR = 10,                      /* CHAR  */
  YYSYMBOL_IF = 11,                        /* IF  */
  YYSYMBOL_ELSE = 12,                      /* ELSE  */
  YYSYMBOL_WHILE = 13,                     /* WHILE  */
  YYSYMBOL_DO = 14,                        /* DO  */
  YYSYMBOL_RETURN = 15,                    /* RETURN  */
  YYSYMBOL_NAME = 16,                      /* NAME  */
  YYSYMBOL_INT_NUM = 17,                   /* INT_NUM  */
  YYSYMBOL_FLOAT_NUM = 18,                 /* FLOAT_NUM  */
  YYSYMBOL_STR_CONST = 19,                 /* STR_CONST  */
  YYSYMBOL_ASSIGN_OP = 20,                 /* ASSIGN_OP  */
  YYSYMBOL_COMMA = 21,                     /* COMMA  */
  YYSYMBOL_SEMICOLON = 22,                 /* SEMICOLON  */
  YYSYMBOL_LEFT_ROUND_BRACKET = 23,        /* LEFT_ROUND_BRACKET  */
  YYSYMBOL_RIGHT_ROUND_BRACKET = 24,       /* RIGHT_ROUND_BRACKET  */
  YYSYMBOL_LEFT_CURLY_BRACKET = 25,        /* LEFT_CURLY_BRACKET  */
  YYSYMBOL_RIGHT_CURLY_BRACKET = 26,       /* RIGHT_CURLY_BRACKET  */
  YYSYMBOL_PLUS = 27,                      /* PLUS  */
  YYSYMBOL_MINUS = 28,                     /* MINUS  */
  YYSYMBOL_MULT = 29,                      /* MULT  */
  YYSYMBOL_DIV = 30,                       /* DIV  */
  YYSYMBOL_GREATER_THAN = 31,              /* GREATER_THAN  */
  YYSYMBOL_LESS_THAN = 32,                 /* LESS_THAN  */
  YYSYMBOL_GREATER_THAN_EQUAL = 33,        /* GREATER_THAN_EQUAL  */
  YYSYMBOL_LESS_THAN_EQUAL = 34,           /* LESS_THAN_EQUAL  */
  YYSYMBOL_EQUAL = 35,                     /* EQUAL  */
  YYSYMBOL_NOT_EQUAL = 36,                 /* NOT_EQUAL  */
  YYSYMBOL_AND = 37,                       /* AND  */
  YYSYMBOL_OR = 38,                        /* OR  */
  YYSYMBOL_QUESTION_MARK = 39,             /* QUESTION_MARK  */
  YYSYMBOL_COLON = 40,                     /* COLON  */
  YYSYMBOL_NOT = 41,                       /* NOT  */
  YYSYMBOL_UMINUS = 42,                    /* UMINUS  */
  YYSYMBOL_YYACCEPT = 43,                  /* $accept  */
  YYSYMBOL_program = 44,                   /* program  */
  YYSYMBOL_program_items = 45,             /* program_items  */
  YYSYMBOL_program_item = 46,              /* program_item  */
  YYSYMBOL_global_var_group = 47,          /* global_var_group  */
  YYSYMBOL_func_item = 48,                 /* func_item  */
  YYSYMBOL_var_decl = 49,                  /* var_decl  */
  YYSYMBOL_type = 50,                      /* type  */
  YYSYMBOL_id_list = 51,                   /* id_list  */
  YYSYMBOL_func_header = 52,               /* func_header  */
  YYSYMBOL_53_1 = 53,                      /* $@1  */
  YYSYMBOL_func_decl = 54,                 /* func_decl  */
  YYSYMBOL_func_def = 55,                  /* func_def  */
  YYSYMBOL_56_2 = 56,                      /* $@2  */
  YYSYMBOL_param_list_opt = 57,            /* param_list_opt  */
  YYSYMBOL_param_list = 58,                /* param_list  */
  YYSYMBOL_param = 59,                     /* param  */
  YYSYMBOL_arg_list_opt = 60,              /* arg_list_opt  */
  YYSYMBOL_arg_list = 61,                  /* arg_list  */
  YYSYMBOL_block = 62,                     /* block  */
  YYSYMBOL_decl_list_opt = 63,             /* decl_list_opt  */
  YYSYMBOL_stmt_list = 64,                 /* stmt_list  */
  YYSYMBOL_stmt = 65,                      /* stmt  */
  YYSYMBOL_if_stmt = 66,                   /* if_stmt  */
  YYSYMBOL_while_stmt = 67,                /* while_stmt  */
  YYSYMBOL_do_while_stmt = 68,             /* do_while_stmt  */
  YYSYMBOL_assign_stmt = 69,               /* assign_stmt  */
  YYSYMBOL_read_stmt = 70,                 /* read_stmt  */
  YYSYMBOL_write_stmt = 71,                /* write_stmt  */
  YYSYMBOL_return_stmt = 72,               /* return_stmt  */
  YYSYMBOL_func_call_stmt = 73,            /* func_call_stmt  */
  YYSYMBOL_expr = 74                       /* expr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

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
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  137

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   297


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
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

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "INTEGER", "STRING",
  "VOID", "READ", "WRITE", "BOOL", "FLOAT", "CHAR", "IF", "ELSE", "WHILE",
  "DO", "RETURN", "NAME", "INT_NUM", "FLOAT_NUM", "STR_CONST", "ASSIGN_OP",
  "COMMA", "SEMICOLON", "LEFT_ROUND_BRACKET", "RIGHT_ROUND_BRACKET",
  "LEFT_CURLY_BRACKET", "RIGHT_CURLY_BRACKET", "PLUS", "MINUS", "MULT",
  "DIV", "GREATER_THAN", "LESS_THAN", "GREATER_THAN_EQUAL",
  "LESS_THAN_EQUAL", "EQUAL", "NOT_EQUAL", "AND", "OR", "QUESTION_MARK",
  "COLON", "NOT", "UMINUS", "$accept", "program", "program_items",
  "program_item", "global_var_group", "func_item", "var_decl", "type",
  "id_list", "func_header", "$@1", "func_decl", "func_def", "$@2",
  "param_list_opt", "param_list", "param", "arg_list_opt", "arg_list",
  "block", "decl_list_opt", "stmt_list", "stmt", "if_stmt", "while_stmt",
  "do_while_stmt", "assign_stmt", "read_stmt", "write_stmt", "return_stmt",
  "func_call_stmt", "expr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-47)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
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

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
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

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -47,   -47,   -47,   130,   -47,   -47,   -47,   -25,   104,   -47,
     -47,   -47,   -47,   -47,   -47,   -47,   115,    73,   -47,   194,
     -47,   -47,   -46,   -47,   -47,   -47,   -47,   -47,   -47,   -47,
     -47,   -42
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     7,     8,     9,    10,    11,    34,    12,    19,    13,
      27,    14,    15,    21,    31,    32,    33,   103,   104,    50,
      29,    36,    51,    52,    53,    54,    55,    56,    57,    58,
      59,   105
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
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

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
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

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
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

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
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


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* program: program_items  */
#line 245 "parser_new.y"
    {
        if(!main_defined){
            cout << "Semantic error: main fn not defined" << endl;
            exit(1);
        }
    }
#line 1435 "parser_new.tab.cpp"
    break;

  case 11: /* type: INTEGER  */
#line 277 "parser_new.y"
              { (yyval.type) = TYPE_INT; current_decl_type = TYPE_INT; }
#line 1441 "parser_new.tab.cpp"
    break;

  case 12: /* type: STRING  */
#line 278 "parser_new.y"
              { (yyval.type) = TYPE_STRING; current_decl_type = TYPE_STRING; }
#line 1447 "parser_new.tab.cpp"
    break;

  case 13: /* type: BOOL  */
#line 279 "parser_new.y"
              { (yyval.type) = TYPE_BOOL; current_decl_type = TYPE_BOOL; }
#line 1453 "parser_new.tab.cpp"
    break;

  case 14: /* type: FLOAT  */
#line 280 "parser_new.y"
              { (yyval.type) = TYPE_FLOAT; current_decl_type = TYPE_FLOAT; }
#line 1459 "parser_new.tab.cpp"
    break;

  case 15: /* type: CHAR  */
#line 281 "parser_new.y"
              { (yyval.type) = TYPE_CHAR; current_decl_type = TYPE_CHAR; }
#line 1465 "parser_new.tab.cpp"
    break;

  case 16: /* type: VOID  */
#line 282 "parser_new.y"
              { (yyval.type) = TYPE_VOID; current_decl_type = TYPE_VOID; }
#line 1471 "parser_new.tab.cpp"
    break;

  case 17: /* id_list: NAME  */
#line 287 "parser_new.y"
        {
          if(strcmp((yyvsp[0].name), "main") == 0) {
              cout << "Semantic error: variable cannot be named main" << endl;
              exit(1);
          }
          if(in_function)
              local_symtab.add((yyvsp[0].name),current_decl_type);
          else
              global_symtab.add((yyvsp[0].name),current_decl_type);
      }
#line 1486 "parser_new.tab.cpp"
    break;

  case 18: /* id_list: id_list COMMA NAME  */
#line 298 "parser_new.y"
      {
          if(strcmp((yyvsp[0].name), "main") == 0) {
              cout << "Semantic error: variable cannot be named main" << endl;
              exit(1);
          }
          if(in_function)
              local_symtab.add((yyvsp[0].name),current_decl_type);
          else
              global_symtab.add((yyvsp[0].name),current_decl_type);
      }
#line 1501 "parser_new.tab.cpp"
    break;

  case 19: /* $@1: %empty  */
#line 312 "parser_new.y"
    {
        current_function_name = string((yyvsp[-1].name));
        in_function = true;
        local_symtab.table.clear();
        current_func_params.clear();  /* Clear parameters for new function */
    }
#line 1512 "parser_new.tab.cpp"
    break;

  case 20: /* func_header: type NAME LEFT_ROUND_BRACKET $@1 param_list_opt RIGHT_ROUND_BRACKET  */
#line 319 "parser_new.y"
    {
        (yyval.type) = (yyvsp[-5].type);
    }
#line 1520 "parser_new.tab.cpp"
    break;

  case 21: /* func_decl: func_header SEMICOLON  */
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

        FunctionInfo func_info(fname, (yyvsp[-1].type), false);
        
        // Pre-allocate return label for non-void functions at declaration time
        // This ensures labels are allocated in declaration order
        if((yyvsp[-1].type) != TYPE_VOID && (show_tac || show_rtl)) {
            Label_TAC_Opd* ret_label = TAC_Generator::get_instance()->create_new_label();
            func_info.return_label_id = ret_label->get_label_id();
            delete ret_label;
        }
        
        function_table.push_back(func_info);

        if(fname == "main"){
            main_seen = true;
        }
        
        in_function = false;  /* Reset after function declaration */
    }
#line 1554 "parser_new.tab.cpp"
    break;

  case 22: /* $@2: %empty  */
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
            function_table.push_back(FunctionInfo(fname, (yyvsp[0].type), true));
        }
    }
#line 1580 "parser_new.tab.cpp"
    break;

  case 23: /* func_def: func_header $@2 block  */
#line 381 "parser_new.y"
    {
        if(current_function_name == "main"){
            if((yyvsp[-2].type) != TYPE_VOID && (yyvsp[-2].type) != TYPE_INT){
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
            fprintf(ast_file, "  Return Type: <%s>\n", data_type_to_string(int_to_datatype((yyvsp[-2].type))));
            fprintf(ast_file, "  Formal Parameters:\n");
            /* Print formal parameters */
            for(auto &param : current_func_params) {
                fprintf(ast_file, "      %s_  Type:<%s>\n", param.name.c_str(), data_type_to_string(int_to_datatype(param.type)));
            }
            fprintf(ast_file, "**BEGIN: Abstract Syntax Tree\n");
            if((yyvsp[0].block)) (yyvsp[0].block)->print(2);
            fprintf(ast_file, "\n**END: Abstract Syntax Tree\n");
        }

        /* Generate  for the function body */
        if((yyvsp[0].block)) {
            list<TAC_Stmt*> tac_stmts;
            int ret_type = (yyvsp[-2].type);
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
                (yyvsp[0].block)->set_return_label_id(ret_label_id);
            }
            
            if(show_tac || show_rtl) {
                // Store function body for deferred TAC generation (after all pre_allocate_temps)
                DeferredFunction df;
                df.name = current_function_name;
                df.return_type = ret_type;
                df.body = (yyvsp[0].block);
                df.return_label_id = ret_label_id;
                deferred_functions.push_back(df);
            } else {
                delete (yyvsp[0].block);
            }
        }

        in_function = false;
    }
#line 1665 "parser_new.tab.cpp"
    break;

  case 24: /* param_list_opt: param_list  */
#line 464 "parser_new.y"
                 { (yyval.ast) = (yyvsp[0].ast); }
#line 1671 "parser_new.tab.cpp"
    break;

  case 25: /* param_list_opt: %empty  */
#line 465 "parser_new.y"
                  { (yyval.ast) = NULL; }
#line 1677 "parser_new.tab.cpp"
    break;

  case 26: /* param_list: param  */
#line 469 "parser_new.y"
            { (yyval.ast) = (yyvsp[0].ast); }
#line 1683 "parser_new.tab.cpp"
    break;

  case 27: /* param_list: param_list COMMA param  */
#line 471 "parser_new.y"
      {
          // For new AST, params are handled differently
          // Just return the first param for now
          // Full param list support requires compound statement structure
          (yyval.ast) = (yyvsp[-2].ast);
      }
#line 1694 "parser_new.tab.cpp"
    break;

  case 28: /* param: type NAME  */
#line 481 "parser_new.y"
      {
          // Check for char parameters - this is not allowed
          if((yyvsp[-1].type) == TYPE_CHAR) {
              cout << "Semantic error: cant parse" << endl;
              exit(1);
          }
          if(in_function) {
              local_symtab.add((yyvsp[0].name),(yyvsp[-1].type));
              /* Also add to current_func_params for AST printing */
              current_func_params.push_back(FuncParam{string((yyvsp[0].name)), (yyvsp[-1].type)});
          }
          (yyval.ast) = NULL;  /* Placeholder for now */
      }
#line 1712 "parser_new.tab.cpp"
    break;

  case 29: /* arg_list_opt: arg_list  */
#line 497 "parser_new.y"
               { (yyval.ast) = (yyvsp[0].ast); }
#line 1718 "parser_new.tab.cpp"
    break;

  case 30: /* arg_list_opt: %empty  */
#line 498 "parser_new.y"
                  { (yyval.ast) = NULL; }
#line 1724 "parser_new.tab.cpp"
    break;

  case 31: /* arg_list: expr  */
#line 503 "parser_new.y"
      {
          FunctionCall_Expr_Ast *tmp = new FunctionCall_Expr_Ast("__tmp__");
          tmp->add_argument((yyvsp[0].expr));
          (yyval.ast) = tmp;
      }
#line 1734 "parser_new.tab.cpp"
    break;

  case 32: /* arg_list: arg_list COMMA expr  */
#line 509 "parser_new.y"
      {
          FunctionCall_Expr_Ast *tmp =
              dynamic_cast<FunctionCall_Expr_Ast*>((yyvsp[-2].ast));
          tmp->add_argument((yyvsp[0].expr));
          (yyval.ast) = tmp;
      }
#line 1745 "parser_new.tab.cpp"
    break;

  case 33: /* block: LEFT_CURLY_BRACKET decl_list_opt stmt_list RIGHT_CURLY_BRACKET  */
#line 519 "parser_new.y"
    {
        (yyval.block) = (yyvsp[-1].block);  /* Return the statement list as a compound statement */
    }
#line 1753 "parser_new.tab.cpp"
    break;

  case 36: /* stmt_list: stmt_list stmt  */
#line 531 "parser_new.y"
    {
        if((yyvsp[-1].block) == NULL) {
            (yyval.block) = new Compound_Stmt();
            if((yyvsp[0].stmt)) (yyval.block)->add_stmt((yyvsp[0].stmt));
        } else {
            (yyval.block)->add_stmt((yyvsp[0].stmt));
        }
    }
#line 1766 "parser_new.tab.cpp"
    break;

  case 37: /* stmt_list: %empty  */
#line 540 "parser_new.y"
    {
        (yyval.block) = new Compound_Stmt();
    }
#line 1774 "parser_new.tab.cpp"
    break;

  case 38: /* stmt: assign_stmt SEMICOLON  */
#line 546 "parser_new.y"
                            { (yyval.stmt) = (yyvsp[-1].stmt); }
#line 1780 "parser_new.tab.cpp"
    break;

  case 39: /* stmt: read_stmt SEMICOLON  */
#line 547 "parser_new.y"
                          { (yyval.stmt) = (yyvsp[-1].stmt); }
#line 1786 "parser_new.tab.cpp"
    break;

  case 40: /* stmt: write_stmt SEMICOLON  */
#line 548 "parser_new.y"
                           { (yyval.stmt) = (yyvsp[-1].stmt); }
#line 1792 "parser_new.tab.cpp"
    break;

  case 41: /* stmt: if_stmt  */
#line 549 "parser_new.y"
              { (yyval.stmt) = (yyvsp[0].stmt); }
#line 1798 "parser_new.tab.cpp"
    break;

  case 42: /* stmt: while_stmt  */
#line 550 "parser_new.y"
                 { (yyval.stmt) = (yyvsp[0].stmt); }
#line 1804 "parser_new.tab.cpp"
    break;

  case 43: /* stmt: do_while_stmt  */
#line 551 "parser_new.y"
                    { (yyval.stmt) = (yyvsp[0].stmt); }
#line 1810 "parser_new.tab.cpp"
    break;

  case 44: /* stmt: return_stmt SEMICOLON  */
#line 552 "parser_new.y"
                            { (yyval.stmt) = (yyvsp[-1].stmt); }
#line 1816 "parser_new.tab.cpp"
    break;

  case 45: /* stmt: func_call_stmt SEMICOLON  */
#line 553 "parser_new.y"
                               { (yyval.stmt) = (yyvsp[-1].stmt); }
#line 1822 "parser_new.tab.cpp"
    break;

  case 46: /* stmt: block  */
#line 554 "parser_new.y"
            { (yyval.stmt) = (yyvsp[0].block); }
#line 1828 "parser_new.tab.cpp"
    break;

  case 47: /* if_stmt: IF LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET stmt  */
#line 559 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: if condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new If_Stmt((yyvsp[-2].expr), (yyvsp[0].stmt), NULL);
    }
#line 1840 "parser_new.tab.cpp"
    break;

  case 48: /* if_stmt: IF LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET stmt ELSE stmt  */
#line 567 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[-4].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: if condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new If_Stmt((yyvsp[-4].expr), (yyvsp[-2].stmt), (yyvsp[0].stmt));
    }
#line 1852 "parser_new.tab.cpp"
    break;

  case 49: /* while_stmt: WHILE LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET stmt  */
#line 578 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: while condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new While_Stmt((yyvsp[-2].expr), (yyvsp[0].stmt));
    }
#line 1864 "parser_new.tab.cpp"
    break;

  case 50: /* do_while_stmt: DO stmt WHILE LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET SEMICOLON  */
#line 589 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: do-while condition must be bool" << endl;
            exit(1);
        }
        (yyval.stmt) = new Do_While_Stmt((yyvsp[-5].stmt), (yyvsp[-2].expr));
    }
#line 1876 "parser_new.tab.cpp"
    break;

  case 51: /* assign_stmt: NAME ASSIGN_OP expr  */
#line 600 "parser_new.y"
    {
        int lhs_type = lookup((yyvsp[-2].name));
        int rhs_type = datatype_to_int((yyvsp[0].expr)->get_data_type());

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

        string lhs_name_with_underscore = string((yyvsp[-2].name)) + "_";
        (yyval.stmt) = new Assignment_Stmt(lhs_name_with_underscore, (yyvsp[0].expr));
        (yyval.stmt)->set_data_type(int_to_datatype(lhs_type));
    }
#line 1901 "parser_new.tab.cpp"
    break;

  case 52: /* read_stmt: READ NAME  */
#line 624 "parser_new.y"
    {
        int var_type = lookup((yyvsp[0].name));
        string var_name_with_underscore = string((yyvsp[0].name)) + "_";
        (yyval.stmt) = new Read_Stmt(var_name_with_underscore);
        (yyval.stmt)->set_data_type(int_to_datatype(var_type));
    }
#line 1912 "parser_new.tab.cpp"
    break;

  case 53: /* write_stmt: WRITE expr  */
#line 634 "parser_new.y"
    {
        if((yyvsp[0].expr)->get_data_type() == BOOL_DATA_TYPE){
            cout << "Semantic error: cannot print bool" << endl;
            exit(1);
        }

        (yyval.stmt) = new Print_Stmt((yyvsp[0].expr));
    }
#line 1925 "parser_new.tab.cpp"
    break;

  case 54: /* return_stmt: RETURN expr  */
#line 646 "parser_new.y"
    {
        (yyval.stmt) = new Return_Stmt((yyvsp[0].expr));
    }
#line 1933 "parser_new.tab.cpp"
    break;

  case 55: /* func_call_stmt: NAME LEFT_ROUND_BRACKET arg_list_opt RIGHT_ROUND_BRACKET  */
#line 653 "parser_new.y"
    {
        FunctionCall_Stmt *call = new FunctionCall_Stmt((yyvsp[-3].name));

        if((yyvsp[-1].ast)){
            FunctionCall_Expr_Ast *tmp =
                dynamic_cast<FunctionCall_Expr_Ast*>((yyvsp[-1].ast));

            for(auto arg : tmp->get_arguments()){
                call->add_argument(arg);
            }

            tmp->get_arguments().clear();
            delete tmp;
        }

        bool found = false;
        for(auto &f : function_table){
            if(f.name == string((yyvsp[-3].name))){
                found = true;
                break;
            }
        }

        if(!found){
            cout << "Semantic error: function not declared " << (yyvsp[-3].name) << endl;
            exit(1);
        }

        (yyval.stmt) = call;
    }
#line 1968 "parser_new.tab.cpp"
    break;

  case 56: /* expr: expr PLUS expr  */
#line 687 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[-2].expr)->get_data_type()),
            datatype_to_int((yyvsp[0].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::PLUS_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    }
#line 1988 "parser_new.tab.cpp"
    break;

  case 57: /* expr: expr MINUS expr  */
#line 703 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[-2].expr)->get_data_type()),
            datatype_to_int((yyvsp[0].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::MINUS_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    }
#line 2008 "parser_new.tab.cpp"
    break;

  case 58: /* expr: expr MULT expr  */
#line 719 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[-2].expr)->get_data_type()),
            datatype_to_int((yyvsp[0].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::MULT_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    }
#line 2028 "parser_new.tab.cpp"
    break;

  case 59: /* expr: expr DIV expr  */
#line 735 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int((yyvsp[-2].expr)->get_data_type()),
            datatype_to_int((yyvsp[0].expr)->get_data_type())
        );

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::DIV_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(int_to_datatype(result_type));
    }
#line 2048 "parser_new.tab.cpp"
    break;

  case 60: /* expr: expr GREATER_THAN expr  */
#line 751 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != datatype_to_int((yyvsp[0].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::GT_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2068 "parser_new.tab.cpp"
    break;

  case 61: /* expr: expr LESS_THAN expr  */
#line 767 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != datatype_to_int((yyvsp[0].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::LT_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2088 "parser_new.tab.cpp"
    break;

  case 62: /* expr: expr GREATER_THAN_EQUAL expr  */
#line 783 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != datatype_to_int((yyvsp[0].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::GE_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2108 "parser_new.tab.cpp"
    break;

  case 63: /* expr: expr LESS_THAN_EQUAL expr  */
#line 799 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != datatype_to_int((yyvsp[0].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::LE_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2128 "parser_new.tab.cpp"
    break;

  case 64: /* expr: expr EQUAL expr  */
#line 815 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != datatype_to_int((yyvsp[0].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::EQ_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2148 "parser_new.tab.cpp"
    break;

  case 65: /* expr: expr NOT_EQUAL expr  */
#line 831 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[-2].expr)->get_data_type())) || 
           !isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != datatype_to_int((yyvsp[0].expr)->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::NE_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2168 "parser_new.tab.cpp"
    break;

  case 66: /* expr: expr AND expr  */
#line 847 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != TYPE_BOOL || 
           datatype_to_int((yyvsp[0].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical AND requires bool operands" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::AND_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2183 "parser_new.tab.cpp"
    break;

  case 67: /* expr: expr OR expr  */
#line 858 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != TYPE_BOOL || 
           datatype_to_int((yyvsp[0].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical OR requires bool operands" << endl;
            exit(1);
        }

        (yyval.expr) = new Binary_Expr_Ast((yyvsp[-2].expr), Binary_Expr_Ast::OR_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2198 "parser_new.tab.cpp"
    break;

  case 68: /* expr: NOT expr  */
#line 869 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[0].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical NOT requires bool operand" << endl;
            exit(1);
        }

        (yyval.expr) = new Unary_Expr_Ast(Unary_Expr_Ast::NOT_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type(BOOL_DATA_TYPE);
    }
#line 2212 "parser_new.tab.cpp"
    break;

  case 69: /* expr: MINUS expr  */
#line 879 "parser_new.y"
    {
        if(!isNumeric(datatype_to_int((yyvsp[0].expr)->get_data_type()))) {
            cout << "Semantic error: unary minus requires numeric operand" << endl;
            exit(1);
        }

        (yyval.expr) = new Unary_Expr_Ast(Unary_Expr_Ast::UMINUS_OP, (yyvsp[0].expr));
        (yyval.expr)->set_data_type((yyvsp[0].expr)->get_data_type());
    }
#line 2226 "parser_new.tab.cpp"
    break;

  case 70: /* expr: expr QUESTION_MARK expr COLON expr  */
#line 889 "parser_new.y"
    {
        if(datatype_to_int((yyvsp[-4].expr)->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: ternary condition must be bool" << endl;
            exit(1);
        }

        if(datatype_to_int((yyvsp[-2].expr)->get_data_type()) != datatype_to_int((yyvsp[0].expr)->get_data_type())) {
            cout << "Semantic error: ternary branches must have same type" << endl;
            exit(1);
        }

        (yyval.expr) = new Ternary_Expr_Ast((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr));
        (yyval.expr)->set_data_type((yyvsp[-2].expr)->get_data_type());
    }
#line 2245 "parser_new.tab.cpp"
    break;

  case 71: /* expr: LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET  */
#line 904 "parser_new.y"
    {
        (yyval.expr) = (yyvsp[-1].expr);
    }
#line 2253 "parser_new.tab.cpp"
    break;

  case 72: /* expr: NAME LEFT_ROUND_BRACKET arg_list_opt RIGHT_ROUND_BRACKET  */
#line 908 "parser_new.y"
    {
        FunctionCall_Expr_Ast *call = new FunctionCall_Expr_Ast((yyvsp[-3].name));

        if((yyvsp[-1].ast)){
            FunctionCall_Expr_Ast *tmp =
                dynamic_cast<FunctionCall_Expr_Ast*>((yyvsp[-1].ast));

            for(auto arg : tmp->get_arguments()){
                call->add_argument(arg);
            }

            tmp->get_arguments().clear();
            delete tmp;
        }

        int ret_type = TYPE_VOID;
        bool found = false;

        for(auto &f : function_table){
            if(f.name == string((yyvsp[-3].name))){
                ret_type = f.return_type;
                found = true;
                break;
            }
        }

        if(!found){
            cout << "Semantic error: function not declared " << (yyvsp[-3].name) << endl;
            exit(1);
        }

        (yyval.expr) = call;
        (yyval.expr)->set_data_type(int_to_datatype(ret_type));
    }
#line 2292 "parser_new.tab.cpp"
    break;

  case 73: /* expr: NAME  */
#line 944 "parser_new.y"
    {
        int var_type = lookup((yyvsp[0].name));
        string name_with_underscore = string((yyvsp[0].name)) + "_";
        (yyval.expr) = new Name_Expr_Ast(name_with_underscore);
        (yyval.expr)->set_data_type(int_to_datatype(var_type));
    }
#line 2303 "parser_new.tab.cpp"
    break;

  case 74: /* expr: INT_NUM  */
#line 951 "parser_new.y"
    {
        // Handle integer overflow with signed wrapping
        long long val = strtoll((yyvsp[0].str), NULL, 10);
        int adjusted = (int)val;  // Automatic two's complement wrapping
        char buf[128];
        snprintf(buf, sizeof(buf), "%d", adjusted);
        (yyval.expr) = new Const_Expr_Ast(string(buf), INT_DATA_TYPE);
    }
#line 2316 "parser_new.tab.cpp"
    break;

  case 75: /* expr: FLOAT_NUM  */
#line 960 "parser_new.y"
    {
        (yyval.expr) = new Const_Expr_Ast((yyvsp[0].str), FLOAT_DATA_TYPE);
    }
#line 2324 "parser_new.tab.cpp"
    break;

  case 76: /* expr: STR_CONST  */
#line 964 "parser_new.y"
    {
        (yyval.expr) = new Const_Expr_Ast((yyvsp[0].str), STRING_DATA_TYPE);
    }
#line 2332 "parser_new.tab.cpp"
    break;


#line 2336 "parser_new.tab.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 969 "parser_new.y"

