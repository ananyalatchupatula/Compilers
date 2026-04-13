#include "tac_str.h"
#include "tac_generator.h"
#include <iostream>
#include <sstream>
#include <cstdio>
using namespace std;

// ============================================================================
// TAC_OPD BASE CLASS
// ============================================================================

TAC_Opd::TAC_Opd(Opd_Type type, DataType dt) : opd_type(type), data_type(dt) {}

// ============================================================================
// CONST_TAC_OPD CLASS
// ============================================================================

Const_TAC_Opd::Const_TAC_Opd(int val) 
    : TAC_Opd(CONSTANT_OPD, INT_DATA_TYPE),
      int_value(val),
      is_float(false),
      is_string(false) {
    float_value = 0;
}

Const_TAC_Opd::Const_TAC_Opd(double val) 
    : TAC_Opd(CONSTANT_OPD, FLOAT_DATA_TYPE),
      float_value(val),
      is_float(true),
      is_string(false) {
    int_value = 0;
}

Const_TAC_Opd::Const_TAC_Opd(string val) 
    : TAC_Opd(CONSTANT_OPD, STRING_DATA_TYPE),
      string_value(val),
      is_float(false),
      is_string(true) {
    int_value = 0;
    float_value = 0;
}

Const_TAC_Opd::~Const_TAC_Opd() {}

void Const_TAC_Opd::print(FILE *file) {
    fprintf(file, "%s", to_string().c_str());
}

string Const_TAC_Opd::to_string() {
    if (is_string) {
        return string_value;  // Already has quotes from lexer
    } else if (is_float) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", float_value);
        return string(buf);
    } else {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", int_value);
        return string(buf);
    }
}

string Const_TAC_Opd::get_name() {
    return to_string();
}

// ============================================================================
// VAR_TAC_OPD CLASS
// ============================================================================

Var_TAC_Opd::Var_TAC_Opd(string name, DataType dt) 
    : TAC_Opd(VARIABLE_OPD, dt), var_name(name) {}

Var_TAC_Opd::~Var_TAC_Opd() {}

void Var_TAC_Opd::print(FILE *file) {
    fprintf(file, "%s", var_name.c_str());
}

string Var_TAC_Opd::to_string() {
    return var_name;
}

string Var_TAC_Opd::get_name() {
    return var_name;
}

string Var_TAC_Opd::get_display_string() const {
    // If var_name starts with _str_, lookup the literal string value
    if (var_name.length() > 5 && var_name.substr(0, 5) == "_str_") {
        string literal = TAC_Generator::get_instance()->get_string_literal_from_label(var_name);
        if (literal != var_name) {
            // Found the literal, return it as-is (already has quotes from parser)
            return literal;
        }
    }
    return var_name;
}

bool Var_TAC_Opd::is_string_label() const {
    // Check if this variable is a string label (starts with _str_)
    return var_name.length() > 5 && var_name.substr(0, 5) == "_str_";
}

// ============================================================================
// TEMP_TAC_OPD CLASS
// ============================================================================

Temp_TAC_Opd::Temp_TAC_Opd(uint32_t id, DataType dt) 
    : TAC_Opd(TEMPORARY_OPD, dt), temp_id(id), is_float(dt == FLOAT_DATA_TYPE) {
    char buf[32];
    snprintf(buf, sizeof(buf), "temp%u", id);
    temp_name = string(buf);
}

Temp_TAC_Opd::Temp_TAC_Opd(uint32_t id, bool float_flag) 
    : TAC_Opd(TEMPORARY_OPD), temp_id(id), is_float(float_flag) {
    char buf[32];
    if (is_float) {
        snprintf(buf, sizeof(buf), "ftemp%u", id);
    } else {
        snprintf(buf, sizeof(buf), "temp%u", id);
    }
    temp_name = string(buf);
}

Temp_TAC_Opd::Temp_TAC_Opd(uint32_t id, DataType dt, const string& prefix) 
    : TAC_Opd(TEMPORARY_OPD, dt), temp_id(id), is_float(dt == FLOAT_DATA_TYPE) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%s%u", prefix.c_str(), id);
    temp_name = string(buf);
}

Temp_TAC_Opd::~Temp_TAC_Opd() {}

void Temp_TAC_Opd::print(FILE *file) {
    fprintf(file, "%s", temp_name.c_str());
}

string Temp_TAC_Opd::to_string() {
    return temp_name;
}

string Temp_TAC_Opd::get_name() {
    return temp_name;
}

// ============================================================================
// LABEL_TAC_OPD CLASS
// ============================================================================

