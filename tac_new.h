#ifndef TAC_NEW_H
#define TAC_NEW_H

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <list>
#include <vector>
#include "ast_new.h"

using std::string;
using std::list;

extern FILE *tac_file;
extern int show_tac;

/* ============================================================================
   TAC OPERAND FORWARD DECLARATIONS
   ============================================================================ */
class TAC_Opd;
class Const_TAC_Opd;
class Var_TAC_Opd;
class Temp_TAC_Opd;
class Label_TAC_Opd;

/* ============================================================================
   TAC OPERAND BASE CLASS AND DERIVED CLASSES
   ============================================================================ */

/**
 * TAC_Opd: Base class for all TAC operands
 * Represents operands in Three Address Code (temporary variables, variables, 
 * constants, and labels)
 */
class TAC_Opd {
public:
    enum operand_type {
        CONST_OPD,      // Const_TAC_Opd
        VAR_OPD,        // Var_TAC_Opd
        TEMP_OPD,       // Temp_TAC_Opd
        LABEL_OPD       // Label_TAC_Opd
    };
    
    DataType opd_data_type;
    operand_type type;
    
    TAC_Opd();
    virtual ~TAC_Opd();
    
    virtual void print() = 0;
    virtual string get_name() = 0;
    
    DataType get_data_type();
    void set_data_type(DataType d);
};

/**
 * Const_TAC_Opd: Represents constant operands
 * Examples: 5, 3.14, "hello", true
 */
class Const_TAC_Opd : public TAC_Opd {
private:
    string value;
    
public:
    Const_TAC_Opd(string val, DataType dtype);
    ~Const_TAC_Opd();
    
    string get_value();
    void print();
    string get_name();
};

/**
 * Var_TAC_Opd: Represents variable operands
 * Examples: x, y, arr[i]
 */
class Var_TAC_Opd : public TAC_Opd {
private:
    string name;
    
public:
    Var_TAC_Opd(string var_name, DataType dtype);
    ~Var_TAC_Opd();
    
    string get_name();
    void print();
};

/**
 * Temp_TAC_Opd: Represents temporary variable operands
 * Examples: temp0, temp1, temp2, etc.
 * Also includes string temporaries: stemp0, stemp1, etc.
 */
class Temp_TAC_Opd : public TAC_Opd {
private:
    int id;
    string prefix;  // "temp" or "stemp"
    
public:
    Temp_TAC_Opd(int temp_id, DataType dtype, string prefix = "temp");
    ~Temp_TAC_Opd();
    
    int get_id();
    string get_name();
    void print();
};

/**
 * Label_TAC_Opd: Represents label operands used for jumps
 * Examples: Label0, Label1, etc.
 */
class Label_TAC_Opd : public TAC_Opd {
private:
    int id;
    
public:
    Label_TAC_Opd(int label_id);
    ~Label_TAC_Opd();
    
    int get_id();
    string get_name();
    void print();
};

/* ============================================================================
   TAC STATEMENT BASE CLASS AND DERIVED CLASSES
   ============================================================================ */

/**
 * TAC_Stmt: Base class for all TAC statements
 * Represents instructions in Three Address Code
 */
class TAC_Stmt {
public:
    enum stmt_type {
        ASSIGN_STMT,        // Assign_TAC_Stmt
        MOVE_STMT,          // Move_TAC_Stmt (assignment without operation)
        LABEL_STMT,         // Label_TAC_Stmt
        GOTO_STMT,          // Goto_TAC_Stmt
        IF_GOTO_STMT,       // If_Goto_TAC_Stmt
        FCALL_STMT,         // FCall_TAC_Stmt
        RETURN_STMT,        // Return_TAC_Stmt
        PARAM_STMT,         // Param_TAC_Stmt
        PRINT_STMT,         // Print_TAC_Stmt
        READ_STMT           // Read_TAC_Stmt
    };
    
    stmt_type type;
    
    TAC_Stmt();
    virtual ~TAC_Stmt();
    
    virtual void print() = 0;
};

/**
 * Assign_TAC_Stmt: Binary or unary operation assignment
 * Examples: temp0 = x + y, temp1 = -z, temp2 = a < b
 */
class Assign_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *result;
    TAC_Opd *opd1;
    TAC_Opd *opd2;
    string operation;  // "+", "-", "*", "/", "<", ">", "==", etc.
    
public:
    // Binary operation: result = opd1 op opd2
    Assign_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1, string op, TAC_Opd *op2);
    
    // Unary operation: result = op opd1
    Assign_TAC_Stmt(TAC_Opd *res, string op, TAC_Opd *op1);
    
    ~Assign_TAC_Stmt();
    
    TAC_Opd* get_result();
    TAC_Opd* get_opd1();
    TAC_Opd* get_opd2();
    string get_operation();
    
    void print();
};

/**
 * Move_TAC_Stmt: Simple assignment (no operation)
 * Examples: x = temp0, y = 5, temp1 = z
 */
class Move_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *result;
    TAC_Opd *opd;
    
public:
    Move_TAC_Stmt(TAC_Opd *res, TAC_Opd *op);
    ~Move_TAC_Stmt();
    
    TAC_Opd* get_result();
    TAC_Opd* get_opd();
    
    void print();
};

/**
 * Label_TAC_Stmt: Label definition for jumps
 * Examples: Label0:, Label1:
 */
class Label_TAC_Stmt : public TAC_Stmt {
private:
    Label_TAC_Opd *label;
    
public:
    Label_TAC_Stmt(Label_TAC_Opd *lbl);
    ~Label_TAC_Stmt();
    
