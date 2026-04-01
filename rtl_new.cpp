#include "rtl_new.h"
#include <cstdio>
#include <cstdlib>

Register_RTL_Opd::Register_RTL_Opd(string name) : reg_name(name) {}
Register_RTL_Opd::~Register_RTL_Opd() {}

void Register_RTL_Opd::print(FILE *file) {
    fprintf(file, "%s", reg_name.c_str());
}

string Register_RTL_Opd::to_string() {
    return reg_name;
}

string Register_RTL_Opd::get_name() {
    return reg_name;
}

bool Register_RTL_Opd::is_float_register() {
    return !reg_name.empty() && reg_name[0] == 'f';
}

Memory_RTL_Opd::Memory_RTL_Opd(string name) : var_name(name) {}
Memory_RTL_Opd::~Memory_RTL_Opd() {}

void Memory_RTL_Opd::print(FILE *file) {
    fprintf(file, "%s", var_name.c_str());
}

string Memory_RTL_Opd::to_string() {
    return var_name;
}

string Memory_RTL_Opd::get_name() {
    return var_name;
}

Label_RTL_Opd::Label_RTL_Opd(string name) : label_name(name) {}
Label_RTL_Opd::~Label_RTL_Opd() {}

void Label_RTL_Opd::print(FILE *file) {
    fprintf(file, "%s", label_name.c_str());
}

string Label_RTL_Opd::to_string() {
    return label_name;
}

string Label_RTL_Opd::get_name() {
    return label_name;
}

Const_RTL_Opd::Const_RTL_Opd(int val) : int_value(val), float_value(0), is_float(false) {}
Const_RTL_Opd::Const_RTL_Opd(double val) : int_value(0), float_value(val), is_float(true) {}
Const_RTL_Opd::~Const_RTL_Opd() {}

void Const_RTL_Opd::print(FILE *file) {
    if (is_float) {
        // Print with 2 decimal places (removes trailing zeros automatically)
        fprintf(file, "%.2f", float_value);
    } else {
        fprintf(file, "%d", int_value);
    }
}

string Const_RTL_Opd::to_string() {
    char buf[32];
    if (is_float) {
        snprintf(buf, sizeof(buf), "%.6f", float_value);
    } else {
        snprintf(buf, sizeof(buf), "%d", int_value);
    }
    return string(buf);
}

Compute_RTL_Stmt::Compute_RTL_Stmt(RTL_Opd *d, RTL_Opd *op1, RTL_Op operation, RTL_Opd *op2)
    : dest(d), opd1(op1), opd2(op2), op(operation) {}

Compute_RTL_Stmt::~Compute_RTL_Stmt() {
    delete dest;
    delete opd1;
    if (opd2) delete opd2;
}