Label_TAC_Opd::Label_TAC_Opd(uint32_t id) 
    : TAC_Opd(LABEL_OPD), label_id(id) {
    char buf[32];
    snprintf(buf, sizeof(buf), "Label%u", id);
    label_name = string(buf);
}

Label_TAC_Opd::~Label_TAC_Opd() {}

void Label_TAC_Opd::print(FILE *file) {
    fprintf(file, "%s", label_name.c_str());
}

string Label_TAC_Opd::to_string() {
    return label_name;
}

string Label_TAC_Opd::get_name() {
    return label_name;
}

// ============================================================================
// TAC_STMT BASE CLASS
// ============================================================================

TAC_Stmt::TAC_Stmt(Stmt_Type type) : stmt_type(type) {}

// ============================================================================
// ASSIGN_TAC_STMT CLASS
// ============================================================================

Assign_TAC_Stmt::Assign_TAC_Stmt(TAC_Opd *res, TAC_Opd *src) 
    : TAC_Stmt(ASSIGN_STMT), result(res), opd1(src) {}

Assign_TAC_Stmt::~Assign_TAC_Stmt() {
    delete result;
    delete opd1;
}

void Assign_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string Assign_TAC_Stmt::to_string() {
    stringstream ss;
    string opd1_str = opd1->to_string();
    
    // Check if opd1 is a string label - if so, use display string
    if (auto var = dynamic_cast<Var_TAC_Opd*>(opd1)) {
        if (var->is_string_label()) {
            opd1_str = var->get_display_string();
        }
    }
    
    ss << result->to_string() << " = " << opd1_str;
    return ss.str();
}

// ============================================================================
// COMPUTE_TAC_STMT CLASS
// ============================================================================

Compute_TAC_Stmt::Compute_TAC_Stmt(TAC_Opd *res, TAC_Opd *op1, TAC_Op o, TAC_Opd *op2) 
    : TAC_Stmt(COMPUTE_STMT), result(res), opd1(op1), oper(o), opd2(op2) {}

Compute_TAC_Stmt::~Compute_TAC_Stmt() {
    delete result;
    delete opd1;
    if (opd2) delete opd2;
}

void Compute_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string Compute_TAC_Stmt::to_string() {
    stringstream ss;
    ss << result->to_string() << " = ";
    
    // For unary operators
    if (opd2 == NULL) {
        ss << op_to_string(oper) << " " << opd1->to_string();
    } else {
        // For binary operators
        ss << opd1->to_string() << " " << op_to_string(oper) << " " << opd2->to_string();
    }
    
    return ss.str();
}

string Compute_TAC_Stmt::op_to_string(TAC_Op op) {
    switch (op) {
        case TAC_OP_PLUS: return "+";
        case TAC_OP_MINUS: return "-";
        case TAC_OP_MULT: return "*";
        case TAC_OP_DIV: return "/";
        case TAC_OP_GT: return ">";
        case TAC_OP_GE: return ">=";
        case TAC_OP_LT: return "<";
        case TAC_OP_LE: return "<=";
        case TAC_OP_EQ: return "==";
        case TAC_OP_NE: return "!=";
        case TAC_OP_AND: return "&&";
        case TAC_OP_OR: return "||";
        case TAC_OP_UMINUS: return "-";
        case TAC_OP_NOT: return "!";
        default: return "?";
    }
}

// ============================================================================
// LABEL_TAC_STMT CLASS
// ============================================================================

Label_TAC_Stmt::Label_TAC_Stmt(Label_TAC_Opd *lbl) 
    : TAC_Stmt(LABEL_STMT), label(lbl) {}

Label_TAC_Stmt::~Label_TAC_Stmt() {
    delete label;
}

void Label_TAC_Stmt::print(FILE *file) {
    fprintf(file, "%s\n", to_string().c_str());
}

string Label_TAC_Stmt::to_string() {
    return label->to_string() + ":";
}

// ============================================================================
// GOTO_TAC_STMT CLASS
// ============================================================================

Goto_TAC_Stmt::Goto_TAC_Stmt(Label_TAC_Opd *lbl) 
    : TAC_Stmt(GOTO_STMT), label(lbl) {}

Goto_TAC_Stmt::~Goto_TAC_Stmt() {
    delete label;
}

void Goto_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string Goto_TAC_Stmt::to_string() {
    return "goto " + label->to_string();
}

// ============================================================================
// COND_GOTO_TAC_STMT CLASS
// ============================================================================

Cond_Goto_TAC_Stmt::Cond_Goto_TAC_Stmt(TAC_Opd *c, Label_TAC_Opd *lbl) 
    : TAC_Stmt(COND_GOTO_STMT), cond(c), label(lbl) {}

