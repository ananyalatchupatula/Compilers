%{
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
    vector<FuncParam> parameters;  // List of parameter names
};
vector<DeferredFunction> deferred_functions;

/* Global variable tracking */
struct GlobalVar {
    string name;
    int type;
};
vector<GlobalVar> global_vars;

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

%}

/* UNION */
%union {
    int type;
    char* name;
    char* str;
    Ast* ast;
    Expression_Ast* expr;
    Statement_Ast* stmt;
    Compound_Stmt* block;
}

/* TOKENS */
%token INTEGER STRING VOID READ WRITE
%token BOOL FLOAT CHAR
%token IF ELSE WHILE DO RETURN

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
%type <type> func_header
%type <expr> expr
%type <stmt> stmt assign_stmt read_stmt write_stmt if_stmt while_stmt do_while_stmt return_stmt func_call_stmt
%type <block> stmt_list block
%type <ast> param_list_opt param_list param
%type <ast> arg_list_opt arg_list


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
%nonassoc IF
%nonassoc ELSE

%%

program
    : program_items
    {
        if(!main_defined){
            cout << "Semantic error: main fn not defined" << endl;
            exit(1);
        }
    }
    ;

program_items
    : program_items program_item
    | program_item
    ;

program_item
    : global_var_group
    | func_item
    ;

global_var_group
    : type id_list SEMICOLON
    ;

func_item
    : func_decl
    | func_def
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
          else {
              global_symtab.add($1,current_decl_type);
              global_vars.push_back(GlobalVar{string($1), current_decl_type});
          }
      }
    | id_list COMMA NAME
      {
          if(strcmp($3, "main") == 0) {
              cout << "Semantic error: variable cannot be named main" << endl;
              exit(1);
          }
          if(in_function)
              local_symtab.add($3,current_decl_type);
          else {
              global_symtab.add($3,current_decl_type);
              global_vars.push_back(GlobalVar{string($3), current_decl_type});
          }
      }
    ;

func_header
    : type NAME LEFT_ROUND_BRACKET
    {
        current_function_name = string($2);
        in_function = true;
        local_symtab.table.clear();
        current_func_params.clear();  /* Clear parameters for new function */
    }
    param_list_opt RIGHT_ROUND_BRACKET
    {
        $$ = $1;
    }
    ;

func_decl
    : func_header SEMICOLON
    {
        string fname = current_function_name;

        for(auto &f : function_table){
            if(f.name == fname){
                cout << "Semantic error: multiple declaration of function "
                     << fname << endl;
                exit(1);
            }
        }

        FunctionInfo func_info(fname, $1, false);
        
        // Pre-allocate return label for non-void functions at declaration time
        // This ensures labels are allocated in declaration order
        if($1 != TYPE_VOID && (show_tac || show_rtl)) {
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
    ;

func_def
    : func_header
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
            function_table.push_back(FunctionInfo(fname, $1, true));
        }
    }
    block
    {
        if(current_function_name == "main"){
            if($1 != TYPE_VOID && $1 != TYPE_INT){
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
            fprintf(ast_file, "  Return Type: <%s>\n", data_type_to_string(int_to_datatype($1)));
            fprintf(ast_file, "  Formal Parameters:\n");
            /* Print formal parameters */
            for(auto &param : current_func_params) {
                fprintf(ast_file, "      %s_  Type:<%s>\n", param.name.c_str(), data_type_to_string(int_to_datatype(param.type)));
            }
            fprintf(ast_file, "**BEGIN: Abstract Syntax Tree\n");
            if($3) $3->print(2);
            fprintf(ast_file, "\n**END: Abstract Syntax Tree\n");
        }

        /* Generate  for the function body */
        if($3) {
            list<TAC_Stmt*> tac_stmts;
            int ret_type = $1;
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
                $3->set_return_label_id(ret_label_id);
            }
            
            // Always store function body for deferred TAC/RTL/ASM generation
            // This is needed for ASM generation even without show_tac/show_rtl flags
            DeferredFunction df;
            df.name = current_function_name;
            df.return_type = ret_type;
            df.body = $3;
            df.return_label_id = ret_label_id;
            df.parameters = current_func_params;  // Store parameter list
            deferred_functions.push_back(df);
        }

        in_function = false;
    }
    ;

