#ifndef AST_NEW_H
#define AST_NEW_H

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <list>
#include <vector>
#include <utility>
#include <cstdint>

using std::string;
using std::list;

extern FILE *ast_file;
extern int show_ast;

/* ============================================================================
   DATA TYPES AND ENUMERATIONS
   ============================================================================ */

typedef enum {
    INT_DATA_TYPE,
    FLOAT_DATA_TYPE,
    BOOL_DATA_TYPE,
    STRING_DATA_TYPE,
    CHAR_DATA_TYPE,
    VOID_DATA_TYPE,
    ERROR_DATA_TYPE,
} DataType;

/* TAC Operand Forward Declarations */
class TAC_Opd;
class Label_TAC_Opd;

/* ============================================================================
   BASE AST CLASS
   ============================================================================ */

class Ast {
public:
    DataType node_data_type;
    
    // Temp allocation tracking (for your pre-processing system)
    int temp_id;
    int stemp_id;
    int not_temp_id;
    
    Ast();
    virtual ~Ast();
    
    void set_data_type(DataType d);
    DataType get_data_type();
    
    virtual void print(int indent = 0) = 0;
    virtual TAC_Opd* generate_tac(list<class TAC_Stmt*>& statements) = 0;
};

/* ============================================================================
   EXPRESSION AST CLASSES
   ============================================================================ */

class Expression_Ast : public Ast {
public:
    Expression_Ast();
    virtual ~Expression_Ast();
    
    // Pre-allocate temp IDs for this expression tree
    // This ensures temps are assigned in the correct order matching old-compiler
    virtual void pre_allocate_temps() {}
};

/* Constant Expression (numbers, strings) */
class Const_Expr_Ast : public Expression_Ast {
private:
    string value;
    
public:
    Const_Expr_Ast(string val, DataType type);
    ~Const_Expr_Ast();
    
    string get_value();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Variable Name */
class Name_Expr_Ast : public Expression_Ast {
private:
    string name;
    
public:
    Name_Expr_Ast(string var_name);
    ~Name_Expr_Ast();
    
    string get_name();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Unary Expressions (!, -) */
class Unary_Expr_Ast : public Expression_Ast {
public:
    enum UnaryOp {
        NOT_OP,
        UMINUS_OP
    };
    
private:
    UnaryOp op;
    Expression_Ast* operand;
    int temp_id = -1;  // Pre-allocated temp ID
    
public:
    Unary_Expr_Ast(UnaryOp operation, Expression_Ast* opd);
    ~Unary_Expr_Ast();
    
    Expression_Ast* get_operand() { return operand; }
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Binary Expressions (+, -, *, /, <, >, <=, >=, ==, !=, &&, ||) */
class Binary_Expr_Ast : public Expression_Ast {
public:
    enum BinaryOp {
        PLUS_OP,
        MINUS_OP,
        MULT_OP,
        DIV_OP,
        LT_OP,      // <
        GT_OP,      // >
        LE_OP,      // <=
        GE_OP,      // >=
        EQ_OP,      // ==
        NE_OP,      // !=
        AND_OP,
        OR_OP
    };
    
    Expression_Ast* left;
    BinaryOp op;
    Expression_Ast* right;
    int temp_id = -1;  // Pre-allocated temp ID
    
public:
    Binary_Expr_Ast(Expression_Ast* l, BinaryOp operation, Expression_Ast* r);
    ~Binary_Expr_Ast();
    
    Expression_Ast* get_left_operand() { return left; }
    Expression_Ast* get_right_operand() { return right; }
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Ternary Expression (condition ? true_expr : false_expr) */
class Ternary_Expr_Ast : public Expression_Ast {
private:
    Expression_Ast* condition;
    Expression_Ast* true_expr;
    Expression_Ast* false_expr;
    int not_temp_id = -1;  // Pre-allocated NOT temp ID
    uint32_t false_label_id = 0;  // Pre-allocated false label ID
    uint32_t end_label_id = 0;    // Pre-allocated end label ID
    int result_stemp_id = -1;     // Pre-allocated stemp ID for ternary result
    
public:
    Ternary_Expr_Ast(Expression_Ast* cond, Expression_Ast* true_e, Expression_Ast* false_e);
    ~Ternary_Expr_Ast();
    
