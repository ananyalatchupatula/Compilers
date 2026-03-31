#ifndef RTL_GENERATOR_NEW_H
#define RTL_GENERATOR_NEW_H

#include <list>
#include <map>
#include <string>
#include "tac_str.h"
#include "rtl_new.h"

using namespace std;

class RTL_Generator {
private:
    static RTL_Generator *instance;

    list<RTL_Stmt*> rtl_stmts;
    map<string, string> temp_register_map;
    string last_compute_dest;
    RTL_Generator();
    int scratch_idx;
    vector<string> scratch_regs;
    bool last_stmt_was_not;
    string get_register_for_temp(string name, bool is_float = false);
    RTL_Opd* convert_tac_operand(TAC_Opd *opd);
    RTL_Opd* materialize_operand(TAC_Opd *opd);

public:
    static RTL_Generator* get_instance();

    ~RTL_Generator();

    void reset();
    list<RTL_Stmt*> generate_rtl(list<TAC_Stmt*> &tac_stmts);
    void print_rtl(FILE *file);
};

#endif