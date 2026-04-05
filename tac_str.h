#ifndef TAC_STR_H
#define TAC_STR_H

#include <string>
#include <list>
#include <stdio.h>
#include <cstdint>
#include "ast_new.h"  // Include for DataType definition
using namespace std;

// Forward declarations
class TAC_Opd;
class TAC_Stmt;

// ============================================================================
// TAC OPERAND CLASSES
// ============================================================================

// Base TAC Operand Class
class TAC_Opd {
public:
    enum Opd_Type {
        CONSTANT_OPD,
        VARIABLE_OPD,
        TEMPORARY_OPD,
        TEMPORARY_FLOAT_OPD,
        LABEL_OPD
    };
    
protected:
    Opd_Type opd_type;
    DataType data_type;
    
public:
    TAC_Opd(Opd_Type type = TEMPORARY_OPD, DataType dt = VOID_DATA_TYPE);
    virtual ~TAC_Opd() = default;
    
    Opd_Type get_opd_type() { return opd_type; }
    DataType get_data_type() { return data_type; }
    
    virtual void print(FILE *file = stdout) = 0;
    virtual string to_string() = 0;
    virtual string get_name() = 0;
};

// Constant Operand
class Const_TAC_Opd : public TAC_Opd {
private:
    int int_value;
    double float_value;
    string string_value;
    bool is_float;
    bool is_string;
    
public:
    Const_TAC_Opd(int val);
    Const_TAC_Opd(double val);
    Const_TAC_Opd(string val);
    ~Const_TAC_Opd();
    
    void print(FILE *file = stdout);
    string to_string();
    string get_name();
    
    int get_int_value() { return int_value; }
    double get_float_value() { return float_value; }
    string get_string_value() { return string_value; }
    bool get_is_float() { return is_float; }
    bool get_is_string() { return is_string; }
};

// Variable Operand
class Var_TAC_Opd : public TAC_Opd {
private:
    string var_name;
    
public:
    Var_TAC_Opd(string name, DataType dt = VOID_DATA_TYPE);
    ~Var_TAC_Opd();
    
    void print(FILE *file = stdout);
    string to_string();
    string get_name();
    string get_display_string() const;
    bool is_string_label() const;
};

// Temporary Variable Operand
class Temp_TAC_Opd : public TAC_Opd {
private:
    string temp_name;
    uint32_t temp_id;
    bool is_float;
    
public:
    Temp_TAC_Opd(uint32_t id, DataType dt = VOID_DATA_TYPE);
    Temp_TAC_Opd(uint32_t id, bool float_flag);  // Legacy constructor
    Temp_TAC_Opd(uint32_t id, DataType dt, const string& prefix);  // Constructor with prefix
    ~Temp_TAC_Opd();
    
    void print(FILE *file = stdout);
    string to_string();
    string get_name();
    
    uint32_t get_temp_id() { return temp_id; }
    bool get_is_float() { return is_float; }
};

// Label Operand
class Label_TAC_Opd : public TAC_Opd {
private:
    string label_name;
    uint32_t label_id;
    
public:
    Label_TAC_Opd(uint32_t id);
    ~Label_TAC_Opd();
    
    void print(FILE *file = stdout);
    string to_string();
    string get_name();
    
    uint32_t get_label_id() { return label_id; }
};

// ============================================================================
// TAC STATEMENT CLASSES
// ============================================================================

// Base TAC Statement Class
class TAC_Stmt {
public:
    enum Stmt_Type {
        ASSIGN_STMT,      // result = opd1
        COMPUTE_STMT,     // result = opd1 op opd2
        LABEL_STMT,       // label:
        GOTO_STMT,        // goto label
        COND_GOTO_STMT,   // if (opd1) goto label
        PARAM_STMT,       // param opd1 (push argument)
        CALL_STMT,        // call func_name
        RETURN_STMT,      // return opd1
        PRINT_STMT,       // print opd1
        READ_STMT         // read var
    };
    
private:
    Stmt_Type stmt_type;
    
public:
    TAC_Stmt(Stmt_Type type);
    virtual ~TAC_Stmt() = default;
    