    Expression_Ast* get_condition() { return condition; }
    Expression_Ast* get_true_expr() { return true_expr; }
    Expression_Ast* get_false_expr() { return false_expr; }
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Function Call Expression (function_name(arg1, arg2, ...)) */
class FunctionCall_Expr_Ast : public Expression_Ast {
private:
    string function_name;
    list<Expression_Ast*> arguments;
    int temp_id = -1;  // Pre-allocated temp ID for return value
    
public:
    FunctionCall_Expr_Ast(string fn_name);
    ~FunctionCall_Expr_Ast();
    
    string get_function_name() { return function_name; }
    void add_argument(Expression_Ast* arg);
    list<Expression_Ast*>& get_arguments() { return arguments; }
    int get_temp_id() { return temp_id; }  // Get pre-allocated temp ID
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* ============================================================================
   STATEMENT AST CLASSES
   ============================================================================ */

class Statement_Ast : public Ast {
public:
    Statement_Ast();
    virtual ~Statement_Ast();
    
    // Pre-allocate temp IDs for expressions in this statement
    virtual void pre_allocate_temps() {}
};

/* Assignment Statement */
class Assignment_Stmt : public Statement_Ast {
private:
    string lhs_name;
    Expression_Ast* rhs;
    
public:
    Assignment_Stmt(string var_name, Expression_Ast* rhs_expr);
    ~Assignment_Stmt();
    
    void pre_allocate_temps();
    void print(int indent = 0);
    void print_no_trailing_newline(int indent = 0);  // Print without final newline
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Read Statement */
class Read_Stmt : public Statement_Ast {
private:
    string var_name;
    
public:
    Read_Stmt(string name);
    ~Read_Stmt();
    
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Print/Write Statement */
class Print_Stmt : public Statement_Ast {
private:
    Expression_Ast* expr;
    
public:
    Print_Stmt(Expression_Ast* e);
    ~Print_Stmt();
    
    void pre_allocate_temps();
    void print(int indent = 0);
    void print_inline(int indent = 0);  // Print without trailing newline
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* If/Else Statement */
class If_Stmt : public Statement_Ast {
private:
    Expression_Ast* condition;
    Statement_Ast* then_stmt;
    Statement_Ast* else_stmt;
    uint32_t else_label_id = 0;
    uint32_t end_label_id = 0;
    int not_temp_id = -1;
    
public:
    If_Stmt(Expression_Ast* cond, Statement_Ast* then_s, Statement_Ast* else_s = NULL);
    ~If_Stmt();
    
    Statement_Ast* get_then_stmt() { return then_stmt; }
    Statement_Ast* get_else_stmt() { return else_stmt; }
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* While Loop */
class While_Stmt : public Statement_Ast {
private:
    Expression_Ast* condition;
    Statement_Ast* body;
    uint32_t loop_label_id = 0;
    uint32_t exit_label_id = 0;
    int not_temp_id = -1;
    
public:
    While_Stmt(Expression_Ast* cond, Statement_Ast* body_stmt);
    ~While_Stmt();
    
    Statement_Ast* get_body() { return body; }
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Do-While Loop */
class Do_While_Stmt : public Statement_Ast {
private:
    Statement_Ast* body;
    Expression_Ast* condition;
    uint32_t loop_label_id = 0;
    
public:
    Do_While_Stmt(Statement_Ast* body_stmt, Expression_Ast* cond);
    ~Do_While_Stmt();
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Compound Statement (block with multiple statements) */
class Compound_Stmt : public Statement_Ast {
private:
    list<Statement_Ast*> statements;
    int return_stemp_id = -1;  // For tracking shared return stemp in functions
    int return_label_id = -1;  // For tracking return label in functions (-1 = not set)
    
public:
    Compound_Stmt();
    ~Compound_Stmt();
    