void Compute_RTL_Stmt::print(FILE *file) {
    // Check if this is a float operation
    bool is_float_op = (op == RTL_OP_FLOAD);
    
    // Check if destination is a float register
    if (!is_float_op && dest) {
        Register_RTL_Opd *reg_dest = dynamic_cast<Register_RTL_Opd*>(dest);
        if (reg_dest && reg_dest->is_float_register()) {
            is_float_op = true;
        }
    }
    
    // Check if any operand is a float constant
    if (!is_float_op && opd2) {
        Const_RTL_Opd *const_opd2 = dynamic_cast<Const_RTL_Opd*>(opd2);
        if (const_opd2 && const_opd2->get_is_float()) {
            is_float_op = true;
        }
    }
    if (!is_float_op && opd1) {
        Const_RTL_Opd *const_opd1 = dynamic_cast<Const_RTL_Opd*>(opd1);
        if (const_opd1 && const_opd1->get_is_float()) {
            is_float_op = true;
        }
    }
    
    // Check if any operand register is a float register
    if (!is_float_op && opd1) {
        Register_RTL_Opd *reg_opd1 = dynamic_cast<Register_RTL_Opd*>(opd1);
        if (reg_opd1 && reg_opd1->is_float_register()) {
            is_float_op = true;
        }
    }
    if (!is_float_op && opd2) {
        Register_RTL_Opd *reg_opd2 = dynamic_cast<Register_RTL_Opd*>(opd2);
        if (reg_opd2 && reg_opd2->is_float_register()) {
            is_float_op = true;
        }
    }
    
    const char *op_str = "";
    switch (op) {
        case RTL_OP_ILOAD: op_str = is_float_op ? "iLoad.d" : "iLoad"; break;
        case RTL_OP_FLOAD: op_str = "iLoad.d"; break;
        case RTL_OP_ADD: op_str = is_float_op ? "add.d" : "add"; break;
        case RTL_OP_SUB: op_str = is_float_op ? "sub.d" : "sub"; break;
        case RTL_OP_MUL: op_str = is_float_op ? "mul.d" : "mul"; break;
        case RTL_OP_DIV: op_str = is_float_op ? "div.d" : "div"; break;
        case RTL_OP_SLT: op_str = is_float_op ? "slt.d" : "slt"; break;
        case RTL_OP_SGT: op_str = is_float_op ? "sgt.d" : "sgt"; break;
        case RTL_OP_SLE: op_str = is_float_op ? "sle.d" : "sle"; break;
        case RTL_OP_SGE: op_str = is_float_op ? "sge.d" : "sge"; break;
        case RTL_OP_SEQ: op_str = is_float_op ? "seq.d" : "seq"; break;
        case RTL_OP_SNE: op_str = is_float_op ? "sne.d" : "sne"; break;
        case RTL_OP_NOT: op_str = "not"; break;
        case RTL_OP_NEG: op_str = "neg"; break;
        case RTL_OP_UMINUS: op_str = is_float_op ? "uminus.d" : "uminus"; break;
        case RTL_OP_AND: op_str = "and"; break;
        case RTL_OP_OR: op_str = "or"; break;
        case RTL_OP_ADD_D: op_str = "add.d"; break;
        case RTL_OP_SUB_D: op_str = "sub.d"; break;
        case RTL_OP_MUL_D: op_str = "mul.d"; break;
        case RTL_OP_DIV_D: op_str = "div.d"; break;
        case RTL_OP_SLT_D: op_str = "slt.d"; break;
        case RTL_OP_SLE_D: op_str = "sle.d"; break;
        case RTL_OP_MOVE: op_str = "move"; break;
        case RTL_OP_MOVT: op_str = "movt"; break;
        case RTL_OP_MOVF: op_str = "movf"; break;
    }

    fprintf(file, "    %s:\t", op_str);
    bool compare_no_dest =
    op == RTL_OP_SLT_D ||
    op == RTL_OP_SLE_D;
    if (!compare_no_dest) {
    dest->print(file);
    fprintf(file, " <- ");
}
    
    // For iLoad/fLoad, opd1 is NULL and the constant is in opd2
    if (op == RTL_OP_ILOAD || op == RTL_OP_FLOAD) {
        if (opd2) {
            opd2->print(file);
        }
        if (op == RTL_OP_ILOAD && !is_float_op) {
    Register_RTL_Opd *reg_dest = dynamic_cast<Register_RTL_Opd*>(dest);
    Const_RTL_Opd *const_val = dynamic_cast<Const_RTL_Opd*>(opd2);

    if (reg_dest && const_val &&
        reg_dest->get_name() == "v0" &&
        const_val->to_string() == "1") {
        fprintf(file, "\t\t\t;; Loading 1 in v0 to indicate syscall to print integter value");
    }
    else {
        fprintf(file, "\t\t\t;; Loading integer number ");
        if (opd2) opd2->print(file);
    }
} else if (op == RTL_OP_FLOAD || is_float_op) {
            fprintf(file, "\t\t\t;; Loading float number ");
            if (opd2) opd2->print(file);
        }
    } else {
        if (op == RTL_OP_MOVE) {
    if (opd1) opd1->print(file);
}
else if (op == RTL_OP_MOVT || op == RTL_OP_MOVF) {
    if (opd1) opd1->print(file);
    fprintf(file, " , 0");
}
else {
    if (opd1) opd1->print(file);
    if (opd2) {
        fprintf(file, " , ");
        opd2->print(file);
    }
}
    }
    fprintf(file, "\n");
}

