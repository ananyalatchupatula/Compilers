#ifndef RTL_GENERATOR_NEW_H
#define RTL_GENERATOR_NEW_H

#include <string>
#include <list>
#include <map>
#include <cstdio>
#include "rtl_new.h"
#include "tac_str.h"

using namespace std;

class RTL_Generator {
private:
    static RTL_Generator* instance;
    map<string, string> temp_to_reg;  // Map temporaries to assigned registers
    
    // Register allocation tracking
    int int_temp_reg_index;      // For v0, t0-t9 loads
    int float_temp_reg_index;    // For f2, f4, f6, f8, etc.
    int compute_reg_index;       // For cycling through compute result registers
    
    RTL_Generator();
    
    // Available registers based on SPIM architecture from images
    string int_temp_regs[11] = {"v0", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};
    string float_temp_regs[15] = {"f2", "f4", "f6", "f8", "f10", "f12", "f14", "f16", "f18", "f20", "f22", "f24", "f26", "f28", "f30"};
    
public:
    static RTL_Generator* get_instance();
    
    // Generate RTL from TAC
    list<RTL_Stmt*> generate_rtl(list<TAC_Stmt*>& tac_stmts);
    
    // Register allocation
    string allocate_temp_register(bool is_float = false);
    string get_temp_register(const string& temp_name, bool is_float = false);
    
    // Reset for new function
    void reset();
    
    // TAC to RTL conversion
    RTL_Opd* convert_tac_opd_to_rtl(TAC_Opd* tac_opd);
    RTL_Stmt* convert_tac_stmt_to_rtl(TAC_Stmt* tac_stmt);
    
    // Register helpers
    string get_next_int_temp_reg();
    string get_next_float_temp_reg();
    string get_load_register();    // For variable loads - prefer v0
    string get_compute_register(); // For computation results - use t0+
    
    // Destructor
    virtual ~RTL_Generator();
};

#endif // RTL_GENERATOR_NEW_H
