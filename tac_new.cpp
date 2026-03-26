#include <iostream>
#include <cstdio>
#include <cstring>
#include "tac_new.h"

using namespace std;

FILE *tac_file = NULL;  // Will be set by main program
int show_tac = 0;       // Will be set by command line options

// Global TACGenerator instance
TACGenerator tac_gen;

/**
 * ============================================================================
 * TAC_Opd CLASS IMPLEMENTATIONS
 * ============================================================================
 */

/* TAC_Opd: Base class */
TAC_Opd::TAC_Opd() : opd_data_type(INT_DATA_TYPE), type(VAR_OPD) {}

TAC_Opd::~TAC_Opd() {}

DataType TAC_Opd::get_data_type() {
    return opd_data_type;
}

void TAC_Opd::set_data_type(DataType d) {
    opd_data_type = d;
}

/* Const_TAC_Opd */
Const_TAC_Opd::Const_TAC_Opd(string val, DataType dtype) : value(val) {
    type = CONST_OPD;
    opd_data_type = dtype;
}

Const_TAC_Opd::~Const_TAC_Opd() {}

string Const_TAC_Opd::get_value() {
    return value;
}

string Const_TAC_Opd::get_name() {
    return value;
}

void Const_TAC_Opd::print() {
    printf("%s", value.c_str());
}

/* Var_TAC_Opd */
Var_TAC_Opd::Var_TAC_Opd(string var_name, DataType dtype) : name(var_name) {
    type = VAR_OPD;
    opd_data_type = dtype;
}

Var_TAC_Opd::~Var_TAC_Opd() {}

string Var_TAC_Opd::get_name() {
    return name;
}

void Var_TAC_Opd::print() {
    printf("%s", name.c_str());
}

/* Temp_TAC_Opd */
Temp_TAC_Opd::Temp_TAC_Opd(int temp_id, DataType dtype, string pref) 
    : id(temp_id), prefix(pref) {
    type = TEMP_OPD;
    opd_data_type = dtype;
}

Temp_TAC_Opd::~Temp_TAC_Opd() {}

int Temp_TAC_Opd::get_id() {
    return id;
}

string Temp_TAC_Opd::get_name() {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%d", prefix.c_str(), id);
    return string(buf);
}

void Temp_TAC_Opd::print() {
    printf("%s%d", prefix.c_str(), id);
}

/* Label_TAC_Opd */
Label_TAC_Opd::Label_TAC_Opd(int label_id) : id(label_id) {
    type = LABEL_OPD;
    opd_data_type = INT_DATA_TYPE;  // Labels don't have real data type
}

Label_TAC_Opd::~Label_TAC_Opd() {}

int Label_TAC_Opd::get_id() {
    return id;
}

string Label_TAC_Opd::get_name() {
    char buf[32];
    snprintf(buf, sizeof(buf), "Label%d", id);
    return string(buf);
}

void Label_TAC_Opd::print() {
    printf("Label%d", id);
}

/**
 * ============================================================================
 * TAC_Stmt CLASS IMPLEMENTATIONS
 * ============================================================================
 */

/* TAC_Stmt: Base class */
TAC_Stmt::TAC_Stmt() : type(ASSIGN_STMT) {}

TAC_Stmt::~TAC_Stmt() {}

/* Assign_TAC_Stmt: Binary operation constructor */
Assign_TAC_Stmt::Assign_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1, string op, TAC_Opd *op2)
    : result(res), opd1(op1), opd2(op2), operation(op) {
    type = ASSIGN_STMT;
}

/* Assign_TAC_Stmt: Unary operation constructor */
Assign_TAC_Stmt::Assign_TAC_Stmt(TAC_Opd *res, string op, TAC_Opd *op1)
    : result(res), opd1(op1), opd2(NULL), operation(op) {
    type = ASSIGN_STMT;
}

Assign_TAC_Stmt::~Assign_TAC_Stmt() {}

TAC_Opd* Assign_TAC_Stmt::get_result() {
    return result;
}

TAC_Opd* Assign_TAC_Stmt::get_opd1() {
    return opd1;
}

TAC_Opd* Assign_TAC_Stmt::get_opd2() {
    return opd2;
}

string Assign_TAC_Stmt::get_operation() {
    return operation;
}

void Assign_TAC_Stmt::print() {
    if (!result) return;
    
    result->print();
    printf(" = ");
    
    if (opd2) {  // Binary operation
        opd1->print();
        printf(" %s ", operation.c_str());
        opd2->print();
    } else {    // Unary operation
        printf("%s ", operation.c_str());
        opd1->print();
    }
    printf("\n");
}

/* Move_TAC_Stmt */
Move_TAC_Stmt::Move_TAC_Stmt(TAC_Opd *res, TAC_Opd *op)
    : result(res), opd(op) {
    type = MOVE_STMT;
}