Cond_Goto_TAC_Stmt::~Cond_Goto_TAC_Stmt() {
    delete cond;
    delete label;
}

void Cond_Goto_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string Cond_Goto_TAC_Stmt::to_string() {
    stringstream ss;
    ss << "if(" << cond->to_string() << ") goto " << label->to_string();
    return ss.str();
}

// ============================================================================
// PRINT_TAC_STMT CLASS
// ============================================================================

Print_TAC_Stmt::Print_TAC_Stmt(TAC_Opd *op) 
    : TAC_Stmt(PRINT_STMT), opd1(op) {}

Print_TAC_Stmt::~Print_TAC_Stmt() {
    delete opd1;
}

void Print_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string Print_TAC_Stmt::to_string() {
    // Check if operand is a string label - if so, show the literal
    if (auto var = dynamic_cast<Var_TAC_Opd*>(opd1)) {
        if (var->is_string_label()) {
            return "write " + var->get_display_string();
        }
    }
    return "write " + opd1->to_string();
}

// ============================================================================
// READ_TAC_STMT CLASS
// ============================================================================

Read_TAC_Stmt::Read_TAC_Stmt(TAC_Opd *v) 
    : TAC_Stmt(READ_STMT), var(v) {}

Read_TAC_Stmt::~Read_TAC_Stmt() {
    delete var;
}

void Read_TAC_Stmt::print(FILE *file) {
    fprintf(file, "%s\n", to_string().c_str());
}

string Read_TAC_Stmt::to_string() {
    return "read " + var->to_string();
}
// ============================================================================
// PARAM_TAC_STMT CLASS (Push argument onto stack)
// ============================================================================

Param_TAC_Stmt::Param_TAC_Stmt(TAC_Opd *op) 
    : TAC_Stmt(PARAM_STMT), opd1(op) {}

Param_TAC_Stmt::~Param_TAC_Stmt() {
    delete opd1;
}

void Param_TAC_Stmt::print(FILE *file) {
    fprintf(file, "%s\n", to_string().c_str());
}

string Param_TAC_Stmt::to_string() {
    return "param " + opd1->to_string();
}

// ============================================================================
// CALL_TAC_STMT CLASS
// ============================================================================

Call_TAC_Stmt::Call_TAC_Stmt(TAC_Opd *fn) 
    : TAC_Stmt(CALL_STMT), func_name(fn) {}

Call_TAC_Stmt::~Call_TAC_Stmt() {
    delete func_name;
}

void Call_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string Call_TAC_Stmt::to_string() {
    // func_name already includes the trailing underscore from parser
    string result = func_name->to_string() + "(";
    
    // Add arguments
    bool first = true;
    for(auto arg : arguments) {
        if(!first) result += ", ";
        result += arg->to_string();
        first = false;
    }
    
    result += ")";
    return result;
}

void Call_TAC_Stmt::add_argument(TAC_Opd *arg) {
    if(arg) arguments.push_back(arg);
}

// ============================================================================
// ASSIGNCALL_TAC_STMT CLASS
// ============================================================================

AssignCall_TAC_Stmt::AssignCall_TAC_Stmt(TAC_Opd *result, TAC_Opd *fn)
    : TAC_Stmt(ASSIGN_STMT), lhs(result), func_name(fn) {}

AssignCall_TAC_Stmt::~AssignCall_TAC_Stmt() {
    if(lhs) delete lhs;
    if(func_name) delete func_name;
    for(auto arg : arguments) {
        if(arg) delete arg;
    }
    arguments.clear();
}

void AssignCall_TAC_Stmt::add_argument(TAC_Opd *arg) {
    if(arg) arguments.push_back(arg);
}

void AssignCall_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string AssignCall_TAC_Stmt::to_string() {
    string result = lhs->to_string() + " = " + func_name->to_string() + "(";
    
    // Add arguments
    bool first = true;
    for(auto arg : arguments) {
        if(!first) result += ", ";
        result += arg->to_string();
        first = false;
    }
    
    result += ")";
    return result;
}

// ============================================================================
// RETURN_TAC_STMT CLASS
// ============================================================================

Return_TAC_Stmt::Return_TAC_Stmt(TAC_Opd *ret_val) 
    : TAC_Stmt(RETURN_STMT), return_val(ret_val) {}

Return_TAC_Stmt::~Return_TAC_Stmt() {
    if(return_val) delete return_val;
}

void Return_TAC_Stmt::print(FILE *file) {
    fprintf(file, "    %s\n", to_string().c_str());
}

string Return_TAC_Stmt::to_string() {
    if(return_val) {
        return "return " + return_val->to_string();
    } else {
        return "return";
    }
}