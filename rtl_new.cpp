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
        fprintf(file, "%.2f", float_value);
    } else {
        fprintf(file, "%d", int_value);
    }
}

string Const_RTL_Opd::to_string() {
    char buf[32];
    if (is_float) {
        snprintf(buf, sizeof(buf), "%.2f", float_value);
    } else {
        snprintf(buf, sizeof(buf), "%d", int_value);
    }
    return string(buf);
}

Compute_RTL_Stmt::Compute_RTL_Stmt(RTL_Opd *d, RTL_Opd *op1, RTL_Op operation, RTL_Opd *op2)
    : dest(d), opd1(op1), op(operation), opd2(op2) {}

Compute_RTL_Stmt::~Compute_RTL_Stmt() {
    delete dest;
    delete opd1;
    if (opd2) delete opd2;
}

void Compute_RTL_Stmt::print(FILE *file) {
    const char *op_str = "";
    switch (op) {
        case RTL_OP_ILOAD: op_str = "iLoad"; break;
        case RTL_OP_FLOAD: op_str = "fLoad"; break;
        case RTL_OP_ADD: op_str = "add"; break;
        case RTL_OP_SUB: op_str = "sub"; break;
        case RTL_OP_MUL: op_str = "mul"; break;
        case RTL_OP_DIV: op_str = "div"; break;
        case RTL_OP_SLT: op_str = "slt"; break;
        case RTL_OP_SGT: op_str = "sgt"; break;
        case RTL_OP_SLE: op_str = "sle"; break;
        case RTL_OP_SGE: op_str = "sge"; break;
        case RTL_OP_SEQ: op_str = "seq"; break;
        case RTL_OP_SNE: op_str = "sne"; break;
        case RTL_OP_NOT: op_str = "not"; break;
        case RTL_OP_NEG: op_str = "neg"; break;
        case RTL_OP_UMINUS: op_str = "uminus"; break;
        case RTL_OP_AND: op_str = "and"; break;
        case RTL_OP_OR: op_str = "or"; break;
    }

    fprintf(file, "    %s:\t", op_str);
    dest->print(file);
    fprintf(file, " <- ");
    
    // For iLoad/fLoad, opd1 is NULL and the constant is in opd2
    if (op == RTL_OP_ILOAD || op == RTL_OP_FLOAD) {
        if (opd2) {
            opd2->print(file);
        }
        if (op == RTL_OP_ILOAD) {
            fprintf(file, "\t\t\t;; Loading integer number ");
            if (opd2) opd2->print(file);
        }
    } else {
        if (opd1) opd1->print(file);
        if (opd2) {
            fprintf(file, " , ");
            opd2->print(file);
        }
    }
    fprintf(file, "\n");
}

Load_RTL_Stmt::Load_RTL_Stmt(RTL_Opd *d, RTL_Opd *s) : dest(d), source(s) {}
Load_RTL_Stmt::~Load_RTL_Stmt() {
    delete dest;
    delete source;
}

void Load_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    load:\t");
    dest->print(file);
    fprintf(file, " <- ");
    source->print(file);
    fprintf(file, "\t\t\t;; Loading variable ");
    source->print(file);
    fprintf(file, " into register\n");
}

Loadaddr_RTL_Stmt::Loadaddr_RTL_Stmt(RTL_Opd *d, RTL_Opd *s) : dest(d), source(s) {}
Loadaddr_RTL_Stmt::~Loadaddr_RTL_Stmt() {
    delete dest;
    delete source;
}

void Loadaddr_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    load_addr:\t");
    dest->print(file);
    fprintf(file, " <- ");
    source->print(file);
    fprintf(file, "\n");
}

Store_RTL_Stmt::Store_RTL_Stmt(RTL_Opd *d, RTL_Opd *s) : dest(d), source(s) {}
Store_RTL_Stmt::~Store_RTL_Stmt() {
    delete dest;
    delete source;
}

void Store_RTL_Stmt::print(FILE *file) {
    fprintf(file, "    store:\t");
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