    Label_TAC_Opd* get_label();
    
    void print();
};

/**
 * Goto_TAC_Stmt: Unconditional jump
 * Examples: goto Label0
 */
class Goto_TAC_Stmt : public TAC_Stmt {
private:
    Label_TAC_Opd *label;
    
public:
    Goto_TAC_Stmt(Label_TAC_Opd *lbl);
    ~Goto_TAC_Stmt();
    
    Label_TAC_Opd* get_label();
    
    void print();
};

/**
 * If_Goto_TAC_Stmt: Conditional jump
 * Examples: if(temp0) goto Label1, if(!temp1) goto Label2
 */
class If_Goto_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *condition;
    Label_TAC_Opd *label;
    bool negate;  // true if we need to negate the condition (if not condition)
    
public:
    If_Goto_TAC_Stmt(TAC_Opd *cond, Label_TAC_Opd *lbl, bool neg = false);
    ~If_Goto_TAC_Stmt();
    
    TAC_Opd* get_condition();
    Label_TAC_Opd* get_label();
    bool is_negated();
    
    void print();
};

/**
 * FCall_TAC_Stmt: Function call statement (when result is not used)
 * Examples: foo(x, y), bar()
 */
class FCall_TAC_Stmt : public TAC_Stmt {
private:
    string func_name;
    list<TAC_Opd*> arguments;
    
public:
    FCall_TAC_Stmt(string fname);
    ~FCall_TAC_Stmt();
    
    string get_func_name();
    list<TAC_Opd*>& get_arguments();
    void add_argument(TAC_Opd *arg);
    
    void print();
    void print(FILE *file);
};

/**
 * FCall_Return_TAC_Stmt: Function call with return value assignment
 * Examples: temp0 = foo(x, y), t = bar()
 */
class FCall_Return_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *result;
    string func_name;
    list<TAC_Opd*> arguments;
    
public:
    FCall_Return_TAC_Stmt(TAC_Opd *res, string fname);
    ~FCall_Return_TAC_Stmt();
    
    TAC_Opd* get_result();
    string get_func_name();
    list<TAC_Opd*>& get_arguments();
    void add_argument(TAC_Opd *arg);
    
    void print();
};

/**
 * Return_TAC_Stmt: Function return statement
 * Examples: return x, return temp0
 */
class Return_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *value;  // NULL if void return
    
public:
    Return_TAC_Stmt(TAC_Opd *val = NULL);
    ~Return_TAC_Stmt();
    
    TAC_Opd* get_value();
    
    void print();
};

/**
 * Param_TAC_Stmt: Function parameter passing
 * Examples: param x, param temp0
 */
class Param_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *value;
    int param_count;  // Order of parameter (1st, 2nd, 3rd, etc.)
    
public:
    Param_TAC_Stmt(TAC_Opd *val, int count = 0);
    ~Param_TAC_Stmt();
    
    TAC_Opd* get_value();
    int get_param_count();
    
    void print();
};

/**
 * Print_TAC_Stmt: Print statement (output)
 * Examples: print x, print "hello", print temp0
 */
class Print_TAC_Stmt : public TAC_Stmt {
private:
    TAC_Opd *value;
    
public:
    Print_TAC_Stmt(TAC_Opd *val);
    ~Print_TAC_Stmt();
    
    TAC_Opd* get_value();
    
    void print();
};

/**
 * Read_TAC_Stmt: Read statement (input)
 * Examples: read x, read y
 */
class Read_TAC_Stmt : public TAC_Stmt {
private:
    Var_TAC_Opd *variable;
    
public:
    Read_TAC_Stmt(Var_TAC_Opd *var);
    ~Read_TAC_Stmt();
    
    Var_TAC_Opd* get_variable();
    
    void print();
};

/* ============================================================================
   TAC GENERATOR CLASS (enhanced)
   ============================================================================ */

/**
 * TACGenerator: Manages TAC generation
 * - Generates unique identifiers for temporaries, string temporaries, and labels
 * - Maintains counters for each type
 */
class TACGenerator {
private:
    int temp_counter;
    int stemp_counter;
    int label_counter;
    
public:
    TACGenerator();
    ~TACGenerator();
    
    // Create new operands
    Temp_TAC_Opd* new_temp(DataType dtype);
    Temp_TAC_Opd* new_stemp(DataType dtype);
    Label_TAC_Opd* new_label();
    
    // Create statements
    Assign_TAC_Stmt* new_assign_stmt(TAC_Opd *res, TAC_Opd *op1, string op, TAC_Opd *op2);
    Assign_TAC_Stmt* new_assign_stmt(TAC_Opd *res, string op, TAC_Opd *op1);  // Unary
    Move_TAC_Stmt* new_move_stmt(TAC_Opd *res, TAC_Opd *op);
    Label_TAC_Stmt* new_label_stmt(Label_TAC_Opd *lbl);
    Goto_TAC_Stmt* new_goto_stmt(Label_TAC_Opd *lbl);
    If_Goto_TAC_Stmt* new_if_goto_stmt(TAC_Opd *cond, Label_TAC_Opd *lbl, bool neg = false);
    Return_TAC_Stmt* new_return_stmt(TAC_Opd *val = NULL);
    Print_TAC_Stmt* new_print_stmt(TAC_Opd *val);
    Read_TAC_Stmt* new_read_stmt(Var_TAC_Opd *var);
    
    void reset();
};

extern TACGenerator tac_gen;

#endif // TAC_NEW_H
