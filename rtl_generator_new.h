#ifndef RTL_GENERATOR_NEW_H
#define RTL_GENERATOR_NEW_H

#include "tac_str.h"
#include "rtl_new.h"
#include <string>
#include <list>
#include <map>

using namespace std;

class RTL_Generator {
private:
    static RTL_Generator *instance;
    static map<string, bool> float_vars;  // Track which variables are floats
    int label_counter;
    map<string, int> reg_allocation;
    int next_register;
    
    RTL_Generator();

public:
    static RTL_Generator* get_instance();
    static bool is_float_var(const string& var_name) {
        return float_vars.count(var_name) && float_vars[var_name];
    }
    static void set_float_var(const string& var_name, bool is_float) {
        float_vars[var_name] = is_float;
    }
    void reset();
    int get_next_label();
    string create_label();
    list<RTL_Stmt*> generate_rtl(list<TAC_Stmt*> &tac_stmts);
    RTL_Opd* get_rtl_opd(TAC_Opd *tac_opd);
};

#endif
