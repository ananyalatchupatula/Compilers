#include "rtl_generator_new.h"
#include "tac_str.h"
#include <cstdio>
#include <sstream>

RTL_Generator* RTL_Generator::instance = NULL;

RTL_Generator::RTL_Generator() : label_counter(0) {}

RTL_Generator* RTL_Generator::get_instance() {
    if (instance == NULL) {
        instance = new RTL_Generator();
    }
    return instance;
}

int RTL_Generator::get_next_label() {
    return label_counter++;
}

string RTL_Generator::create_label() {
    stringstream ss;
    ss << "Label" << get_next_label();
    return ss.str();
}

RTL_Opd* RTL_Generator::get_rtl_opd(TAC_Opd *tac_opd) {
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

list<RTL_Stmt*> RTL_Generator::generate_rtl(list<TAC_Stmt*> &tac_stmts) {
    list<RTL_Stmt*> rtl_stmts;
    map<string, string> temp_to_register;  // Track which register holds which temp value

    for (auto tac_stmt : tac_stmts) {
        Assign_TAC_Stmt *assign_stmt = dynamic_cast<Assign_TAC_Stmt*>(tac_stmt);
        Compute_TAC_Stmt *compute_stmt = dynamic_cast<Compute_TAC_Stmt*>(tac_stmt);
        Cond_Goto_TAC_Stmt *cond_goto = dynamic_cast<Cond_Goto_TAC_Stmt*>(tac_stmt);
        Goto_TAC_Stmt *goto_stmt = dynamic_cast<Goto_TAC_Stmt*>(tac_stmt);
        Label_TAC_Stmt *label_stmt = dynamic_cast<Label_TAC_Stmt*>(tac_stmt);
        Print_TAC_Stmt *print_stmt = dynamic_cast<Print_TAC_Stmt*>(tac_stmt);
        Read_TAC_Stmt *read_stmt = dynamic_cast<Read_TAC_Stmt*>(tac_stmt);

        if (assign_stmt) {
            // Assignment: dest = source
            TAC_Opd *dest = assign_stmt->get_result();
            TAC_Opd *source = assign_stmt->get_opd1();
            string dest_str = dest->to_string();
            string source_str = source->to_string();

            Const_TAC_Opd *const_source = dynamic_cast<Const_TAC_Opd*>(source);
            Temp_TAC_Opd *temp_source = dynamic_cast<Temp_TAC_Opd*>(source);
            Temp_TAC_Opd *temp_dest = dynamic_cast<Temp_TAC_Opd*>(dest);
            
            if (const_source) {
                // Assigning a constant: use iLoad
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
                if (temp_dest) {
                    // Destination is a temp - just track the register
                    temp_to_register[dest_str] = "v0";
                } else {
                    // Destination is a variable - store to memory
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd("v0")
                    ));
                    temp_to_register.erase(dest_str);
                }
            } else if (temp_source && temp_to_register.count(source_str)) {
                // Source is a temp that's in a register - use that register directly
                string src_reg = temp_to_register[source_str];
                if (temp_dest) {
                    // Both are temps - just track the register for destination
                    temp_to_register[dest_str] = src_reg;
                } else {
                    // Dest is a variable - store to memory
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd(src_reg)
                    ));
                    temp_to_register.erase(dest_str);
                }
                temp_to_register.erase(source_str);  // source is no longer "hot"
            } else {
                // Assigning a variable/temp - standard load/store
                rtl_stmts.push_back(new Load_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    new Memory_RTL_Opd(source_str)
                ));
                if (temp_dest) {
                    // Destination is a temp - just track the register
                    temp_to_register[dest_str] = "v0";
                } else {
                    // Destination is a variable - store to memory
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd("v0")
                    ));
                    temp_to_register.erase(dest_str);
                }
            }
        }
        else if (compute_stmt) {
            TAC_Opd *result = compute_stmt->get_result();
            TAC_Opd *opd1 = compute_stmt->get_opd1();
            TAC_Opd *opd2 = compute_stmt->get_opd2();
            int op = compute_stmt->get_op();
            string result_str = result->to_string();
            string opd1_str = opd1->to_string();

            // Check if opd1 is already in a register from a previous computation
            string src_register = "v0";
            Temp_TAC_Opd *temp_opd1 = dynamic_cast<Temp_TAC_Opd*>(opd1);
            if (temp_opd1 && temp_to_register.count(opd1_str)) {
                src_register = temp_to_register[opd1_str];
            } else {
                // Load operand 1 into v0
                rtl_stmts.push_back(new Load_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    new Memory_RTL_Opd(opd1_str)
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

            // Load operand 2 if it's a binary operation
            if (opd2) {
                Const_TAC_Opd *const_opd2 = dynamic_cast<Const_TAC_Opd*>(opd2);
                if (const_opd2) {
                    string const_str = const_opd2->to_string();
                    if (const_str.find('.') != string::npos) {
                        rtl_stmts.push_back(new Compute_RTL_Stmt(
                            new Register_RTL_Opd("t1"),
                            NULL,
                            Compute_RTL_Stmt::RTL_OP_FLOAD,
                            new Const_RTL_Opd(atof(const_str.c_str()))
                        ));
                    } else {
                        rtl_stmts.push_back(new Compute_RTL_Stmt(
                            new Register_RTL_Opd("t1"),
                            NULL,
                            Compute_RTL_Stmt::RTL_OP_ILOAD,
                            new Const_RTL_Opd(atoi(const_str.c_str()))
                        ));
                    }
                } else {
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd("t1"),
                        new Memory_RTL_Opd(opd2->to_string())
                    ));
                }
            }

            // For unary operations (NOT, NEG), use v0 as destination (for branching)
            // For binary operations, use t0
            string dest_reg = (opd2) ? "t0" : "v0";

            // Perform the computation using the right source register
            if (opd2) {
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd(dest_reg),
                    new Register_RTL_Opd(src_register),
                    rtl_op,
                    new Register_RTL_Opd("t1")
                ));
            } else {
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd(dest_reg),
                    new Register_RTL_Opd(src_register),
                    rtl_op,
                    NULL
                ));
            }

            // Store result and track it in the appropriate register
            Temp_TAC_Opd *temp_result = dynamic_cast<Temp_TAC_Opd*>(result);
            if (temp_result) {
                // For temporaries, just track the register - don't store to memory
                temp_to_register[result_str] = dest_reg;
            } else {
                // For actual variables, store to memory
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(result_str),
                    new Register_RTL_Opd(dest_reg)
                ));
                temp_to_register.erase(result_str);
            }
        }
        else if (cond_goto) {
            TAC_Opd *cond = cond_goto->get_cond();
            if (cond) {
                Label_TAC_Opd *label_opd = cond_goto->get_label();
                string label_str = label_opd->to_string();
                string cond_str = cond->to_string();

                // Check if condition is in a register
                Temp_TAC_Opd *temp_cond = dynamic_cast<Temp_TAC_Opd*>(cond);
                if (temp_cond && temp_to_register.count(cond_str)) {
                    // Use the register directly
                    rtl_stmts.push_back(new Bgtz_RTL_Stmt(
                        new Register_RTL_Opd(temp_to_register[cond_str]),
                        label_str
                    ));
                } else {
                    // Load condition and branch if non-zero
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd("v0"),
                        new Memory_RTL_Opd(cond_str)
                    ));
                    rtl_stmts.push_back(new Bgtz_RTL_Stmt(
                        new Register_RTL_Opd("v0"),
                        label_str
                    ));
                }
            }
            // Don't clear register tracking - temporaries may live across branches
        }
        else if (goto_stmt) {
            Label_TAC_Opd *label_opd = goto_stmt->get_label();
            string label_str = label_opd->to_string();
            rtl_stmts.push_back(new Goto_RTL_Stmt(label_str));
            temp_to_register.clear();
        }
        else if (label_stmt) {
            Label_TAC_Opd *label_opd = label_stmt->get_label();
            string label_str = label_opd->to_string();
            rtl_stmts.push_back(new Label_RTL_Stmt(label_str));
            temp_to_register.clear();  // Clear register state at labels
        }
        else if (print_stmt) {
            TAC_Opd *opd = print_stmt->get_opd();
            if (opd) {
                Const_TAC_Opd *const_opd = dynamic_cast<Const_TAC_Opd*>(opd);
                bool is_string = (const_opd && const_opd->get_is_string());
                
                // Load syscall type into v0
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    NULL,
                    Compute_RTL_Stmt::RTL_OP_ILOAD,
                    new Const_RTL_Opd(is_string ? 4 : 1)  // 4 for string, 1 for integer
                ));
                
                // Load value to print into a0 (argument register)
                if (is_string) {
                    // For strings, use load_addr with the string constant
                    rtl_stmts.push_back(new Loadaddr_RTL_Stmt(
                        new Register_RTL_Opd("a0"),
                        new Memory_RTL_Opd(const_opd->get_string_value())
                    ));
                } else {
                    // For variables/integers, use load
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd("a0"),
                        new Memory_RTL_Opd(opd->to_string())
                    ));
                }
                
                // System call
                rtl_stmts.push_back(new Write_RTL_Stmt());
            }
        }
        else if (read_stmt) {
            TAC_Opd *opd = read_stmt->get_var();
            if (opd) {
                // Load 5 into v0 to indicate syscall type (read integer)
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    NULL,
                    Compute_RTL_Stmt::RTL_OP_ILOAD,
                    new Const_RTL_Opd(5)
                ));
                
                // System call to read
                rtl_stmts.push_back(new Read_RTL_Stmt());
                
                // Store the read value into the variable
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(opd->to_string()),
                    new Register_RTL_Opd("v0")
                ));
                temp_to_register.erase(opd->to_string());
            }
        }
    }

    return rtl_stmts;
}
