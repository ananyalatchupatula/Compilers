#ifndef AST_NODE_H
#define AST_NODE_H

#include <string>
#include <vector>
#include <list>
#include <stdio.h>
#include <cstdint>
using namespace std;

// Forward declarations
class TAC_Stmt;
class TAC_Opd;

// Data types for SCLP
enum Data_Type {
    INT_DATA_TYPE = 0,
    FLOAT_DATA_TYPE = 1,
    BOOL_DATA_TYPE = 2,
    STRING_DATA_TYPE = 3,
    VOID_DATA_TYPE = 4,
};

// Base AST Class - All AST nodes inherit from this
class Ast {
public:
    Data_Type node_data_type;
    int lineno;
    
    Ast();
    virtual ~Ast() = default;
    
    // Virtual methods for AST processing
    virtual void print(FILE *file = stdout) = 0;
    virtual Data_Type get_data_type() { return node_data_type; }
    virtual void set_data_type(Data_Type t) { node_data_type = t; }
    
    // TAC generation - returns the TAC operand containing the result
    virtual TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list) = 0;
};

// Expression AST Base Class
class Expression_Ast : public Ast {
public:
    Expression_Ast();
    virtual ~Expression_Ast() = default;
    
    virtual void print(FILE *file = stdout) = 0;
    virtual TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list) = 0;
};

// Unary Expression AST
class Unary_Expr_Ast : public Expression_Ast {
public:
    enum Unary_Op {
        UMINUS_OP,
        NOT_OP
    };
    
private:
    Unary_Op oper;
    Expression_Ast *operand;
    
public:
    Unary_Expr_Ast(Unary_Op op, Expression_Ast *opd);
    ~Unary_Expr_Ast();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    Unary_Op get_op() { return oper; }
    Expression_Ast* get_operand() { return operand; }
};

// Binary Expression AST
class Binary_Expr_Ast : public Expression_Ast {
public:
    enum Binary_Op {
        PLUS_OP,
        MINUS_OP,
        MULT_OP,
        DIV_OP,
        GT_OP,
        GE_OP,
        LT_OP,
        LE_OP,
        EQ_OP,
        NE_OP,
        AND_OP,
        OR_OP
    };
    
private:
    Binary_Op oper;
    Expression_Ast *lhs;
    Expression_Ast *rhs;
    
public:
    Binary_Expr_Ast(Expression_Ast *l, Binary_Op op, Expression_Ast *r);
    ~Binary_Expr_Ast();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    Binary_Op get_op() { return oper; }
    Expression_Ast* get_lhs() { return lhs; }
    Expression_Ast* get_rhs() { return rhs; }
};

// Ternary Expression AST (conditional a ? b : c)
class Ternary_Expr_Ast : public Expression_Ast {
private:
    Expression_Ast *cond;
    Expression_Ast *true_expr;
    Expression_Ast *false_expr;
    
public:
    Ternary_Expr_Ast(Expression_Ast *c, Expression_Ast *t, Expression_Ast *f);
    ~Ternary_Expr_Ast();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    Expression_Ast* get_cond() { return cond; }
    Expression_Ast* get_true_expr() { return true_expr; }
    Expression_Ast* get_false_expr() { return false_expr; }
};

// Constant Expression AST
class Const_Expr_Ast : public Expression_Ast {
private:
    int int_value;
    double float_value;
    string string_value;
    
public:
    Const_Expr_Ast(int val);
    Const_Expr_Ast(double val);
    Const_Expr_Ast(string val);
    ~Const_Expr_Ast();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    int get_int_value() { return int_value; }
    double get_float_value() { return float_value; }
    string get_string_value() { return string_value; }
};

// Name/Variable Expression AST
class Name_Expr_Ast : public Expression_Ast {
private:
    string var_name;
    
public:
    Name_Expr_Ast(string name);
    ~Name_Expr_Ast();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    string get_name() { return var_name; }
};

// Statement AST Base Class
class Statement_Ast : public Ast {
public:
    Statement_Ast();
    virtual ~Statement_Ast() = default;
    
    virtual void print(FILE *file = stdout) = 0;
    virtual TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list) = 0;
};

// Assignment Statement AST
class Assignment_Stmt : public Statement_Ast {
private:
    string lhs_name;
    Expression_Ast *rhs;
    
public:
    Assignment_Stmt(string name, Expression_Ast *expr);
    ~Assignment_Stmt();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    string get_lhs_name() { return lhs_name; }
    Expression_Ast* get_rhs() { return rhs; }
};

// Print Statement AST
class Print_Stmt : public Statement_Ast {
private:
    Expression_Ast *expr;
    
public:
    Print_Stmt(Expression_Ast *e);
    ~Print_Stmt();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    Expression_Ast* get_expr() { return expr; }
};

// Read Statement AST
class Read_Stmt : public Statement_Ast {
private:
    string var_name;
    
public:
    Read_Stmt(string name);
    ~Read_Stmt();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    string get_var_name() { return var_name; }
};

// If Statement AST
class If_Stmt : public Statement_Ast {
private:
    Expression_Ast *cond;
    Statement_Ast *then_stmt;
    Statement_Ast *else_stmt;  // NULL if no else
    
public:
    If_Stmt(Expression_Ast *c, Statement_Ast *t, Statement_Ast *e = NULL);
    ~If_Stmt();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    Expression_Ast* get_cond() { return cond; }
    Statement_Ast* get_then_stmt() { return then_stmt; }
    Statement_Ast* get_else_stmt() { return else_stmt; }
};

// While Statement AST
class While_Stmt : public Statement_Ast {
private:
    Expression_Ast *cond;
    Statement_Ast *body;
    
public:
    While_Stmt(Expression_Ast *c, Statement_Ast *b);
    ~While_Stmt();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    Expression_Ast* get_cond() { return cond; }
    Statement_Ast* get_body() { return body; }
};

// Do-While Statement AST
class Do_While_Stmt : public Statement_Ast {
private:
    Statement_Ast *body;
    Expression_Ast *cond;
    
public:
    Do_While_Stmt(Statement_Ast *b, Expression_Ast *c);
    ~Do_While_Stmt();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    Statement_Ast* get_body() { return body; }
    Expression_Ast* get_cond() { return cond; }
};

// Compound Statement AST
class Compound_Stmt : public Statement_Ast {
private:
    list<Statement_Ast*> statements;
    
public:
    Compound_Stmt();
    ~Compound_Stmt();
    
    void print(FILE *file = stdout);
    TAC_Opd* generate_tac(list<TAC_Stmt*> &tac_list);
    
    void add_statement(Statement_Ast *stmt);
    list<Statement_Ast*>& get_statements() { return statements; }
};

#endif
