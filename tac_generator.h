#ifndef TAC_GENERATOR_H
#define TAC_GENERATOR_H

#include <string>
#include <list>
#include <stdint.h>
using namespace std;

class TAC_Opd;
class Label_TAC_Opd;

// TAC Generator Utility Class
class TAC_Generator {
private:
    uint32_t temp_counter;
    uint32_t stemp_counter;
    uint32_t label_counter;
    
    // Singleton pattern
    static TAC_Generator *instance;
    TAC_Generator();
    
public:
    static TAC_Generator* get_instance();
    
    ~TAC_Generator();
    
    // Generate unique temporary variable names
    TAC_Opd* create_new_temp(bool is_float = false);
    
    // Generate unique string temporary variable names (for ternary results)
    TAC_Opd* create_new_stemp(bool is_float = false);
    
    // Generate unique labels
    Label_TAC_Opd* create_new_label();
    
    // Get next label ID without creating object
    uint32_t allocate_label_id();
    
    // Reset counters
    void reset_counters();
    
    uint32_t get_temp_counter() { return temp_counter; }
    uint32_t get_stemp_counter() { return stemp_counter; }
    uint32_t get_label_counter() { return label_counter; }
};

#endif