Move_TAC_Stmt::~Move_TAC_Stmt() {}

TAC_Opd* Move_TAC_Stmt::get_result() {
    return result;
}

TAC_Opd* Move_TAC_Stmt::get_opd() {
    return opd;
}

void Move_TAC_Stmt::print() {
    if (!result) return;
    
    result->print();
    printf(" = ");
    opd->print();
    printf("\n");
}

/* Label_TAC_Stmt */
Label_TAC_Stmt::Label_TAC_Stmt(Label_TAC_Opd *lbl) : label(lbl) {
    type = LABEL_STMT;
}

Label_TAC_Stmt::~Label_TAC_Stmt() {}

Label_TAC_Opd* Label_TAC_Stmt::get_label() {
    return label;
}

void Label_TAC_Stmt::print() {
    if (!label) return;
    
    label->print();
    printf(":\n");
}

/* Goto_TAC_Stmt */
Goto_TAC_Stmt::Goto_TAC_Stmt(Label_TAC_Opd *lbl) : label(lbl) {
    type = GOTO_STMT;
}

Goto_TAC_Stmt::~Goto_TAC_Stmt() {}

Label_TAC_Opd* Goto_TAC_Stmt::get_label() {
    return label;
}

void Goto_TAC_Stmt::print() {
    if (!label) return;
    
    printf("GOTO ");
    label->print();
    printf("\n");
}

/* If_Goto_TAC_Stmt */
If_Goto_TAC_Stmt::If_Goto_TAC_Stmt(TAC_Opd *cond, Label_TAC_Opd *lbl, bool neg)
    : condition(cond), label(lbl), negate(neg) {
    type = IF_GOTO_STMT;
}

If_Goto_TAC_Stmt::~If_Goto_TAC_Stmt() {}

TAC_Opd* If_Goto_TAC_Stmt::get_condition() {
    return condition;
}

Label_TAC_Opd* If_Goto_TAC_Stmt::get_label() {
    return label;
}

bool If_Goto_TAC_Stmt::is_negated() {
    return negate;
}

void If_Goto_TAC_Stmt::print() {
    if (!condition || !label) return;
    
    printf("IF (");
    if (negate) printf("!");
    condition->print();
    printf(") GOTO ");
    label->print();
    printf("\n");
}

/* FCall_TAC_Stmt */
FCall_TAC_Stmt::FCall_TAC_Stmt(string fname) : func_name(fname) {
    type = FCALL_STMT;
}

FCall_TAC_Stmt::~FCall_TAC_Stmt() {
    for (auto arg : arguments) {
        delete arg;
    }
    arguments.clear();
}

string FCall_TAC_Stmt::get_func_name() {
    return func_name;
}

list<TAC_Opd*>& FCall_TAC_Stmt::get_arguments() {
    return arguments;
}

void FCall_TAC_Stmt::add_argument(TAC_Opd *arg) {
    if (arg) arguments.push_back(arg);
}

void FCall_TAC_Stmt::print() {
    printf("call %s(", func_name.c_str());
    
    bool first = true;
    for (auto arg : arguments) {
        if (!first) printf(", ");
        arg->print();
        first = false;
    }
    
    printf(")\n");
}

/* FCall_Return_TAC_Stmt */
FCall_Return_TAC_Stmt::FCall_Return_TAC_Stmt(TAC_Opd *res, string fname)
    : result(res), func_name(fname) {
    type = FCALL_STMT;
}

FCall_Return_TAC_Stmt::~FCall_Return_TAC_Stmt() {
    for (auto arg : arguments) {
        delete arg;
    }
    arguments.clear();
}

TAC_Opd* FCall_Return_TAC_Stmt::get_result() {
    return result;
}

string FCall_Return_TAC_Stmt::get_func_name() {
    return func_name;
}

list<TAC_Opd*>& FCall_Return_TAC_Stmt::get_arguments() {
    return arguments;
}

void FCall_Return_TAC_Stmt::add_argument(TAC_Opd *arg) {
    if (arg) arguments.push_back(arg);
}

void FCall_Return_TAC_Stmt::print() {
    if (!result) return;
    
    result->print();
    printf(" = call %s(", func_name.c_str());
    
    bool first = true;
    for (auto arg : arguments) {
        if (!first) printf(", ");
        arg->print();
        first = false;
    }
    
    printf(")\n");
}

/* Return_TAC_Stmt */
Return_TAC_Stmt::Return_TAC_Stmt(TAC_Opd *val) : value(val) {
    type = RETURN_STMT;
}

Return_TAC_Stmt::~Return_TAC_Stmt() {}

TAC_Opd* Return_TAC_Stmt::get_value() {
    return value;
}

void Return_TAC_Stmt::print() {
    printf("return");
    
    if (value) {
        printf(" ");
        value->print();
    }
    
    printf("\n");
}

