%{
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

%}

/* UNION */

%union {
    int type;
    char* name;
    char* str;
    ASTNode* node;
}


/* TOKENS */

%token INTEGER STRING VOID READ WRITE
%token BOOL FLOAT CHAR

%token <name> NAME
%token <str> INT_NUM FLOAT_NUM STR_CONST

%token ASSIGN_OP COMMA SEMICOLON
%token LEFT_ROUND_BRACKET RIGHT_ROUND_BRACKET
%token LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET

%token PLUS MINUS MULT DIV
%token GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL EQUAL NOT_EQUAL
%token AND OR
%token QUESTION_MARK COLON
%token NOT



/* TYPES */

%type <type> type
%type <node> expr stmt stmt_list block
%type <node> assign_stmt read_stmt write_stmt
%type <node> func_def param_list_opt param_list param


/* PRECEDENCE */

%right QUESTION_MARK COLON
%left OR
%left AND
%right NOT
%left EQUAL NOT_EQUAL
%left GREATER_THAN LESS_THAN GREATER_THAN_EQUAL LESS_THAN_EQUAL
%left PLUS MINUS
%left MULT DIV
%right UMINUS


%%

program
    : globals_var_decls func_decl globals_var_decls func_def
    {
        if(!main_defined){
            cout << "Semantic error:main fn not defined" << endl;
            exit(1);
        }
    }
    | globals_var_decls func_def
    {
        if(!main_defined){
            cout << "Semantic error:main fn not defined" << endl;
            exit(1);
        }
    }
    ;

globals_var_decls
    : globals_var_decls var_decl
    | /* empty */
    ;

var_decl
    : type id_list SEMICOLON
    ;

type
    : INTEGER { $$ = TYPE_INT; current_decl_type = TYPE_INT; }
    | STRING  { $$ = TYPE_STRING; current_decl_type = TYPE_STRING; }
    | BOOL    { $$ = TYPE_BOOL; current_decl_type = TYPE_BOOL; }
    | FLOAT   { $$ = TYPE_FLOAT; current_decl_type = TYPE_FLOAT; }
    | CHAR    { $$ = TYPE_CHAR; current_decl_type = TYPE_CHAR; }
    | VOID    { $$ = TYPE_VOID; current_decl_type = TYPE_VOID; }
    ;

id_list
    : NAME
        {
          if(strcmp($1, "main") == 0) {
              cout << "Semantic error: variable cannot be named main" << endl;
              exit(1);
          }
          if(in_function)
              local_symtab.add($1,current_decl_type);
          else
              global_symtab.add($1,current_decl_type);
      }
    | id_list COMMA NAME
      {
          if(strcmp($3, "main") == 0) {
              cout << "Semantic error: variable cannot be named main" << endl;
              exit(1);
          }
          if(in_function)
              local_symtab.add($3,current_decl_type);
          else
              global_symtab.add($3,current_decl_type);
      }
    ;

