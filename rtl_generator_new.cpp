#include "rtl_generator_new.h"
#include "tac_str.h"
#include <cstdio>
#include <sstream>

RTL_Generator* RTL_Generator::instance = NULL;
map<string, bool> RTL_Generator::float_vars;
map<string, int> RTL_Generator::string_indices;

// Helper function to get register name based on float type
// Integer regs: v0, t0, t1, t2
// Float regs: f2, f4, f6
static string get_register(const string& int_reg, bool is_float) {
    if (!is_float) return int_reg;
    
    if (int_reg == "v0") return "f2";
    if (int_reg == "t0") return "f4";
    if (int_reg == "t1") return "f6";
    if (int_reg == "t2") return "f2";  // reuse f2 for t2 equivalents
    return int_reg;  // fallback
}

RTL_Generator::RTL_Generator() : label_counter(0) {}

RTL_Generator* RTL_Generator::get_instance() {
    if (instance == NULL) {
        instance = new RTL_Generator();
    }
    return instance;
}

void RTL_Generator::reset() {
    label_counter = 0;
    string_indices.clear();
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
    map<string, string> and_op_opd_regs;  // For AND/OR operations, track which register each operand should use
    
    // Clear and repopulate float_vars for this function
    float_vars.clear();

    // Pre-pass: identify all float variables and temporaries
    for (auto tac_stmt : tac_stmts) {
        Assign_TAC_Stmt *assign = dynamic_cast<Assign_TAC_Stmt*>(tac_stmt);
        Compute_TAC_Stmt *compute = dynamic_cast<Compute_TAC_Stmt*>(tac_stmt);
        Print_TAC_Stmt *print = dynamic_cast<Print_TAC_Stmt*>(tac_stmt);
        Read_TAC_Stmt *read = dynamic_cast<Read_TAC_Stmt*>(tac_stmt);
        
        if (assign) {
            TAC_Opd *result = assign->get_result();
            TAC_Opd *source = assign->get_opd1();
            Temp_TAC_Opd *temp = dynamic_cast<Temp_TAC_Opd*>(result);
            if (temp && temp->get_is_float()) {
                float_vars[result->to_string()] = true;
            }
            // If source is a float constant, mark result as float
            Const_TAC_Opd *const_src = dynamic_cast<Const_TAC_Opd*>(source);
            if (const_src && const_src->get_is_float()) {
                float_vars[result->to_string()] = true;
            }
        }
        if (compute) {
            TAC_Opd *result = compute->get_result();
            Temp_TAC_Opd *temp = dynamic_cast<Temp_TAC_Opd*>(result);
            if (temp && temp->get_is_float()) {
                float_vars[result->to_string()] = true;
            }
            // Check operands for float types to determine if result is float
            TAC_Opd *opd1 = compute->get_opd1();
            TAC_Opd *opd2 = compute->get_opd2();
            bool opd1_float = false, opd2_float = false;
            
            Const_TAC_Opd *const_opd1 = dynamic_cast<Const_TAC_Opd*>(opd1);
            if (const_opd1 && const_opd1->get_is_float()) opd1_float = true;
            Temp_TAC_Opd *temp_opd1 = dynamic_cast<Temp_TAC_Opd*>(opd1);
            if (temp_opd1 && temp_opd1->get_is_float()) opd1_float = true;
            // Check if operand is a float variable
            if (opd1->get_data_type() == FLOAT_DATA_TYPE) opd1_float = true;
            
            if (opd2) {
                Const_TAC_Opd *const_opd2 = dynamic_cast<Const_TAC_Opd*>(opd2);
                if (const_opd2 && const_opd2->get_is_float()) opd2_float = true;
                Temp_TAC_Opd *temp_opd2 = dynamic_cast<Temp_TAC_Opd*>(opd2);
                if (temp_opd2 && temp_opd2->get_is_float()) opd2_float = true;
                // Check if operand is a float variable
                if (opd2->get_data_type() == FLOAT_DATA_TYPE) opd2_float = true;
            }
            
            if (opd1_float || opd2_float) {
                float_vars[result->to_string()] = true;
                // Also mark operands as float if they're variables
                if (opd1_float) {
                    float_vars[opd1->to_string()] = true;
                }
                if (opd2_float) {
                    float_vars[opd2->to_string()] = true;
                }
            }
        }
    }

    // First pass: identify AND/OR operations and allocate registers for their operands
    for (auto tac_stmt : tac_stmts) {
        Compute_TAC_Stmt *compute = dynamic_cast<Compute_TAC_Stmt*>(tac_stmt);
        if (compute && (compute->get_op() == Compute_TAC_Stmt::TAC_OP_AND || 
                       compute->get_op() == Compute_TAC_Stmt::TAC_OP_OR)) {
            TAC_Opd *opd1 = compute->get_opd1();
            TAC_Opd *opd2 = compute->get_opd2();
            string opd1_str = opd1 ? opd1->to_string() : "";
            string opd2_str = opd2 ? opd2->to_string() : "";
            
            // Allocate first operand to t0 if not already assigned
            if (!opd1_str.empty() && and_op_opd_regs.count(opd1_str) == 0) {
                and_op_opd_regs[opd1_str] = "t0";
            }
            
            // Allocate second operand to t1 (or t2 if t1 is same as first)
            if (!opd2_str.empty() && and_op_opd_regs.count(opd2_str) == 0) {
                if (opd1_str == opd2_str) {
                    // Same operand used twice (unlikely but possible)
                    and_op_opd_regs[opd2_str] = "t0";
                } else {
                    and_op_opd_regs[opd2_str] = "t1";
                }
            }
        }
    }

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
            
            // Determine if this is a float assignment
            bool is_float_assign = false;
            if (const_source && const_source->get_is_float()) is_float_assign = true;
            if (float_vars.count(dest_str) && float_vars[dest_str]) is_float_assign = true;
            if (float_vars.count(source_str) && float_vars[source_str]) is_float_assign = true;
            
            string assign_reg = is_float_assign ? "f2" : "v0";
            
            if (const_source) {
                // Assigning a constant: use iLoad
                string const_str = const_source->to_string();
                if (const_str.find('.') != string::npos) {
                    rtl_stmts.push_back(new Compute_RTL_Stmt(
                        new Register_RTL_Opd(assign_reg), 
                        NULL, 
                        Compute_RTL_Stmt::RTL_OP_FLOAD, 
                        new Const_RTL_Opd(atof(const_str.c_str()))
                    ));
                } else {
                    rtl_stmts.push_back(new Compute_RTL_Stmt(
                        new Register_RTL_Opd(assign_reg),
                        NULL, 
                        Compute_RTL_Stmt::RTL_OP_ILOAD, 
                        new Const_RTL_Opd(atoi(const_str.c_str()))
                    ));
                }
                if (temp_dest) {
                    // Destination is a temp - always store to memory (needed for ternary branches)
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd(assign_reg),
                        is_float_assign
                    ));
                    temp_to_register.erase(dest_str);
                } else {
                    // Destination is a variable - store to memory
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd(assign_reg),
                        is_float_assign
                    ));
                    temp_to_register.erase(dest_str);
                }
            } else if (temp_source && temp_to_register.count(source_str)) {
                // Source is a temp that's in a register - use that register directly
                string src_reg = temp_to_register[source_str];
                bool is_float = float_vars.count(source_str) && float_vars[source_str];
                if (temp_dest) {
                    // Both are temps - always store to memory (needed for ternary branches)
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd(src_reg),
                        is_float
                    ));
                    temp_to_register.erase(dest_str);
                } else {
                    // Dest is a variable - store to memory
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd(src_reg),
                        is_float
                    ));
                    temp_to_register.erase(dest_str);
                }
                temp_to_register.erase(source_str);  // source is no longer "hot"
            } else {
                // Assigning a variable/temp - standard load/store
                bool src_is_float = float_vars.count(source_str) && float_vars[source_str];
                rtl_stmts.push_back(new Load_RTL_Stmt(
                    new Register_RTL_Opd("v0"),
                    new Memory_RTL_Opd(source_str),
                    src_is_float
                ));
                bool dest_is_float = float_vars.count(dest_str) && float_vars[dest_str];
                if (temp_dest) {
                    // Destination is a temp - always store to memory (needed for ternary branches)
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd("v0"),
                        dest_is_float
                    ));
                    temp_to_register.erase(dest_str);
                } else {
                    // Destination is a variable - store to memory
                    rtl_stmts.push_back(new Store_RTL_Stmt(
                        new Memory_RTL_Opd(dest_str),
                        new Register_RTL_Opd("v0"),
                        dest_is_float
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

            // Determine if this is a float operation
            bool is_float_op = false;
            Const_TAC_Opd *const_opd1 = dynamic_cast<Const_TAC_Opd*>(opd1);
            if (const_opd1 && const_opd1->get_is_float()) is_float_op = true;
            Temp_TAC_Opd *temp_opd1 = dynamic_cast<Temp_TAC_Opd*>(opd1);
            if (temp_opd1 && temp_opd1->get_is_float()) is_float_op = true;
            if (float_vars.count(opd1_str) && float_vars[opd1_str]) is_float_op = true;
            
            if (opd2) {
                Const_TAC_Opd *const_opd2 = dynamic_cast<Const_TAC_Opd*>(opd2);
                if (const_opd2 && const_opd2->get_is_float()) is_float_op = true;
                Temp_TAC_Opd *temp_opd2 = dynamic_cast<Temp_TAC_Opd*>(opd2);
                if (temp_opd2 && temp_opd2->get_is_float()) is_float_op = true;
                string opd2_str = opd2->to_string();
                if (float_vars.count(opd2_str) && float_vars[opd2_str]) is_float_op = true;
            }

            // Check if opd1 is already in a register from a previous computation
            string src_register = is_float_op ? "f2" : "v0";
            Temp_TAC_Opd *temp_opd1_cast = dynamic_cast<Temp_TAC_Opd*>(opd1);
            if (temp_opd1_cast && temp_to_register.count(opd1_str)) {
                src_register = temp_to_register[opd1_str];
                // If the register is a float register, mark as float operation
                if (src_register == "f2" || src_register == "f4" || src_register == "f6") {
                    is_float_op = true;
                }
            } else {
                // Load operand 1 into appropriate register

                Const_TAC_Opd *const_opd1 = dynamic_cast<Const_TAC_Opd*>(opd1);
                if (const_opd1) {
                    // Operand 1 is a constant - use iLoad/fLoad
                    string const_str = const_opd1->to_string();
                    if (const_str.find('.') != string::npos) {
                        rtl_stmts.push_back(new Compute_RTL_Stmt(
                            new Register_RTL_Opd(src_register),
                            NULL,
                            Compute_RTL_Stmt::RTL_OP_FLOAD,
                            new Const_RTL_Opd(atof(const_str.c_str()))
                        ));
                    } else {
                        rtl_stmts.push_back(new Compute_RTL_Stmt(
                            new Register_RTL_Opd(src_register),
                            NULL,
                            Compute_RTL_Stmt::RTL_OP_ILOAD,
                            new Const_RTL_Opd(atoi(const_str.c_str()))
                        ));
                    }
                } else {
                    // Operand 1 is a variable - use load
                    bool is_float = float_vars.count(opd1_str) && float_vars[opd1_str];
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd(src_register),
                        new Memory_RTL_Opd(opd1_str),
                        is_float
                    ));
                }
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

            // For unary operations, determine destination register based on source
            string dest_reg = is_float_op ? "f4" : "t0";  // Default for binary ops
            string opd2_reg = is_float_op ? "f6" : "t1";  // Default register for opd2
            
            if (!opd2) {
                // Unary operation: write to opposite register from input
                // For int: If input is v0, write to t0; if t0, write to v0; if t1, write to t2; etc.
                // For float: If input is f2, write to f4; if f4, write to f2; if f6, write to f2; etc.
                if (is_float_op) {
                    if (src_register == "f2") {
                        dest_reg = "f4";
                    } else if (src_register == "f4") {
                        dest_reg = "f2";
                    } else if (src_register == "f6") {
                        dest_reg = "f4";
                    } else {
                        dest_reg = "f2";  // fallback
                    }
                } else {
                    if (src_register == "v0") {
                        dest_reg = "t0";
                    } else if (src_register == "t0") {
                        dest_reg = "v0";
                    } else if (src_register == "t1") {
                        dest_reg = "t2";
                    } else if (src_register == "t2") {
                        dest_reg = "t1";
                    } else {
                        dest_reg = "v0";  // fallback
                    }
                }
            } else if (op == Compute_TAC_Stmt::TAC_OP_AND || op == Compute_TAC_Stmt::TAC_OP_OR) {
                // AND/OR always write to v0 (integer-only operations)
                dest_reg = "v0";
            } else if (and_op_opd_regs.count(result_str)) {
                // This result is an AND/OR operand - use the allocated register
                dest_reg = and_op_opd_regs[result_str];
            }
            
            // Now load operand 2
            if (opd2) {
                // First pass: determine what register opd2 will be in
                Const_TAC_Opd *const_opd2 = dynamic_cast<Const_TAC_Opd*>(opd2);
                string opd2_str = opd2->to_string();
                
                if (and_op_opd_regs.count(result_str)) {
                    // Result is an AND/OR operand, so pick opd2_reg carefully
                    if (dest_reg == "t0") {
                        opd2_reg = "t1";
                    } else if (dest_reg == "t1") {
                        opd2_reg = "t2";
                    } else if (dest_reg == "t2") {
                        opd2_reg = "t1";
                    }
                } else if (!const_opd2) {
                    // For non-constant opd2, check if it's already in a register
                    if (temp_to_register.count(opd2_str) && !temp_to_register[opd2_str].empty()) {
                        opd2_reg = temp_to_register[opd2_str];
                    } else {
                        // Will be loaded to opd2_reg - default depends on float type
                        opd2_reg = is_float_op ? "f6" : "t1";
                    }
                }
                
                // Now choose dest_reg for binary operations to avoid conflicts
                if (!and_op_opd_regs.count(result_str)) {
                    // Binary operation that's not an AND/OR operand
                    // Choose dest_reg to avoid src_register and opd2_reg
                    if (is_float_op) {
                        // Use float registers
                        if (src_register != "f2" && opd2_reg != "f2") {
                            dest_reg = "f2";
                        } else if (src_register != "f4" && opd2_reg != "f4") {
                            dest_reg = "f4";
                        } else if (src_register != "f6" && opd2_reg != "f6") {
                            dest_reg = "f6";
                        } else {
                            // Fallback
                            dest_reg = "f2";
                        }
                    } else {
                        // Use integer registers
                        if (src_register != "v0" && opd2_reg != "v0") {
                            dest_reg = "v0";
                        } else if (src_register != "t0" && opd2_reg != "t0") {
                            dest_reg = "t0";
                        } else if (src_register != "t1" && opd2_reg != "t1") {
                            dest_reg = "t1";
                        } else if (src_register != "t2" && opd2_reg != "t2") {
                            dest_reg = "t2";
                        } else {
                            // Fallback - shouldn't happen normally
                            dest_reg = "v0";
                        }
                    }
                }
                
                // Now load operand 2
                if (const_opd2) {
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
                } else {
                    // opd2 is not a constant - check if it's a temp in a register first
                    if (temp_to_register.count(opd2_str) && !temp_to_register[opd2_str].empty()) {
                        // opd2 is a temp already in a register, use that register
                        opd2_reg = temp_to_register[opd2_str];
                    } else {
                        // opd2 is a variable or temp not in a register - load it to opd2_reg
                        bool is_float = float_vars.count(opd2_str) && float_vars[opd2_str];
                        rtl_stmts.push_back(new Load_RTL_Stmt(
                            new Register_RTL_Opd(opd2_reg),
                            new Memory_RTL_Opd(opd2->to_string()),
                            is_float
                        ));
                    }
                }
            }

            // Perform the computation using the right source register
            if (opd2) {
                rtl_stmts.push_back(new Compute_RTL_Stmt(
                    new Register_RTL_Opd(dest_reg),
                    new Register_RTL_Opd(src_register),
                    rtl_op,
                    new Register_RTL_Opd(opd2_reg)
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
                bool is_float = float_vars.count(result_str) && float_vars[result_str];
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(result_str),
                    new Register_RTL_Opd(dest_reg),
                    is_float
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
                    bool is_float = float_vars.count(cond_str) && float_vars[cond_str];
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd("v0"),
                        new Memory_RTL_Opd(cond_str),
                        is_float
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
                    // For strings, use load_addr with a string label
                    string str_value = const_opd->get_string_value();
                    // Remove surrounding quotes from string value
                    if (!str_value.empty() && str_value[0] == '"' && str_value[str_value.length()-1] == '"') {
                        str_value = str_value.substr(1, str_value.length() - 2);
                    }
                    int str_index = get_string_index(str_value);
                    string str_label = "_str_" + to_string(str_index);
                    rtl_stmts.push_back(new Loadaddr_RTL_Stmt(
                        new Register_RTL_Opd("a0"),
                        new Memory_RTL_Opd(str_label),
                        str_value  // Pass unquoted string value for comment
                    ));
                } else {
                    // For variables/integers, use load
                    bool is_float = float_vars.count(opd->to_string()) && float_vars[opd->to_string()];
                    rtl_stmts.push_back(new Load_RTL_Stmt(
                        new Register_RTL_Opd("a0"),
                        new Memory_RTL_Opd(opd->to_string()),
                        is_float
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
                bool is_float = float_vars.count(opd->to_string()) && float_vars[opd->to_string()];
                rtl_stmts.push_back(new Store_RTL_Stmt(
                    new Memory_RTL_Opd(opd->to_string()),
                    new Register_RTL_Opd("v0"),
                    is_float
                ));
                temp_to_register.erase(opd->to_string());
            }
        }
    }

    return rtl_stmts;
}
