#include "rtl_generator_new.h"
#include "tac_str.h"
#include <cstdio>
#include <sstream>
#include <set>
#include <map>

RTL_Generator* RTL_Generator::instance = NULL;

RTL_Generator::RTL_Generator() 
    : int_temp_reg_index(0), float_temp_reg_index(0), compute_reg_index(0) {
}



RTL_Generator* RTL_Generator::get_instance() {
    if (instance == NULL) {
        instance = new RTL_Generator();
    }
    return instance;
}

void RTL_Generator::reset() {
    int_temp_reg_index = 0;
    float_temp_reg_index = 0;
    compute_reg_index = 0;
    temp_to_reg.clear();
}

string RTL_Generator::allocate_temp_register(bool is_float) {
    if (is_float) {
        string reg = float_temp_regs[float_temp_reg_index];
        float_temp_reg_index = (float_temp_reg_index + 1) % 15;
        return reg;
    } else {
        string reg = int_temp_regs[int_temp_reg_index];
        int_temp_reg_index = (int_temp_reg_index + 1) % 11;
        return reg;
    }
}

string RTL_Generator::get_temp_register(const string& temp_name, bool is_float) {
    if (temp_to_reg.count(temp_name)) {
        return temp_to_reg[temp_name];
    }
    string reg = allocate_temp_register(is_float);
    temp_to_reg[temp_name] = reg;
    return reg;
}

string RTL_Generator::get_load_register() {
    // For loads: use v0 first, then t1
    if (int_temp_reg_index == 0) {
        int_temp_reg_index = 1;
        return int_temp_regs[0];  // v0
    } else {
        int_temp_reg_index = 0;
        return int_temp_regs[2];  // t1
    }
}

string RTL_Generator::get_compute_register() {
    // For compute results: always use t0
    return int_temp_regs[1];  // t0
}

string RTL_Generator::get_next_int_temp_reg() {
    string reg = int_temp_regs[int_temp_reg_index];
    int_temp_reg_index = (int_temp_reg_index + 1) % 11;
    return reg;
}

string RTL_Generator::get_next_float_temp_reg() {
    string reg = float_temp_regs[float_temp_reg_index];
    float_temp_reg_index = (float_temp_reg_index + 1) % 15;
    return reg;
}

RTL_Opd* RTL_Generator::convert_tac_opd_to_rtl(TAC_Opd* tac_opd) {
    if (!tac_opd) return NULL;
    
    Const_TAC_Opd *const_opd = dynamic_cast<Const_TAC_Opd*>(tac_opd);
    Var_TAC_Opd *var_opd = dynamic_cast<Var_TAC_Opd*>(tac_opd);
    Temp_TAC_Opd *temp_opd = dynamic_cast<Temp_TAC_Opd*>(tac_opd);

    if (const_opd) {
        string const_str = const_opd->to_string();
        if (const_str.find('.') != string::npos) {
            return new Const_RTL_Opd(atof(const_str.c_str()));
        } else {
            return new Const_RTL_Opd(atoi(const_str.c_str()));
        }
    } else if (var_opd) {
        return new Memory_RTL_Opd(var_opd->to_string());
    } else if (temp_opd) {
        return new Memory_RTL_Opd(temp_opd->to_string());
    }
    
    return NULL;
}

RTL_Stmt* RTL_Generator::convert_tac_stmt_to_rtl(TAC_Stmt* tac_stmt) {
    // Placeholder - actual conversion happens in generate_rtl
    return NULL;
}