Load_RTL_Stmt::Load_RTL_Stmt(RTL_Opd *d, RTL_Opd *s, bool float_op) : dest(d), source(s), is_float(float_op) {}
Load_RTL_Stmt::~Load_RTL_Stmt() {
    delete dest;
    delete source;
}

void Load_RTL_Stmt::print(FILE *file) {
    Const_RTL_Opd *const_src = dynamic_cast<Const_RTL_Opd*>(source);

bool float_load = false;
Register_RTL_Opd *reg_dest = dynamic_cast<Register_RTL_Opd*>(dest);
if (reg_dest && reg_dest->is_float_register())
    float_load = true;

if (const_src)
    fprintf(file, "    iLoad%s:\t", float_load ? ".d" : "");
else
    fprintf(file, "    load%s:\t", float_load ? ".d" : "");

    dest->print(file);
    fprintf(file, " <- ");
    source->print(file);


    if (reg_dest && reg_dest->get_name() == "a0") {
        fprintf(file, "\t\t\t;; Moving the value to be printed into register a0\n");
    }
    else if (const_src) {
        fprintf(file, "\t\t\t;; Loading integer number ");
        source->print(file);
        fprintf(file, "\n");
    }
    else {
        fprintf(file, "\t\t\t;; Loading variable ");
        source->print(file);
        fprintf(file, " into register\n");
    }
}

Loadaddr_RTL_Stmt::Loadaddr_RTL_Stmt(RTL_Opd *d, RTL_Opd *s, string str_val) : dest(d), source(s), string_value(str_val) {}
Loadaddr_RTL_Stmt::~Loadaddr_RTL_Stmt() {
    delete dest;
    delete source;
}

void Loadaddr_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    load_addr:\t");
    dest->print(file);
    fprintf(file, " <- ");
    source->print(file);
    if (!string_value.empty()) {
        fprintf(file, "\t\t\t;; String = \"%s\"", string_value.c_str());
    }
    fprintf(file, "\n");
}

Store_RTL_Stmt::Store_RTL_Stmt(RTL_Opd *d, RTL_Opd *s, bool float_op) : dest(d), source(s), is_float(float_op) {}
Store_RTL_Stmt::~Store_RTL_Stmt() {
    delete dest;
    delete source;
}

void Store_RTL_Stmt::print(FILE *file) {
    bool float_store = false;
Register_RTL_Opd *reg_src = dynamic_cast<Register_RTL_Opd*>(source);
if (reg_src && reg_src->is_float_register())
    float_store = true;

fprintf(file, "    store%s:\t", float_store ? ".d" : "");
    dest->print(file);
    fprintf(file, " <- ");
    source->print(file);
    fprintf(file, "\n");
}

Label_RTL_Stmt::Label_RTL_Stmt(string name) : label_name(name) {}
Label_RTL_Stmt::~Label_RTL_Stmt() {}

void Label_RTL_Stmt::print(FILE *file) {
    fprintf(file, "%s:\n", label_name.c_str());
}

Goto_RTL_Stmt::Goto_RTL_Stmt(string name) : label_name(name) {}
Goto_RTL_Stmt::~Goto_RTL_Stmt() {}

void Goto_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    goto:\t\t%s\n", label_name.c_str());
}

Bgtz_RTL_Stmt::Bgtz_RTL_Stmt(RTL_Opd *o, string label) : opd(o), label_name(label) {}
Bgtz_RTL_Stmt::~Bgtz_RTL_Stmt() {
    delete opd;
}

void Bgtz_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    bgtz:\t");
    opd->print(file);
    fprintf(file, " , %s\n", label_name.c_str());
}

Write_RTL_Stmt::Write_RTL_Stmt() {}
Write_RTL_Stmt::~Write_RTL_Stmt() {}

void Write_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    write\t\t\t\t;; This is where syscall will be made\n");
}

Read_RTL_Stmt::Read_RTL_Stmt() {}
Read_RTL_Stmt::~Read_RTL_Stmt() {}

void Read_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    read\t\t\t\t;; This is where syscall will be made\n");
}
