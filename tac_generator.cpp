#include "tac_generator.h"
#include "tac_str.h"
using namespace std;

// Initialize static instance
TAC_Generator *TAC_Generator::instance = NULL;

TAC_Generator::TAC_Generator() 
    : temp_counter(0), stemp_counter(0), label_counter(0) {}

TAC_Generator* TAC_Generator::get_instance() {
    if (instance == NULL) {
        instance = new TAC_Generator();
    }
    return instance;
}

TAC_Generator::~TAC_Generator() {}

TAC_Opd* TAC_Generator::create_new_temp(bool is_float) {
    TAC_Opd *temp = new Temp_TAC_Opd(temp_counter, is_float);
    temp_counter++;
    return temp;
}

TAC_Opd* TAC_Generator::create_new_stemp(bool is_float) {
    TAC_Opd *stemp = new Temp_TAC_Opd(stemp_counter, is_float ? FLOAT_DATA_TYPE : INT_DATA_TYPE, "stemp");
    stemp_counter++;
    return stemp;
}

Label_TAC_Opd* TAC_Generator::create_new_label() {
    Label_TAC_Opd *label = new Label_TAC_Opd(label_counter);
    label_counter++;
    return label;
}

uint32_t TAC_Generator::allocate_label_id() {
    return label_counter++;
}

void TAC_Generator::reset_counters() {
    temp_counter = 0;
    stemp_counter = 0;
    label_counter = 0;
}
