#include "asm_generator.h"
#include <iostream>

using std::cerr;
using std::endl;

ASM_Generator* ASM_Generator::instance = NULL;

ASM_Generator::ASM_Generator() 
    : current_offset(0), local_variable_count(0) {
}

ASM_Generator* ASM_Generator::get_instance() {
    if (instance == NULL)
        instance = new ASM_Generator();
    return instance;
}

void ASM_Generator::reset() {
    asm_statements.clear();
    local_var_offsets.clear();
    current_offset = 0;
    local_variable_count = 0;
    current_procedure = "";
}

string ASM_Generator::mips_reg(const string& rtl_reg) {
    // Map RTL register names to MIPS register names
    if (rtl_reg == "v0") return "$v0";
    if (rtl_reg == "v1") return "$v1";
    if (rtl_reg == "a0") return "$a0";
    if (rtl_reg == "a1") return "$a1";
    if (rtl_reg == "a2") return "$a2";
    if (rtl_reg == "a3") return "$a3";
    
    if (rtl_reg == "t0") return "$t0";
    if (rtl_reg == "t1") return "$t1";
    if (rtl_reg == "t2") return "$t2";
    if (rtl_reg == "t3") return "$t3";
    if (rtl_reg == "t4") return "$t4";
    if (rtl_reg == "t5") return "$t5";
    if (rtl_reg == "t6") return "$t6";
    if (rtl_reg == "t7") return "$t7";
    if (rtl_reg == "t8") return "$t8";
    if (rtl_reg == "t9") return "$t9";
    
    if (rtl_reg == "sp") return "$sp";
    if (rtl_reg == "fp") return "$fp";
    if (rtl_reg == "ra") return "$ra";
    if (rtl_reg == "zero") return "$zero";
    
    // Float registers
    if (rtl_reg == "f0") return "$f0";
    if (rtl_reg == "f2") return "$f2";
    if (rtl_reg == "f4") return "$f4";
    if (rtl_reg == "f6") return "$f6";
    if (rtl_reg == "f8") return "$f8";
    
    return "$" + rtl_reg;  // Fallback
}

ASM_Opd* ASM_Generator::convert_operand(RTL_Opd* opd) {
    if (auto reg_opd = dynamic_cast<Register_RTL_Opd*>(opd)) {
        return new ASM_Register_Opd(mips_reg(reg_opd->get_name()));
    } else if (auto const_opd = dynamic_cast<Const_RTL_Opd*>(opd)) {
        // Constants are handled separately; this is placeholder
        return new ASM_Int_Const_Opd(0);
    } else if (auto mem_opd = dynamic_cast<Memory_RTL_Opd*>(opd)) {
        int offset = local_var_offsets.count(mem_opd->get_name()) 
            ? local_var_offsets[mem_opd->get_name()]
            : -(local_variable_count * 4 + 4);
        return new ASM_Mem_Opd(mem_opd->get_name(), offset);
    } else if (auto label_opd = dynamic_cast<Label_RTL_Opd*>(opd)) {
        return new ASM_Label_Opd(label_opd->get_name());
    }
    return nullptr;
}

void ASM_Generator::process_load(Load_RTL_Stmt* stmt) {
    // RTL Load: dest <- source
    // Can be load from memory or load immediate
    
    ASM_Opd* dest = convert_operand(stmt->get_dest());
    RTL_Opd* src = stmt->get_source();
    
    if (auto const_opd = dynamic_cast<Const_RTL_Opd*>(src)) {
        // Load immediate - extract the actual value
        int value = const_opd->get_int_value();
        asm_statements.push_back(new Load_ASM_Stmt(
            dest,
            new ASM_Int_Const_Opd(value)
        ));
    } else {
        // Load from memory
        ASM_Opd* source = convert_operand(src);
        asm_statements.push_back(new Load_ASM_Stmt(dest, source));
    }
}

void ASM_Generator::process_store(Store_RTL_Stmt* stmt) {
    ASM_Opd* source = convert_operand(stmt->get_source());
    ASM_Opd* dest = convert_operand(stmt->get_dest());
    asm_statements.push_back(new Store_ASM_Stmt(source, dest));
}