/* Param_TAC_Stmt */
Param_TAC_Stmt::Param_TAC_Stmt(TAC_Opd *val, int count)
    : value(val), param_count(count) {
    type = PARAM_STMT;
}

Param_TAC_Stmt::~Param_TAC_Stmt() {}

TAC_Opd* Param_TAC_Stmt::get_value() {
    return value;
}

int Param_TAC_Stmt::get_param_count() {
    return param_count;
}

void Param_TAC_Stmt::print() {
    printf("param ");
    if (value) value->print();
    printf("\n");
}

/* Print_TAC_Stmt */
Print_TAC_Stmt::Print_TAC_Stmt(TAC_Opd *val) : value(val) {
    type = PRINT_STMT;
}

Print_TAC_Stmt::~Print_TAC_Stmt() {}

TAC_Opd* Print_TAC_Stmt::get_value() {
    return value;
}

void Print_TAC_Stmt::print() {
    printf("PRINT ");
    if (value) value->print();
    printf("\n");
}

/* Read_TAC_Stmt */
Read_TAC_Stmt::Read_TAC_Stmt(Var_TAC_Opd *var) : variable(var) {
    type = READ_STMT;
}

Read_TAC_Stmt::~Read_TAC_Stmt() {}

Var_TAC_Opd* Read_TAC_Stmt::get_variable() {
    return variable;
}

void Read_TAC_Stmt::print() {
    printf("READ ");
    if (variable) variable->print();
    printf("\n");
}

/**
 * ============================================================================
 * TACGenerator CLASS IMPLEMENTATIONS
 * ============================================================================
 */

TACGenerator::TACGenerator() 
    : temp_counter(0), stemp_counter(0), label_counter(0) {
}

TACGenerator::~TACGenerator() {}

Temp_TAC_Opd* TACGenerator::new_temp(DataType dtype) {
    Temp_TAC_Opd *temp = new Temp_TAC_Opd(temp_counter++, dtype, "temp");
    return temp;
}

Temp_TAC_Opd* TACGenerator::new_stemp(DataType dtype) {
    Temp_TAC_Opd *stemp = new Temp_TAC_Opd(stemp_counter++, dtype, "stemp");
    return stemp;
}

Label_TAC_Opd* TACGenerator::new_label() {
    Label_TAC_Opd *label = new Label_TAC_Opd(label_counter++);
    return label;
}

Assign_TAC_Stmt* TACGenerator::new_assign_stmt(TAC_Opd *res, TAC_Opd *op1, 
                                               string op, TAC_Opd *op2) {
    return new Assign_TAC_Stmt(res, op1, op, op2);
}

Assign_TAC_Stmt* TACGenerator::new_assign_stmt(TAC_Opd *res, string op, TAC_Opd *op1) {
    return new Assign_TAC_Stmt(res, op, op1);
}

Move_TAC_Stmt* TACGenerator::new_move_stmt(TAC_Opd *res, TAC_Opd *op) {
    return new Move_TAC_Stmt(res, op);
}

Label_TAC_Stmt* TACGenerator::new_label_stmt(Label_TAC_Opd *lbl) {
    return new Label_TAC_Stmt(lbl);
}

Goto_TAC_Stmt* TACGenerator::new_goto_stmt(Label_TAC_Opd *lbl) {
    return new Goto_TAC_Stmt(lbl);
}

If_Goto_TAC_Stmt* TACGenerator::new_if_goto_stmt(TAC_Opd *cond, Label_TAC_Opd *lbl, bool neg) {
    return new If_Goto_TAC_Stmt(cond, lbl, neg);
}

Return_TAC_Stmt* TACGenerator::new_return_stmt(TAC_Opd *val) {
    return new Return_TAC_Stmt(val);
}

Print_TAC_Stmt* TACGenerator::new_print_stmt(TAC_Opd *val) {
    return new Print_TAC_Stmt(val);
}

Read_TAC_Stmt* TACGenerator::new_read_stmt(Var_TAC_Opd *var) {
    return new Read_TAC_Stmt(var);
}

void TACGenerator::reset() {
    temp_counter = 0;
    stemp_counter = 0;
    label_counter = 0;
}

/**
 * ============================================================================
 * UTILITY FUNCTIONS FOR TAC PRINTING
 * ============================================================================
 */

/**
 * print_tac_stmts: Print a list of TAC statements
 * @param statements - List of TAC_Stmt pointers to print
 */
void print_tac_stmts(list<TAC_Stmt*>& statements) {
    if (!tac_file) return;
    
    for (auto stmt : statements) {
        if (stmt) stmt->print();
    }
}

/**
 * print_tac_opd: Print a single TAC operand
 * @param opd - TAC_Opd pointer to print
 */
void print_tac_opd(TAC_Opd *opd) {
    if (!tac_file || !opd) return;
    opd->print();
}
