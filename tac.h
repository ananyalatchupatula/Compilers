#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include <string>
#include "ast.h"

extern FILE *tac_file;   // declaration only

class TACGenerator {
public:
    int temp_counter;
    int stemp_counter;
    int label_counter;
    
    TACGenerator() : temp_counter(0), 
    stemp_counter(0), label_counter(0) {}
    
    std::string new_temp();
    std::string new_stemp();
    std::string new_label();
    void reset();
};

extern TACGenerator tac_gen;

void generate_TAC(ASTNode *root);

#endif