func_decl
    : type NAME LEFT_ROUND_BRACKET param_list_opt RIGHT_ROUND_BRACKET SEMICOLON
        {
        if(strcmp($2,"main") != 0 || $1 != TYPE_VOID){
            cout << "Semantic error: only void main allowed" << endl;
            exit(1);
        }

        if(main_seen){
            cout << "Semantic error: multiple main decls" << endl;
            exit(1);
        }

        /* Check for char parameters in declaration and store params */
        if($4 != NULL) {
            ASTNode* param_node = $4;
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
    }
    
    ;

func_def
    : type NAME LEFT_ROUND_BRACKET param_list_opt RIGHT_ROUND_BRACKET
    {
        in_function = true;
        main_def_params.clear();  /* Clear previous definition params */
        local_symtab.table.clear();  /* Clear local symbol table for new function */
        /* Parameters are already added to local_symtab in param rule */
    }
    block
    {

        /* Procedure name */
        if(strcmp($2,"main") != 0 || $1 != TYPE_VOID){
            cout << "Semantic error: only void main allowed" << endl;
            exit(1);
        }

        if(main_defined){
            cout << "Semantic error:multiple main definitions" << endl;
            exit(1);
        }

        /* Check for char parameters in definition and store params */
        if($4 != NULL) {
            ASTNode* param_node = $4;
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
        snprintf(procbuf,sizeof(procbuf),"**PROCEDURE: %s",$2);

        /*Return type */
        const char* typestr =
            ($1==TYPE_INT)?"<int>":
            ($1==TYPE_FLOAT)?"<float>":
            ($1==TYPE_BOOL)?"<bool>":
            ($1==TYPE_STRING)?"<string>":
            ($1==TYPE_CHAR)?"<char>":
            ($1==TYPE_VOID)?"<void>":"";

    char retbuf[128];
    snprintf(retbuf,sizeof(retbuf),"Return Type: %s",typestr);

    ASTNode* returnNode = make_node(retbuf,(DataType)$1,NULL,NULL,NULL);

    ASTNode* paramNode = make_node("Formal Parameters:",(DataType)TYPE_VOID,NULL,NULL,NULL);

    /* Attach parameters to paramNode */
    if($4 != NULL) {
        paramNode->left = $4;
    }

    /*END NODE*/
    ASTNode* endNode = make_node("**END: Abstract Syntax Tree",(DataType)TYPE_VOID,NULL,NULL,NULL);

    ASTNode* beginNode = $7;
    returnNode->right = paramNode;
    paramNode->right = beginNode;

    ASTNode* t = beginNode;
    while(t->right) t = t->right;
    t->right = endNode;

    $$ = make_node(procbuf,(DataType)TYPE_VOID,returnNode,NULL,NULL);

    if(show_ast) print_ast($$,0);
    generate_TAC($$);
    in_function = false;
}
;
param_list_opt
    : param_list
      {
          $$ = $1;
      }
    | /* empty */
      {
          $$ = NULL;
      }
    ;

param_list
    : param
      {
          $$ = $1;
      }
    | param_list COMMA param
      {
          ASTNode* t = $1;
          while(t->right) t = t->right;
          t->right = $3;
          $$ = $1;
      }
    ;

param
    : type NAME
      {
          /* Only add to symbol table if we're in a function definition, not a declaration */
          if(in_function) {
              local_symtab.add($2,$1);
          }
          
          /* Create AST node for this parameter */
          const char* typestr =
              ($1==TYPE_INT)?"<int>":
              ($1==TYPE_FLOAT)?"<float>":
              ($1==TYPE_BOOL)?"<bool>":
              ($1==TYPE_STRING)?"<string>":
              ($1==TYPE_CHAR)?"<char>":"";
          
          char buf[128];
          snprintf(buf, sizeof(buf), "%s_     Type:%s", $2, typestr);
          $$ = make_node(buf, (DataType)$1, NULL, NULL, NULL);
      }
    ;

block
: LEFT_CURLY_BRACKET decl_list_opt stmt_list RIGHT_CURLY_BRACKET
{
    $$ = make_node(
            (char*)"**BEGIN: Abstract Syntax Tree",
            (DataType)TYPE_VOID,
            $3,
            NULL,
            NULL);


}
;


decl_list_opt
    : decl_list_opt var_decl
    | /* empty */
    ;

stmt_list
: stmt_list stmt
{
    if($1==NULL) $$=$2;
    else
    {
        ASTNode* t=$1;
        while(t->right) t=t->right;
        t->right=$2;
        $$=$1;
    }
}
| /* empty */
{
    $$ = NULL;
}
;


stmt
    : assign_stmt SEMICOLON
    | read_stmt SEMICOLON
    | write_stmt SEMICOLON
    ;

assign_stmt
: NAME ASSIGN_OP expr
{
    int lhs = lookup($1);
    int rhs = $3->type;

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

    snprintf(buf,sizeof(buf),"Name : %s_%s", $1, typestr);



    ASTNode* lhsNode =
        make_node(buf,(DataType)lhs,NULL,NULL,NULL);


    ASTNode* closeRHS =
        make_node(")",
            (DataType)TYPE_VOID,
            NULL,NULL,NULL);
    
    ASTNode* tmp = $3;
    while(tmp->right) tmp = tmp->right;
    tmp->right = closeRHS;

    ASTNode* rhsWrapper = 
            make_node("RHS (", (DataType)$3->type,$3,NULL,NULL);

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
    $$ = make_node(
            "Asgn:",
            (DataType)TYPE_VOID,
            lhsWrapper,
            rhsWrapper,
            NULL);
}
;



read_stmt
    : READ NAME
      {
          DataType t = (DataType)lookup($2);

          if(t != TYPE_INT && t != TYPE_FLOAT){
            cout << "Type error: read supports only int and float " << endl;
            exit(1);
        }

          char buf[128];
          snprintf(buf, sizeof(buf),"Read: Name : %s_<%s>", $2, type_to_string(t));

          $$ = make_node(buf, t, NULL, NULL, NULL);
      }
    ;


write_stmt
: WRITE expr
{
    if($2->type == TYPE_BOOL){
        cout << "Type error: write does not support boolean variables" << endl;
        exit(1);
    }

    $$ =
        make_node("Write:",
            (DataType)TYPE_VOID,
            $2,
            NULL,
            NULL);
}
;


expr 
    : expr PLUS expr
{
    if(!isNumeric($1->type) || !isNumeric($3->type))
    {
        cout<<"Type error in +"<<endl;
        exit(1);
    }

    DataType resultType = (DataType)numericResult($1->type,$3->type);

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL = $1;
    while(tempL->right) tempL=tempL->right;
    tempL->right = closeL;

    ASTNode* L =
        make_node("L_Opd (", $1->type, $1,NULL,NULL);


    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR = $3;
    while(tempR->right) tempR=tempR->right;
    tempR->right = closeR;

    ASTNode* R =
        make_node("R_Opd (", $3->type, $3,NULL,NULL);


    /* FIX: dynamic type */
    char buf[128];
    snprintf(buf, sizeof(buf), "Arith: Plus<%s>", type_to_string(resultType));

    $$ = make_node(buf, resultType, L, R, NULL);
}



| expr MINUS expr
{
    DataType resultType = (DataType)numericResult($1->type, $3->type);

    // Create brackets
    ASTNode* closeL = make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);
    ASTNode* closeR = make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);

    // FIX: Traverse to the ABSOLUTE end of the left child
    ASTNode* currL = $1;
    while(currL && currL->right) currL = currL->right;
    if(currL) currL->right = closeL;

    ASTNode* L = make_node("L_Opd (", (DataType)$1->type, $1, NULL, NULL);

    // FIX: Traverse to the ABSOLUTE end of the right child
    ASTNode* currR = $3;
    while(currR && currR->right) currR = currR->right;
    if(currR) currR->right = closeR;

    ASTNode* R = make_node("R_Opd (", (DataType)$3->type, $3, NULL, NULL);

    char buf[128];
    snprintf(buf, sizeof(buf), "Arith: Minus<%s>", type_to_string(resultType));
    $$ = make_node(buf, resultType, L, R, NULL);
}


    | expr MULT expr
{
    if(!isNumeric($1->type) || !isNumeric($3->type))
    {
        cout<<"Type error in *"<<endl;
        exit(1);
    }

    DataType resultType = (DataType)numericResult($1->type,$3->type);

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL = $1;
    while(tempL->right) tempL=tempL->right;
    tempL->right = closeL;

    ASTNode* L =
        make_node("L_Opd (", $1->type, $1,NULL,NULL);


    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR = $3;
    while(tempR->right) tempR=tempR->right;
    tempR->right = closeR;

    ASTNode* R =
        make_node("R_Opd (", $3->type, $3,NULL,NULL);


    char buf[128];
    snprintf(buf, sizeof(buf),"Arith: Mult<%s>", type_to_string(resultType));

    $$ = make_node(buf, resultType, L, R, NULL);
}


    | expr DIV expr
{
    if(!isNumeric($1->type) || !isNumeric($3->type))
    {
        cout<<"Type error in division"<<endl;
        exit(1);
    }

    DataType resultType = (DataType)numericResult($1->type,$3->type);

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL = $1;
    while(tempL->right) tempL=tempL->right;
    tempL->right = closeL;

    ASTNode* L =
        make_node("L_Opd (", $1->type, $1,NULL,NULL);


    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR = $3;
    while(tempR->right) tempR=tempR->right;
    tempR->right = closeR;

    ASTNode* R =
        make_node("R_Opd (", $3->type, $3,NULL,NULL);


    /* FIX: generate correct type string */
    char buf[128];
    snprintf(buf, sizeof(buf), "Arith: Div<%s>", type_to_string(resultType));

    $$ = make_node(buf, resultType, L, R, NULL);
}



    /* RELATIONAL */

    | expr GREATER_THAN expr
{
    if(!isNumeric($1->type) || !isNumeric($3->type))
    {
        cout<<"Type error in >"<<endl;
        exit(1);
    }
    if($1->type != $3->type)
    {
        cout<<"Type error: relational operations require same data types"<<endl;
        exit(1);
    }

    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: GT<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);
}


    | expr LESS_THAN expr
    {
        if(!isNumeric($1->type) || !isNumeric($3->type))
        {
            cout<<"Type error in <"<<endl;
            exit(1);
        }
        if($1->type != $3->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: LT<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    }

    | expr GREATER_THAN_EQUAL expr
    {
        if(!isNumeric($1->type) || !isNumeric($3->type))
        {
            cout<<"Type error in >="<<endl;
            exit(1);
        }
        if($1->type != $3->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: GE<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    }

    | expr LESS_THAN_EQUAL expr
    {
        if(!isNumeric($1->type) || !isNumeric($3->type))
        {
            cout<<"Type error in <="<<endl;
            exit(1);
        }
        if($1->type != $3->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: LE<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);


    }
    | expr EQUAL expr
    {
        if(!isNumeric($1->type) || !isNumeric($3->type))
        {
            cout<<"Type error in =="<<endl;
            exit(1);
        }
        if($1->type != $3->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: EQ<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);
    }

    | expr NOT_EQUAL expr
    {
        if(!isNumeric($1->type) || !isNumeric($3->type))
        {
            cout<<"Type error in !="<<endl;
            exit(1);
        }
        if($1->type != $3->type)
        {
            cout<<"Type error: relational operations require same data types"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: NE<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    }



    /* LOGICAL */

    | expr AND expr
    {
        if($1->type!=TYPE_BOOL || $3->type!=TYPE_BOOL)
        {
            cout<<"Type error in &&"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: AND<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);

    }

    | expr OR expr
    {
        if($1->type!=TYPE_BOOL || $3->type!=TYPE_BOOL)
        {
            cout<<"Type error in ||"<<endl;
            exit(1);
        }
        
    ASTNode* closeL =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempL=$1;
    while(tempL->right) tempL=tempL->right;
    tempL->right=closeL;

    ASTNode* L=
        make_node("L_Opd (",
            (DataType)$1->type,
            $1,NULL,NULL);

    ASTNode* closeR =
        make_node(")", (DataType)TYPE_VOID, NULL,NULL,NULL);

    ASTNode* tempR=$3;
    while(tempR->right) tempR=tempR->right;
    tempR->right=closeR;

    ASTNode* R=
        make_node("R_Opd (",
            (DataType)$3->type,
            $3,NULL,NULL);

    $$=
        make_node("Condition: OR<bool>",
            (DataType)TYPE_BOOL,
            L,R,NULL);


    }

    /* TERNARY */

    | expr QUESTION_MARK expr COLON expr
    {
        if($1->type != TYPE_BOOL)
        {
            cout<<"Type error in ternary condition"<<endl;
            exit(1);
        }

        DataType resultType;

        if(isNumeric($3->type) && isNumeric($5->type))
            resultType = (DataType)numericResult($3->type,$5->type);
        else if($3->type == $5->type)
            resultType = (DataType)$3->type;
        else
        {
            cout<<"Type error in ternary branches"<<endl;
            exit(1);
        }

        ASTNode* closeTrue =
    make_node(")",
            (DataType)TYPE_VOID,
            NULL,NULL,NULL);

ASTNode* tempT=$3;
while(tempT->right) tempT=tempT->right;
tempT->right=closeTrue;

ASTNode* truePart =
    make_node("True_Part (",
            resultType,
            $3,
            NULL,
            NULL);


        ASTNode* closeFalse =
    make_node(")",
            (DataType)TYPE_VOID,
            NULL,NULL,NULL);

    ASTNode* tempF=$5;
    while(tempF->right) tempF=tempF->right;
    tempF->right=closeFalse;

    ASTNode* falsePart =
        make_node("False_Part (",
                resultType,
                $5,
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
        ASTNode* ternary_node = make_node("Ternary:", resultType, $1, truePart, falsePart);

        $$ = ternary_node; 

    }

    | NOT expr
    {
        ASTNode* close =
            make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);

        ASTNode* tmp = $2;
        while(tmp->right) tmp = tmp->right;
        tmp->right = close;

        ASTNode* childWrapper =
            make_node("L_Opd (", (DataType)TYPE_BOOL, $2, NULL, NULL);

        $$ = make_node("Condition: NOT<bool>", (DataType)TYPE_BOOL, childWrapper, NULL, NULL);

    }

    | MINUS expr %prec UMINUS
    {
        if(!isNumeric($2->type)){
            cout << "Type error in unary minus" << endl;
            exit(1);
        }

        ASTNode* closeL =
            make_node(")", (DataType)TYPE_VOID, NULL, NULL, NULL);

        ASTNode* temp = $2;
        while(temp->right) temp = temp->right;
        temp->right = closeL;

        ASTNode* L =
            make_node("L_Opd (", (DataType)$2->type, $2, NULL, NULL);

        char buf[128];
        snprintf(buf, sizeof(buf), "Arith: Uminus<%s>", type_to_string((DataType)$2->type));

        $$ = make_node(buf, (DataType)$2->type, L, NULL, NULL);
    }


    | NAME
    {
        int t = lookup($1);

        char buf[128];
        const char* typestr =
            (t==TYPE_INT)?"<int>":
            (t==TYPE_FLOAT)?"<float>":
            (t==TYPE_BOOL)?"<bool>":
            (t==TYPE_STRING)?"<string>":"";

        snprintf(buf,sizeof(buf),"Name : %s_%s", $1, typestr);



        $$ = make_node(buf,(DataType)t,NULL,NULL,NULL);
    }


    | INT_NUM
    {
        char buf[128];
        long long val = strtoll($1, NULL, 10);
        
        /* Adjust for overflow using two's complement wrapping */
        int adjusted = (int)val;  /* This automatically wraps for out-of-range values */
        
        /* Display the adjusted value in the AST */
        snprintf(buf, sizeof(buf), "Num : %d<int>", adjusted);
        $$ = make_node(
                strdup(buf),
                (DataType)TYPE_INT,
                NULL,NULL,NULL);
    }


    | FLOAT_NUM
    {
        char buf[128];
        double val = atof($1);
        
        /* Adjust for overflow by clamping to float range */
        float adjusted = (float)val;  /* Conversion handles overflow automatically */
        if(val > FLT_MAX) adjusted = FLT_MAX;
        else if(val < -FLT_MAX) adjusted = -FLT_MAX;
        
        snprintf(buf, sizeof(buf), "Num : %.2f<float>", adjusted);
        $$ = make_node(
                strdup(buf),
                (DataType)TYPE_FLOAT,
                NULL,NULL,NULL);
    }


    | STR_CONST
    {
        char buf[128];
        snprintf(buf, sizeof(buf), "String : %s<string>", $1);
        $$ = make_node(
                strdup(buf),
                (DataType)TYPE_STRING,
                NULL,NULL,NULL);
    }


    | LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET
    {
        $$=$2;
    }
;



%%