    Stmt_Type get_stmt_type() { return stmt_type; }
    
    virtual void print(FILE *file = stdout) = 0;
    virtual string to_string() = 0;
};

// Assignment TAC Statement: result = opd1
class Assign_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *result;
    TAC_Opd *opd1;
    
public:
    Assign_TAC_Stmt(TAC_Opd *res, TAC_Opd *src);
    ~Assign_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_result() { return result; }
    TAC_Opd* get_opd1() { return opd1; }
};

// Compute TAC Statement: result = opd1 op opd2
class Compute_TAC_Stmt : public TAC_Stmt {
public:
    enum TAC_Op {
        TAC_OP_PLUS,
        TAC_OP_MINUS,
        TAC_OP_MULT,
        TAC_OP_DIV,
        TAC_OP_GT,
        TAC_OP_GE,
        TAC_OP_LT,
        TAC_OP_LE,
        TAC_OP_EQ,
        TAC_OP_NE,
        TAC_OP_AND,
        TAC_OP_OR,
        TAC_OP_UMINUS,
        TAC_OP_NOT
    };
    
private:
    TAC_Opd *result;
    TAC_Opd *opd1;
    TAC_Op oper;
    TAC_Opd *opd2;  // NULL for unary ops
    
public:
    Compute_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1, TAC_Op o, TAC_Opd *op2 = NULL);
    ~Compute_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_result() { return result; }
    TAC_Opd* get_opd1() { return opd1; }
    TAC_Opd* get_opd2() { return opd2; }
    TAC_Op get_op() { return oper; }
    
private:
    string op_to_string(TAC_Op op);
};

// Label TAC Statement: LABEL:
class Label_TAC_Stmt : public TAC_Stmt {
private:
    Label_TAC_Opd *label;
    
public:
    Label_TAC_Stmt(Label_TAC_Opd *lbl);
    ~Label_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    Label_TAC_Opd* get_label() { return label; }
};

// Goto TAC Statement: GOTO label
class Goto_TAC_Stmt : public TAC_Stmt {
private:
    Label_TAC_Opd *label;
    
public:
    Goto_TAC_Stmt(Label_TAC_Opd *lbl);
    ~Goto_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    Label_TAC_Opd* get_label() { return label; }
};

// Conditional Goto TAC Statement: IF (opd1) GOTO label
class Cond_Goto_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *cond;
    Label_TAC_Opd *label;
    
public:
    Cond_Goto_TAC_Stmt(TAC_Opd *c, Label_TAC_Opd *lbl);
    ~Cond_Goto_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_cond() { return cond; }
    Label_TAC_Opd* get_label() { return label; }
};

// Print TAC Statement: PRINT opd1
class Print_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *opd1;
    
public:
    Print_TAC_Stmt(TAC_Opd *op);
    ~Print_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_opd() { return opd1; }
};

// Read TAC Statement: READ var
class Read_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *var;
    
public:
    Read_TAC_Stmt(TAC_Opd *v);
    ~Read_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_var() { return var; }
};

// Parameter TAC Statement: PARAM opd1 (push argument onto stack)
class Param_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *opd1;
    
public:
    Param_TAC_Stmt(TAC_Opd *op);
    ~Param_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_opd() { return opd1; }
};

// Call TAC Statement: CALL func_name
class Call_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *func_name;  // Variable containing function name
    
public:
    Call_TAC_Stmt(TAC_Opd *fn);
    ~Call_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_func() { return func_name; }
};

// Return TAC Statement: RETURN opd1 (or RETURN with no value)
class Return_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *return_val;  // NULL if returning void
    
public:
    Return_TAC_Stmt(TAC_Opd *ret_val = NULL);
    ~Return_TAC_Stmt();
    
    void print(FILE *file = stdout);
    string to_string();
    
    TAC_Opd* get_return_val() { return return_val; }
};

#endif