void ASM_Generator::process_compute(Compute_RTL_Stmt* stmt) {
    ASM_Opd* dest = convert_operand(stmt->get_dest());
    ASM_Opd* opd1 = convert_operand(stmt->get_opd1());
    ASM_Opd* opd2 = stmt->get_opd2() ? convert_operand(stmt->get_opd2()) : nullptr;
    
    Compute_ASM_Stmt::ASM_Op asm_op = Compute_ASM_Stmt::ASM_OP_ADD;
    
    switch (stmt->get_op()) {
        case Compute_RTL_Stmt::RTL_OP_ADD:
            asm_op = Compute_ASM_Stmt::ASM_OP_ADD;
            break;
        case Compute_RTL_Stmt::RTL_OP_SUB:
            asm_op = Compute_ASM_Stmt::ASM_OP_SUB;
            break;
        case Compute_RTL_Stmt::RTL_OP_MUL:
            asm_op = Compute_ASM_Stmt::ASM_OP_MUL;
            break;
        case Compute_RTL_Stmt::RTL_OP_DIV:
            asm_op = Compute_ASM_Stmt::ASM_OP_DIV;
            break;
        case Compute_RTL_Stmt::RTL_OP_MOVE:
            asm_op = Compute_ASM_Stmt::ASM_OP_MOVE;
            break;
        default:
            return;
    }
    
    asm_statements.push_back(new Compute_ASM_Stmt(dest, opd1, asm_op, opd2));
}

void ASM_Generator::process_label(Label_RTL_Stmt* stmt) {
    asm_statements.push_back(new Label_ASM_Stmt(stmt->get_label()));
}

void ASM_Generator::process_bgtz(Bgtz_RTL_Stmt* stmt) {
    ASM_Opd *test_reg = convert_operand(stmt->get_opd());
    string label = stmt->get_label();
    asm_statements.push_back(new If_Goto_ASM_Stmt(test_reg, label));
}

void ASM_Generator::process_goto(Goto_RTL_Stmt *stmt) {
    string label = stmt->get_label();
    asm_statements.push_back(new Goto_ASM_Stmt(label));
}

void ASM_Generator::process_call(Call_RTL_Stmt* stmt) {
    asm_statements.push_back(new Call_ASM_Stmt(stmt->get_func_name()));
}

void ASM_Generator::process_push(Push_RTL_Stmt* stmt) {
    ASM_Opd *value = convert_operand(stmt->get_value());
    // Push is stack operation - generate move to stack
    asm_statements.push_back(new Store_ASM_Stmt(value, new ASM_Mem_Opd("sp", 0)));
}

void ASM_Generator::process_pop(Pop_RTL_Stmt* stmt) {
    // Pop is stack operation - generate load from stack
    asm_statements.push_back(new Load_ASM_Stmt(new ASM_Register_Opd("$sp"), new ASM_Mem_Opd("sp", 0)));
}

void ASM_Generator::process_return(Return_RTL_Stmt* stmt) {
    // Return_RTL_Stmt doesn't have get_source, just generates jr $ra
    asm_statements.push_back(new Jump_Reg_ASM_Stmt(
        new ASM_Register_Opd("$ra")
    ));
}

void ASM_Generator::process_move(Move_RTL_Stmt* stmt) {
    // Move statements will be handled later
    // For now, just add a NOP placeholder
    asm_statements.push_back(new NOP_ASM_Stmt());
}

void ASM_Generator::process_rtl_statement(RTL_Stmt* stmt) {
    if (auto load = dynamic_cast<Load_RTL_Stmt*>(stmt)) {
        process_load(load);
    } else if (auto store = dynamic_cast<Store_RTL_Stmt*>(stmt)) {
        process_store(store);
    } else if (auto compute = dynamic_cast<Compute_RTL_Stmt*>(stmt)) {
        process_compute(compute);
    } else if (auto label = dynamic_cast<Label_RTL_Stmt*>(stmt)) {
        process_label(label);
    } else if (auto bgtz = dynamic_cast<Bgtz_RTL_Stmt*>(stmt)) {
        process_bgtz(bgtz);
    } else if (auto go = dynamic_cast<Goto_RTL_Stmt*>(stmt)) {
        process_goto(go);
    } else if (auto call = dynamic_cast<Call_RTL_Stmt*>(stmt)) {
        process_call(call);
    } else if (auto push = dynamic_cast<Push_RTL_Stmt*>(stmt)) {
        process_push(push);
    } else if (auto pop = dynamic_cast<Pop_RTL_Stmt*>(stmt)) {
        process_pop(pop);
    } else if (auto ret = dynamic_cast<Return_RTL_Stmt*>(stmt)) {
        process_return(ret);
    } else if (auto move = dynamic_cast<Move_RTL_Stmt*>(stmt)) {
        process_move(move);
    }
}

list<ASM_Stmt*> ASM_Generator::generate_asm(list<RTL_Stmt*>& rtl_statements) {
    reset();
    
    for (RTL_Stmt* stmt : rtl_statements) {
        process_rtl_statement(stmt);
    }
    
    return asm_statements;
}