list<RTL_Stmt*> RTL_Generator::generate_rtl(list<TAC_Stmt*>& tac_stmts) {
    list<RTL_Stmt*> rtl_stmts;
    reset();  // Clear temporary to register mappings
    
    // Pre-pass 1: identify temps that are used as operands in AND/OR operations
    set<string> temps_used_in_logical_op;
    list<Compute_TAC_Stmt*> and_or_stmts;  // Store AND/OR statements in order
    for (auto tac_stmt : tac_stmts) {
        Compute_TAC_Stmt *compute_stmt = dynamic_cast<Compute_TAC_Stmt*>(tac_stmt);
        if (compute_stmt) {
            int op = compute_stmt->get_op();
            if (op == Compute_TAC_Stmt::TAC_OP_AND || op == Compute_TAC_Stmt::TAC_OP_OR) {
                and_or_stmts.push_back(compute_stmt);
                TAC_Opd *opd1 = compute_stmt->get_opd1();
                TAC_Opd *opd2 = compute_stmt->get_opd2();
                if (opd1) temps_used_in_logical_op.insert(opd1->to_string());
                if (opd2) temps_used_in_logical_op.insert(opd2->to_string());
            }
        }
    }
    
    // Pre-pass 2: for each AND/OR, identify which temps are its operands
    map<Compute_TAC_Stmt*, set<string>> comparisons_per_and_or;
    map<string, Compute_TAC_Stmt*> temp_to_and_or;  // Maps temp to the AND/OR it's used in
    map<string, string> operand2_register_assignments;  // Maps operand2 temp to register
    map<Compute_TAC_Stmt*, int> cycling_count_per_and_or;
    
    for (auto and_or_stmt : and_or_stmts) {
        // The operands of this AND/OR are the temps we're looking for
        TAC_Opd *opd1 = and_or_stmt->get_opd1();
        TAC_Opd *opd2 = and_or_stmt->get_opd2();
        string opd1_str = opd1 ? opd1->to_string() : "";
        string opd2_str = opd2 ? opd2->to_string() : "";
        
        if (!opd1_str.empty()) {
            comparisons_per_and_or[and_or_stmt].insert(opd1_str);
            temp_to_and_or[opd1_str] = and_or_stmt;
        }
        if (!opd2_str.empty()) {
            comparisons_per_and_or[and_or_stmt].insert(opd2_str);
            temp_to_and_or[opd2_str] = and_or_stmt;
        }
    }
    
    // Pre-pass 3: for each comparison in an AND/OR, identify its operand2's register
    for (auto tac_stmt : tac_stmts) {
        Compute_TAC_Stmt *compute_stmt = dynamic_cast<Compute_TAC_Stmt*>(tac_stmt);
        if (!compute_stmt) continue;
        
        int op = compute_stmt->get_op();
        bool is_comparison = (op == Compute_TAC_Stmt::TAC_OP_LT || op == Compute_TAC_Stmt::TAC_OP_GT ||
                              op == Compute_TAC_Stmt::TAC_OP_LE || op == Compute_TAC_Stmt::TAC_OP_GE ||
                              op == Compute_TAC_Stmt::TAC_OP_EQ || op == Compute_TAC_Stmt::TAC_OP_NE);
        
        if (is_comparison) {
            string result_str = compute_stmt->get_result()->to_string();
            if (temp_to_and_or.count(result_str)) {
                // This comparison is part of an AND/OR - assign operand2 to t1 for this group
                TAC_Opd *opd2 = compute_stmt->get_opd2();
                if (opd2) {
                    string opd2_str = opd2->to_string();
                    operand2_register_assignments[opd2_str] = "t1";
                }
            }
        }
    }
    
    Compute_TAC_Stmt* current_and_or_context = NULL;
    int current_cycling_count = 0;
    
    for (auto tac_stmt : tac_stmts) {
        int_temp_reg_index = 0;  // Reset load register allocation for each statement
        
        Assign_TAC_Stmt *assign_stmt = dynamic_cast<Assign_TAC_Stmt*>(tac_stmt);
        Compute_TAC_Stmt *compute_stmt = dynamic_cast<Compute_TAC_Stmt*>(tac_stmt);
        Cond_Goto_TAC_Stmt *cond_goto = dynamic_cast<Cond_Goto_TAC_Stmt*>(tac_stmt);
        Goto_TAC_Stmt *goto_stmt = dynamic_cast<Goto_TAC_Stmt*>(tac_stmt);
        Label_TAC_Stmt *label_stmt = dynamic_cast<Label_TAC_Stmt*>(tac_stmt);
        Print_TAC_Stmt *print_stmt = dynamic_cast<Print_TAC_Stmt*>(tac_stmt);
        Read_TAC_Stmt *read_stmt = dynamic_cast<Read_TAC_Stmt*>(tac_stmt);

        if (label_stmt) {
            Label_TAC_Opd *label_opd = label_stmt->get_label();
            string label_str = label_opd->to_string();
            rtl_stmts.push_back(new Label_RTL_Stmt(label_str));
            current_and_or_context = NULL;
            current_cycling_count = 0;
        }
        else if (goto_stmt) {
            Label_TAC_Opd *label_opd = goto_stmt->get_label();
            string label_str = label_opd->to_string();
            rtl_stmts.push_back(new Goto_RTL_Stmt(label_str));
        }
        else if (cond_goto) {
            TAC_Opd *cond = cond_goto->get_cond();
            if (cond) {
                Label_TAC_Opd *label_opd = cond_goto->get_label();
                string label_str = label_opd->to_string();
                
                Temp_TAC_Opd *temp_cond = dynamic_cast<Temp_TAC_Opd*>(cond);
                Var_TAC_Opd *var_cond = dynamic_cast<Var_TAC_Opd*>(cond);
                
                if ((temp_cond || var_cond) && temp_to_reg.count(cond->to_string())) {
                    // Condition is a temp in a register
                    rtl_stmts.push_back(new Bgtz_RTL_Stmt(
                        new Register_RTL_Opd(temp_to_reg[cond->to_string()]),
                        label_str
                    ));
                } else {
                    // Load condition into v0 and branch
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd("v0"),
                        new Memory_RTL_Opd(cond->to_string()),
                        false
                    ));
                    rtl_stmts.push_back(new Bgtz_RTL_Stmt(
                        new Register_RTL_Opd("v0"),
                        label_str
                    ));
                }
            }
        }
        else if (assign_stmt) {
            TAC_Opd *dest = assign_stmt->get_result();
            TAC_Opd *source = assign_stmt->get_opd1();
            string dest_str = dest->to_string();
            string source_str = source->to_string();

            Const_TAC_Opd *const_source = dynamic_cast<Const_TAC_Opd*>(source);
            Temp_TAC_Opd *temp_dest = dynamic_cast<Temp_TAC_Opd*>(dest);
            Temp_TAC_Opd *temp_source = dynamic_cast<Temp_TAC_Opd*>(source);
            
            if (const_source) {
                // Assigning a constant
                string const_str = const_source->to_string();
                
                if (const_str.find('.') != string::npos) {
                    rtl_stmts.push_back(new Compute_RTL_Stmt(
                        new Register_RTL_Opd("v0"),
                        NULL,
                        Compute_RTL_Stmt::RTL_OP_FLOAD,
                        new Const_RTL_Opd(atof(const_str.c_str()))
                    ));
                } else {
                    rtl_stmts.push_back(new Compute_RTL_Stmt(
                        new Register_RTL_Opd("v0"),
                        NULL,
                        Compute_RTL_Stmt::RTL_OP_ILOAD,
                        new Const_RTL_Opd(atoi(const_str.c_str()))
                    ));
                }
                
                // Store to destination
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(dest_str),
                    new Register_RTL_Opd("v0"),
                    false
                ));
                temp_to_reg.erase(dest_str);
            } 
            else if (temp_source && temp_to_reg.count(source_str)) {
                // Source is a temp in a register - use that register
                string src_reg = temp_to_reg[source_str];
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(dest_str),
                    new Register_RTL_Opd(src_reg),
                    false
                ));
                temp_to_reg.erase(dest_str);
            }
            else {
                // Assigning a variable or unmapped temp - standard load/store
                rtl_stmts.push_back(new Load_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    new Memory_RTL_Opd(source_str),
                    false
                ));
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(dest_str),
                    new Register_RTL_Opd("v0"),
                    false
                ));
                temp_to_reg.erase(dest_str);
            }
        }
        else if (compute_stmt) {
            TAC_Opd *result = compute_stmt->get_result();
            TAC_Opd *opd1 = compute_stmt->get_opd1();
            TAC_Opd *opd2 = compute_stmt->get_opd2();
            int op = compute_stmt->get_op();
            string result_str = result->to_string();
            string opd1_str = opd1->to_string();
            
            // Check if this is a comparison operation
            bool is_comparison = (op == Compute_TAC_Stmt::TAC_OP_LT || op == Compute_TAC_Stmt::TAC_OP_GT ||
                                  op == Compute_TAC_Stmt::TAC_OP_LE || op == Compute_TAC_Stmt::TAC_OP_GE ||
                                  op == Compute_TAC_Stmt::TAC_OP_EQ || op == Compute_TAC_Stmt::TAC_OP_NE);
            
            // Determine operand2 register - will be set by cycling logic below
            string opd2_reg = "t1";  // default

            // Load operand 1 into appropriate register
            string src_reg = "v0";
            Const_TAC_Opd *const_opd1 = dynamic_cast<Const_TAC_Opd*>(opd1);
            Temp_TAC_Opd *temp_opd1 = dynamic_cast<Temp_TAC_Opd*>(opd1);
            
            if (const_opd1) {
                // Operand 1 is a constant
                string const_str = const_opd1->to_string();
                if (const_str.find('.') != string::npos) {
                    rtl_stmts.push_back(new Compute_RTL_Stmt(
                        new Register_RTL_Opd(src_reg),
                        NULL,
                        Compute_RTL_Stmt::RTL_OP_FLOAD,
                        new Const_RTL_Opd(atof(const_str.c_str()))
                    ));
                } else {
                    rtl_stmts.push_back(new Compute_RTL_Stmt(
                        new Register_RTL_Opd(src_reg),
                        NULL,
                        Compute_RTL_Stmt::RTL_OP_ILOAD,
                        new Const_RTL_Opd(atoi(const_str.c_str()))
                    ));
                }
            } 
            else if (temp_opd1 && temp_to_reg.count(opd1_str)) {
                // Operand 1 is a temp already in a register
                src_reg = temp_to_reg[opd1_str];
            }
            else {
                // Operand 1 is a variable - load it
                rtl_stmts.push_back(new Load_RTL_Stmt(
                    new Register_RTL_Opd(src_reg),
                    new Memory_RTL_Opd(opd1_str),
                    false
                ));
            }

            // Map TAC operations to RTL operations
            Compute_RTL_Stmt::RTL_Op rtl_op;
            switch (op) {
                case Compute_TAC_Stmt::TAC_OP_PLUS:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_ADD;
                    break;
                case Compute_TAC_Stmt::TAC_OP_MINUS:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_SUB;
                    break;
                case Compute_TAC_Stmt::TAC_OP_MULT:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_MUL;
                    break;
                case Compute_TAC_Stmt::TAC_OP_DIV:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_DIV;
                    break;
                case Compute_TAC_Stmt::TAC_OP_LT:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_SLT;
                    break;
                case Compute_TAC_Stmt::TAC_OP_GT:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_SGT;
                    break;
                case Compute_TAC_Stmt::TAC_OP_LE:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_SLE;
                    break;
                case Compute_TAC_Stmt::TAC_OP_GE:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_SGE;
                    break;
                case Compute_TAC_Stmt::TAC_OP_EQ:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_SEQ;
                    break;
                case Compute_TAC_Stmt::TAC_OP_NE:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_SNE;
                    break;
                case Compute_TAC_Stmt::TAC_OP_NOT:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_NOT;
                    break;
                case Compute_TAC_Stmt::TAC_OP_UMINUS:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_UMINUS;
                    break;
                case Compute_TAC_Stmt::TAC_OP_AND:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_AND;
                    break;
                case Compute_TAC_Stmt::TAC_OP_OR:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_OR;
                    break;
                default:
                    rtl_op = Compute_RTL_Stmt::RTL_OP_ADD;
            }

            string dest_reg = get_compute_register();
            
            // AND/OR operations always output to v0
            if (op == Compute_TAC_Stmt::TAC_OP_AND || op == Compute_TAC_Stmt::TAC_OP_OR) {
                dest_reg = "v0";
                // When we hit an AND/OR, update context for subsequent comparisons
                current_and_or_context = compute_stmt;
                current_cycling_count = 0;
            }
            // Special handling for NOT: swap registers
            else if (op == Compute_TAC_Stmt::TAC_OP_NOT) {
                if (src_reg == "v0") {
                    dest_reg = "t0";
                } else if (src_reg == "t0") {
                    dest_reg = "v0";
                } else {
                    dest_reg = "t0";
                }
            }
            // Apply cycling for comparisons that are operands in AND/OR
            else if (is_comparison && temp_to_and_or.count(result_str)) {
                // This comparison result is used in AND/OR - apply cycling
                Compute_TAC_Stmt* target_and_or = temp_to_and_or[result_str];
                
                // If we're not in this AND/OR's context yet, switch context
                if (current_and_or_context != target_and_or) {
                    current_and_or_context = target_and_or;
                    current_cycling_count = 0;
                }
                
                // Apply cycling based on current position in this AND/OR group
                // Result register cycles: t0, t1, t2, ...
                // Operand2 register STAYS at t1 for the whole group
                if (current_cycling_count == 0) {
                    dest_reg = "t0";
                    opd2_reg = "t1";
                } else if (current_cycling_count == 1) {
                    dest_reg = "t1";
                    opd2_reg = "t1";  // Use same register for operand2
                } else {
                    dest_reg = "t2";
                    opd2_reg = "t1";  // Use same register for operand2
                }
                
                current_cycling_count++;
            }
            
            // If this temp is an operand2 in an AND/OR comparison, override its register
            if (operand2_register_assignments.count(result_str)) {
                dest_reg = operand2_register_assignments[result_str];
            }
            
            if (!opd2) {
                // Unary operation
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd(dest_reg),
                    new Register_RTL_Opd(src_reg),
                    rtl_op,
                    NULL
                ));
            } else {
                // Binary operation - load operand 2
                Const_TAC_Opd *const_opd2 = dynamic_cast<Const_TAC_Opd*>(opd2);
                Temp_TAC_Opd *temp_opd2 = dynamic_cast<Temp_TAC_Opd*>(opd2);
                string opd2_str = opd2->to_string();
                
                if (const_opd2) {
                    // Operand 2 is a constant
                    string const_str = const_opd2->to_string();
                    if (const_str.find('.') != string::npos) {
                        rtl_stmts.push_back(new Compute_RTL_Stmt(
                            new Register_RTL_Opd(opd2_reg),
                            NULL,
                            Compute_RTL_Stmt::RTL_OP_FLOAD,
                            new Const_RTL_Opd(atof(const_str.c_str()))
                        ));
                    } else {
                        rtl_stmts.push_back(new Compute_RTL_Stmt(
                            new Register_RTL_Opd(opd2_reg),
                            NULL,
                            Compute_RTL_Stmt::RTL_OP_ILOAD,
                            new Const_RTL_Opd(atoi(const_str.c_str()))
                        ));
                    }
                }
                else if (temp_opd2 && temp_to_reg.count(opd2_str)) {
                    // Operand 2 is a temp already in a register
                    opd2_reg = temp_to_reg[opd2_str];
                }
                else {
                    // Operand 2 is a variable
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd(opd2_reg),
                        new Memory_RTL_Opd(opd2_str),
                        false
                    ));
                }
                
                // Perform the computation
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd(dest_reg),
                    new Register_RTL_Opd(src_reg),
                    rtl_op,
                    new Register_RTL_Opd(opd2_reg)
                ));
            }
            // Store result and track it
            Temp_TAC_Opd *temp_result = dynamic_cast<Temp_TAC_Opd*>(result);
            if (temp_result) {
                // For temporaries, track the register
                // If this temp is an operand2 in an AND/OR comparison, use the designated register
                if (operand2_register_assignments.count(result_str)) {
                    dest_reg = operand2_register_assignments[result_str];
                }
                temp_to_reg[result_str] = dest_reg;
            } else {
                // For variables, store to memory
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(result_str),
                    new Register_RTL_Opd(dest_reg),
                    false
                ));
                temp_to_reg.erase(result_str);
            }
        }
        else if (print_stmt) {
            TAC_Opd *opd = print_stmt->get_opd();
            if (opd) {
                // Load syscall type (1 for print int, 4 for print string)
                Const_TAC_Opd *const_opd = dynamic_cast<Const_TAC_Opd*>(opd);
                bool is_string = (const_opd && const_opd->get_is_string());
                
                int syscall_num = is_string ? 4 : 1;
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    NULL,
                    Compute_RTL_Stmt::RTL_OP_ILOAD,
                    new Const_RTL_Opd(syscall_num)
                ));
                
                // Load value into a0
                Temp_TAC_Opd *temp_opd = dynamic_cast<Temp_TAC_Opd*>(opd);
                string opd_str = opd->to_string();
                
                if (const_opd && is_string) {
                    // For string constants - would need string label handling
                } else if (temp_opd && temp_to_reg.count(opd_str)) {
                    // Value is in a register
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd("a0"),
                        new Register_RTL_Opd(temp_to_reg[opd_str]),
                        false
                    ));
                } else {
                    // Value is in memory
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd("a0"),
                        new Memory_RTL_Opd(opd_str),
                        false
                    ));
                }
                
                // System call
                rtl_stmts.push_back(new Write_RTL_Stmt());
            }
        }
        else if (read_stmt) {
            TAC_Opd *opd = read_stmt->get_var();
            if (opd) {
                // Load 5 (read int) into v0
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    NULL,
                    Compute_RTL_Stmt::RTL_OP_ILOAD,
                    new Const_RTL_Opd(5)
                ));
                
                // System call to read
                rtl_stmts.push_back(new Read_RTL_Stmt());
                
                // Store result into the variable
                string opd_str = opd->to_string();
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(opd_str),
                    new Register_RTL_Opd("v0"),
                    false
                ));
                temp_to_reg.erase(opd_str);
            }
        }
    }

    return rtl_stmts;
}

RTL_Generator::~RTL_Generator() {
}