param_list_opt
    : param_list { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;

param_list
    : param { $$ = $1; }
    | param_list COMMA param
      {
          // For new AST, params are handled differently
          // Just return the first param for now
          // Full param list support requires compound statement structure
          $$ = $1;
      }
    ;

param
    : type NAME
      {
          // Check for char parameters - this is not allowed
          if($1 == TYPE_CHAR) {
              cout << "Semantic error: cant parse" << endl;
              exit(1);
          }
          if(in_function) {
              local_symtab.add($2,$1);
              /* Also add to current_func_params for AST printing */
              current_func_params.push_back(FuncParam{string($2), $1});
          }
          $$ = NULL;  /* Placeholder for now */
      }
    ;

arg_list_opt
    : arg_list { $$ = $1; }
    | /* empty */ { $$ = NULL; }
    ;

arg_list
    : expr
      {
          FunctionCall_Expr_Ast *tmp = new FunctionCall_Expr_Ast("__tmp__");
          tmp->add_argument($1);
          $$ = tmp;
      }
    | arg_list COMMA expr
      {
          FunctionCall_Expr_Ast *tmp =
              dynamic_cast<FunctionCall_Expr_Ast*>($1);
          tmp->add_argument($3);
          $$ = tmp;
      }
    ;

block
    : LEFT_CURLY_BRACKET decl_list_opt stmt_list RIGHT_CURLY_BRACKET
    {
        $$ = $3;  /* Return the statement list as a compound statement */
    }
    ;

decl_list_opt
    : decl_list_opt var_decl
    | /* empty */
    ;

stmt_list
    : stmt_list stmt
    {
        if($1 == NULL) {
            $$ = new Compound_Stmt();
            if($2) $$->add_stmt($2);
        } else {
            $$->add_stmt($2);
        }
    }
    | /* empty */
    {
        $$ = new Compound_Stmt();
    }
    ;

stmt
    : assign_stmt SEMICOLON { $$ = $1; }
    | read_stmt SEMICOLON { $$ = $1; }
    | write_stmt SEMICOLON { $$ = $1; }
    | if_stmt { $$ = $1; }
    | while_stmt { $$ = $1; }
    | do_while_stmt { $$ = $1; }
    | return_stmt SEMICOLON { $$ = $1; }
    | func_call_stmt SEMICOLON { $$ = $1; }
    | block { $$ = $1; }
    ;

if_stmt
    : IF LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET stmt %prec IF
    {
        if(datatype_to_int($3->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: if condition must be bool" << endl;
            exit(1);
        }
        $$ = new If_Stmt($3, $5, NULL);
    }
    | IF LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET stmt ELSE stmt
    {
        if(datatype_to_int($3->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: if condition must be bool" << endl;
            exit(1);
        }
        $$ = new If_Stmt($3, $5, $7);
    }
    ;

while_stmt
    : WHILE LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET stmt
    {
        if(datatype_to_int($3->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: while condition must be bool" << endl;
            exit(1);
        }
        $$ = new While_Stmt($3, $5);
    }
    ;

do_while_stmt
    : DO stmt WHILE LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET SEMICOLON
    {
        if(datatype_to_int($5->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: do-while condition must be bool" << endl;
            exit(1);
        }
        $$ = new Do_While_Stmt($2, $5);
    }
    ;

assign_stmt
    : NAME ASSIGN_OP expr
    {
        int lhs_type = lookup($1);
        int rhs_type = datatype_to_int($3->get_data_type());

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

        string lhs_name_with_underscore = string($1) + "_";
        $$ = new Assignment_Stmt(lhs_name_with_underscore, $3);
        $$->set_data_type(int_to_datatype(lhs_type));
    }
    ;

read_stmt
    : READ NAME
    {
        int var_type = lookup($2);
        string var_name_with_underscore = string($2) + "_";
        $$ = new Read_Stmt(var_name_with_underscore);
        $$->set_data_type(int_to_datatype(var_type));
    }
    ;

write_stmt
    : WRITE expr
    {
        if($2->get_data_type() == BOOL_DATA_TYPE){
            cout << "Semantic error: cannot print bool" << endl;
            exit(1);
        }

        $$ = new Print_Stmt($2);
    }
    ;

return_stmt
    : RETURN expr
    {
        $$ = new Return_Stmt($2);
    }
    ;

func_call_stmt
    : NAME LEFT_ROUND_BRACKET arg_list_opt RIGHT_ROUND_BRACKET
    {
        FunctionCall_Stmt *call = new FunctionCall_Stmt($1);

        if($3){
            FunctionCall_Expr_Ast *tmp =
                dynamic_cast<FunctionCall_Expr_Ast*>($3);

            for(auto arg : tmp->get_arguments()){
                call->add_argument(arg);
            }

            tmp->get_arguments().clear();
            delete tmp;
        }

        bool found = false;
        for(auto &f : function_table){
            if(f.name == string($1)){
                found = true;
                break;
            }
        }

        if(!found){
            cout << "Semantic error: function not declared " << $1 << endl;
            exit(1);
        }

        $$ = call;
    }
    ;

expr
    : expr PLUS expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int($1->get_data_type()),
            datatype_to_int($3->get_data_type())
        );

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::PLUS_OP, $3);
        $$->set_data_type(int_to_datatype(result_type));
    }
    | expr MINUS expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int($1->get_data_type()),
            datatype_to_int($3->get_data_type())
        );

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::MINUS_OP, $3);
        $$->set_data_type(int_to_datatype(result_type));
    }
    | expr MULT expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int($1->get_data_type()),
            datatype_to_int($3->get_data_type())
        );

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::MULT_OP, $3);
        $$->set_data_type(int_to_datatype(result_type));
    }
    | expr DIV expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        int result_type = numericResult(
            datatype_to_int($1->get_data_type()),
            datatype_to_int($3->get_data_type())
        );

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::DIV_OP, $3);
        $$->set_data_type(int_to_datatype(result_type));
    }
    | expr GREATER_THAN expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int($1->get_data_type()) != datatype_to_int($3->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::GT_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | expr LESS_THAN expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int($1->get_data_type()) != datatype_to_int($3->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::LT_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | expr GREATER_THAN_EQUAL expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int($1->get_data_type()) != datatype_to_int($3->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::GE_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | expr LESS_THAN_EQUAL expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int($1->get_data_type()) != datatype_to_int($3->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::LE_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | expr EQUAL expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int($1->get_data_type()) != datatype_to_int($3->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::EQ_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | expr NOT_EQUAL expr
    {
        if(!isNumeric(datatype_to_int($1->get_data_type())) || 
           !isNumeric(datatype_to_int($3->get_data_type()))) {
            cout << "Semantic error: operands must be numeric" << endl;
            exit(1);
        }

        if(datatype_to_int($1->get_data_type()) != datatype_to_int($3->get_data_type())) {
            cout << "Semantic error: type mismatch in relational operator" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::NE_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | expr AND expr
    {
        if(datatype_to_int($1->get_data_type()) != TYPE_BOOL || 
           datatype_to_int($3->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical AND requires bool operands" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::AND_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | expr OR expr
    {
        if(datatype_to_int($1->get_data_type()) != TYPE_BOOL || 
           datatype_to_int($3->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical OR requires bool operands" << endl;
            exit(1);
        }

        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::OR_OP, $3);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | NOT expr
    {
        if(datatype_to_int($2->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: logical NOT requires bool operand" << endl;
            exit(1);
        }

        $$ = new Unary_Expr_Ast(Unary_Expr_Ast::NOT_OP, $2);
        $$->set_data_type(BOOL_DATA_TYPE);
    }
    | MINUS expr %prec UMINUS
    {
        if(!isNumeric(datatype_to_int($2->get_data_type()))) {
            cout << "Semantic error: unary minus requires numeric operand" << endl;
            exit(1);
        }

        $$ = new Unary_Expr_Ast(Unary_Expr_Ast::UMINUS_OP, $2);
        $$->set_data_type($2->get_data_type());
    }
    | expr QUESTION_MARK expr COLON expr
    {
        if(datatype_to_int($1->get_data_type()) != TYPE_BOOL) {
            cout << "Semantic error: ternary condition must be bool" << endl;
            exit(1);
        }

        if(datatype_to_int($3->get_data_type()) != datatype_to_int($5->get_data_type())) {
            cout << "Semantic error: ternary branches must have same type" << endl;
            exit(1);
        }

        $$ = new Ternary_Expr_Ast($1, $3, $5);
        $$->set_data_type($3->get_data_type());
    }
    | LEFT_ROUND_BRACKET expr RIGHT_ROUND_BRACKET
    {
        $$ = $2;
    }
    | NAME LEFT_ROUND_BRACKET arg_list_opt RIGHT_ROUND_BRACKET
    {
        FunctionCall_Expr_Ast *call = new FunctionCall_Expr_Ast($1);

        if($3){
            FunctionCall_Expr_Ast *tmp =
                dynamic_cast<FunctionCall_Expr_Ast*>($3);

            for(auto arg : tmp->get_arguments()){
                call->add_argument(arg);
            }

            tmp->get_arguments().clear();
            delete tmp;
        }

        int ret_type = TYPE_VOID;
        bool found = false;

        for(auto &f : function_table){
            if(f.name == string($1)){
                ret_type = f.return_type;
                found = true;
                break;
            }
        }

        if(!found){
            cout << "Semantic error: function not declared " << $1 << endl;
            exit(1);
        }

        $$ = call;
        $$->set_data_type(int_to_datatype(ret_type));
    }

    | NAME
    {
        int var_type = lookup($1);
        string name_with_underscore = string($1) + "_";
        $$ = new Name_Expr_Ast(name_with_underscore);
        $$->set_data_type(int_to_datatype(var_type));
    }
    | INT_NUM
    {
        // Handle integer overflow with signed wrapping
        long long val = strtoll($1, NULL, 10);
        int adjusted = (int)val;  // Automatic two's complement wrapping
        char buf[128];
        snprintf(buf, sizeof(buf), "%d", adjusted);
        $$ = new Const_Expr_Ast(string(buf), INT_DATA_TYPE);
    }
    | FLOAT_NUM
    {
        $$ = new Const_Expr_Ast($1, FLOAT_DATA_TYPE);
    }
    | STR_CONST
    {
        $$ = new Const_Expr_Ast($1, STRING_DATA_TYPE);
    }
    ;

%%
