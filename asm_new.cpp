#include "asm_new.h"
#include <iostream>

using std::cerr;
using std::endl;

// ===== ASM_Register_Opd =====
void ASM_Register_Opd::print(FILE *file) {
    fprintf(file, "%s", reg_name.c_str());
}

string ASM_Register_Opd::to_string() {
    return reg_name;
}

string ASM_Register_Opd::get_name() {
    return reg_name;
}

// ===== ASM_Mem_Opd =====
void ASM_Mem_Opd::print(FILE *file) {
    fprintf(file, "%d($fp)", offset);
}

string ASM_Mem_Opd::to_string() {
    return to_string() + "($fp)";
}

string ASM_Mem_Opd::get_name() {
    return var_name;
}

// ===== ASM_Int_Const_Opd =====
void ASM_Int_Const_Opd::print(FILE *file) {
    fprintf(file, "%d", value);
}

string ASM_Int_Const_Opd::to_string() {
    return std::to_string(value);
}

string ASM_Int_Const_Opd::get_name() {
    return std::to_string(value);
}

// ===== ASM_Label_Opd =====
void ASM_Label_Opd::print(FILE *file) {
    fprintf(file, "%s", label_name.c_str());
}

string ASM_Label_Opd::to_string() {
    return label_name;
}

string ASM_Label_Opd::get_name() {
    return label_name;
}

// ===== ASM_Str_Const_Opd =====
void ASM_Str_Const_Opd::print(FILE *file) {
    fprintf(file, "\"%s\"", str_value.c_str());
}

string ASM_Str_Const_Opd::to_string() {
    return "\"" + str_value + "\"";
}

string ASM_Str_Const_Opd::get_name() {
    return str_value;
}

// ===== Compute_ASM_Stmt =====
Compute_ASM_Stmt::~Compute_ASM_Stmt() {
    delete dest;
    delete opd1;
    delete opd2;
}

void Compute_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    ");
    
    switch (op) {
        case ASM_OP_ADD:
            fprintf(file, "add ");
            dest->print(file);
            fprintf(file, ", ");
            opd1->print(file);
            fprintf(file, ", ");
            opd2->print(file);
            break;
        case ASM_OP_SUB:
            fprintf(file, "sub ");
            dest->print(file);
            fprintf(file, ", ");
            opd1->print(file);
            fprintf(file, ", ");
            opd2->print(file);
            break;
        case ASM_OP_MUL:
            fprintf(file, "mul ");
            dest->print(file);
            fprintf(file, ", ");
            opd1->print(file);
            fprintf(file, ", ");
            opd2->print(file);
            break;
        case ASM_OP_DIV:
            fprintf(file, "div ");
            opd1->print(file);
            fprintf(file, ", ");
            opd2->print(file);
            break;
        case ASM_OP_LI:
            fprintf(file, "li ");
            dest->print(file);
            fprintf(file, ", ");
            opd1->print(file);
            break;
        case ASM_OP_MOVE:
            fprintf(file, "move ");
            dest->print(file);
            fprintf(file, ", ");
            opd1->print(file);
            break;
    }
    fprintf(file, "\n");
}

// ===== Load_ASM_Stmt =====
Load_ASM_Stmt::~Load_ASM_Stmt() {
    delete dest;
    delete source;
}

void Load_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    ");
    // If source is a constant, use li (load immediate), otherwise use lw (load word)
    if (dynamic_cast<ASM_Int_Const_Opd*>(source)) {
        fprintf(file, "li ");
    } else {
        fprintf(file, "lw ");
    }
    dest->print(file);
    fprintf(file, ", ");
    source->print(file);
    fprintf(file, "\n");
}

// ===== Store_ASM_Stmt =====
Store_ASM_Stmt::~Store_ASM_Stmt() {
    delete source;
    delete dest;
}

void Store_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    sw ");
    source->print(file);
    fprintf(file, ", ");
    dest->print(file);
    fprintf(file, "\n");
}

// ===== Label_ASM_Stmt =====
Label_ASM_Stmt::~Label_ASM_Stmt() {}

void Label_ASM_Stmt::print(FILE *file) {
    fprintf(file, "%s:\n", label_name.c_str());
}

// ===== Goto_ASM_Stmt =====
Goto_ASM_Stmt::~Goto_ASM_Stmt() {}

void Goto_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    j %s\n", label_name.c_str());
}

// ===== If_Goto_ASM_Stmt =====
If_Goto_ASM_Stmt::~If_Goto_ASM_Stmt() {
    delete test_register;
}

void If_Goto_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    bgtz ");
    test_register->print(file);
    fprintf(file, ", %s\n", label_name.c_str());
}

// ===== Jump_Reg_ASM_Stmt =====
Jump_Reg_ASM_Stmt::~Jump_Reg_ASM_Stmt() {
    delete reg;
}

void Jump_Reg_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    jr ");
    reg->print(file);
    fprintf(file, "\n");
}

// ===== Call_ASM_Stmt =====
Call_ASM_Stmt::~Call_ASM_Stmt() {}

void Call_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    jal %s\n", proc_name.c_str());
}

// ===== Syscall_ASM_Stmt =====
Syscall_ASM_Stmt::~Syscall_ASM_Stmt() {}

void Syscall_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    syscall\n");
}

// ===== Move_ASM_Stmt =====
Move_ASM_Stmt::~Move_ASM_Stmt() {
    delete dest;
    delete source;
}

void Move_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    move ");
    dest->print(file);
    fprintf(file, ", ");
    source->print(file);
    fprintf(file, "\n");
}

// ===== NOP_ASM_Stmt =====
NOP_ASM_Stmt::~NOP_ASM_Stmt() {}

void NOP_ASM_Stmt::print(FILE *file) {
    fprintf(file, "    nop\n");
}
