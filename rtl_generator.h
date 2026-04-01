#ifndef RTL_GENERATOR_H
#define RTL_GENERATOR_H

#include "tac_str.h"
#include "rtl_new.h"
#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;

class RTL_Generator {
private:
    static RTL_Generator* instance;

    list<RTL_Stmt*> rtl_list;
    void reset_block_allocator();
    map<string, string> active_temp_map;
    vector<string> free_pool;

    string allocate_reg();
    void free_reg(const string &reg);

    RTL_Opd* materialize_operand(
        TAC_Opd *opd,
        string &used_reg,
        bool &is_temp
    );

    Compute_RTL_Stmt::RTL_Op
    map_tac_to_rtl_op(Compute_TAC_Stmt::TAC_Op op);

public:
    static RTL_Generator* get_instance();

    RTL_Generator();
    void reset();

    list<RTL_Stmt*> generate_rtl(list<TAC_Stmt*>& tac_stmts);
    void print_rtl(FILE *file = stdout);
};

#endif