    void add_stmt(Statement_Ast* stmt);
    list<Statement_Ast*>& get_statements() { return statements; }
    
    void set_return_stemp_id(int id) { return_stemp_id = id; }
    int get_return_stemp_id() { return return_stemp_id; }
    void set_return_label_id(int id);  // Defined in ast_new.cpp
    int get_return_label_id() { return return_label_id; }
    
    // Helper to propagate return stemp ID to all Return_Stmt nodes recursively
    void propagate_return_stemp_to_all_returns(Statement_Ast* stmt, int stemp_id);
    // Helper to propagate return label ID to all Return_Stmt nodes recursively
    void propagate_return_label_to_all_returns(Statement_Ast* stmt, int label_id);
    
    void pre_allocate_temps();
    void print(int indent = 0);
    void print_inline_if_single_print(int indent);  // Print inline if single Print_Stmt
    void print_with_last_no_newline(int indent);  // Print all stmts, last one without trailing newline
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Return Statement */
class Return_Stmt : public Statement_Ast {
private:
    Expression_Ast* return_expr;  // NULL if returning void
    int return_label_id = -1;  // Label to jump to for return (-1 = not set)
    int return_stemp_id = -1;  // Shared stemp ID from function (-1 = not allocated)
    
public:
    Return_Stmt(Expression_Ast* expr = NULL);
    ~Return_Stmt();
    
    Expression_Ast* get_return_expr() { return return_expr; }
    void set_return_label_id(int label_id) { return_label_id = label_id; }
    int get_return_label_id() { return return_label_id; }
    
    void set_return_stemp_id(int id) { return_stemp_id = id; }
    int get_return_stemp_id() { return return_stemp_id; }
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Function Call Statement */
class FunctionCall_Stmt : public Statement_Ast {
private:
    string function_name;
    list<Expression_Ast*> arguments;
    
public:
    FunctionCall_Stmt(string fn_name);
    ~FunctionCall_Stmt();
    
    string get_function_name() { return function_name; }
    void add_argument(Expression_Ast* arg);
    list<Expression_Ast*>& get_arguments() { return arguments; }
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* Function Definition Statement */
class FunctionDef_Stmt : public Statement_Ast {
private:
    string function_name;
    list<std::pair<string, DataType>> parameters;  // (param_name, param_type)
    Compound_Stmt* body;
    uint32_t return_label_id = 0;  // Label for the return point
    int return_stemp_id = -1;  // Shared stemp for all returns (-1 = not allocated)
    
public:
    FunctionDef_Stmt(string fn_name, DataType ret_type);
    ~FunctionDef_Stmt();
    
    string get_function_name() { return function_name; }
    void add_parameter(string param_name, DataType param_type);
    list<std::pair<string, DataType>>& get_parameters() { return parameters; }
    void set_body(Compound_Stmt* function_body);
    Compound_Stmt* get_body() { return body; }
    
    void set_return_label_id(int label_id) { return_label_id = label_id; }
    int get_return_label_id() { return return_label_id; }
    
    void set_return_stemp_id(int id) { return_stemp_id = id; }
    int get_return_stemp_id() { return return_stemp_id; }
    
    // Helper method to set return stemp ID in all Return_Stmt nodes in the body
    void propagate_return_stemp_id(Statement_Ast* stmt);
    
    void pre_allocate_temps();
    void print(int indent = 0);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements);
};

/* ============================================================================
   UTILITY FUNCTIONS
   ============================================================================ */

void print_ast(Ast* root);
const char* data_type_to_string(DataType t);

// Helper to check if stmt is a Compound_Stmt with single Print_Stmt
Print_Stmt* get_single_print_stmt(Statement_Ast* stmt);

/* For compatibility with old code */
typedef Ast ASTNode;

#endif