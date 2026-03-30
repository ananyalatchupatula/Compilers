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
    static map<string, int> string_indices;  // Track string literals to indices
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
    static int get_string_index(const string& str_value) {
        if (string_indices.count(str_value) == 0) {
            string_indices[str_value] = string_indices.size();
        }
        return string_indices[str_value];
    }
    static map<string, int> get_string_indices() {
        return string_indices;
    }
    void reset();
    int get_next_label();
    string create_label();
    list<RTL_Stmt*> generate_rtl(list<TAC_Stmt*> &tac_stmts);
    RTL_Opd* get_rtl_opd(TAC_Opd *tac_opd);
};

#endif
