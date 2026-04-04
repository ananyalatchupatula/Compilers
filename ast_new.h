#ifndef AST_NEW_H
#define AST_NEW_H

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <list>
#include <vector>
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
    
public:
    Compound_Stmt();
    ~Compound_Stmt();
    
    void add_stmt(Statement_Ast* stmt);
    list<Statement_Ast*>& get_statements() { return statements; }
    void pre_allocate_temps();
    void print(int indent = 0);
    void print_inline_if_single_print(int indent);  // Print inline if single Print_Stmt
    void print_with_last_no_newline(int indent);  // Print all stmts, last one without trailing newline
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