#ifndef ASM_GENERATOR_H
#define ASM_GENERATOR_H

#include "rtl_new.h"
#include "asm_new.h"
#include <string>
#include <list>
#include <map>
#include <vector>

using std::string;
using std::list;
using std::map;
using std::vector;

class ASM_Generator {
private:
    static ASM_Generator* instance;
    
    // MIPS register mapping
    string mips_reg(const string& rtl_reg);
    
    // Stack frame management
    map<string, int> local_var_offsets;  // variable name -> offset from $fp
    int current_offset;                   // Current stack offset for locals
    int local_variable_count;             // Number of local variables in current function
    
    // Procedure tracking
    string current_procedure;
    
    // Generated ASM statements
    list<ASM_Stmt*> asm_statements;
    
    // Helper methods
    void process_rtl_statement(RTL_Stmt* stmt);
    void process_load(Load_RTL_Stmt* stmt);
    void process_store(Store_RTL_Stmt* stmt);
    void process_compute(Compute_RTL_Stmt* stmt);
    void process_label(Label_RTL_Stmt* stmt);
    void process_bgtz(Bgtz_RTL_Stmt* stmt);
    void process_goto(Goto_RTL_Stmt* stmt);
    void process_call(Call_RTL_Stmt* stmt);
    void process_pop(Pop_RTL_Stmt* stmt);
    void process_push(Push_RTL_Stmt* stmt);
    void process_return(Return_RTL_Stmt* stmt);
    void process_move(Move_RTL_Stmt* stmt);
    
    // Operand conversion
    ASM_Opd* convert_operand(RTL_Opd* opd);
    
    ASM_Generator();

public:
    static ASM_Generator* get_instance();
    
    // Main entry point
    list<ASM_Stmt*> generate_asm(list<RTL_Stmt*>& rtl_statements);
    
    // Reset for new compilation
    void reset();
};

#endif
