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
    int label_counter;
    map<string, int> reg_allocation;
    int next_register;
    
    RTL_Generator();

public:
    static RTL_Generator* get_instance();
    void reset();
    int get_next_label();
    string create_label();
    list<RTL_Stmt*> generate_rtl(list<TAC_Stmt*> &tac_stmts);
    RTL_Opd* get_rtl_opd(TAC_Opd *tac_opd);
};

#endif
