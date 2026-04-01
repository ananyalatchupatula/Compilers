#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include <string>
#include <list>
#include <stdint.h>
#include <map>
using namespace std;

class TAC_Opd;
class Label_TAC_Opd;

class TAC_Generator {
private:
    uint32_t temp_counter;
    uint32_t stemp_counter;
    uint32_t label_counter;
    uint32_t string_counter;
    map<string, string> string_label_map;

    static TAC_Generator *instance;
    TAC_Generator();

public:
    static TAC_Generator* get_instance();
    ~TAC_Generator();

    TAC_Opd* create_new_temp(bool is_float = false);
    TAC_Opd* create_new_stemp(bool is_float = false);
    Label_TAC_Opd* create_new_label();

    string get_or_create_string_label(const string &literal);
    string get_string_literal_from_label(const string &label) const;

    uint32_t allocate_label_id();
    void reset_counters();

    uint32_t get_temp_counter() { return temp_counter; }
    uint32_t get_stemp_counter() { return stemp_counter; }
    uint32_t get_label_counter() { return label_counter; }
};

#endif