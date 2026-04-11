#include "rtl_generator.h"
#include <iostream>
#include <algorithm>
#include <vector>



RTL_Generator::RTL_Generator() {
    free_pool = {"v0", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};
}

RTL_Generator* RTL_Generator::instance = NULL;

void RTL_Generator::reset() {
    rtl_list.clear();
    active_temp_map.clear();
    free_pool = {"v0", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};
    float_pool = {"f2", "f4", "f6", "f8", "f7"};
}

void RTL_Generator::reset_block_allocator() {
    active_temp_map.clear();
    free_pool = {"v0","t0","t1","t2","t3","t4","t5","t6","t7","t8","t9"};
    float_pool = {"f2","f4","f6","f8","f7"};
}

string RTL_Generator::allocate_float_reg() {
    if (float_pool.empty()) {
        cerr << "Float register pool exhausted\n";
        exit(1);
    }

    string reg = float_pool.front();
    float_pool.erase(float_pool.begin());
    return reg;
}

void RTL_Generator::free_float_reg(const string &reg) {
    float_pool.push_back(reg);

    sort(float_pool.begin(), float_pool.end(), [](string a, string b) {
        return stoi(a.substr(1)) < stoi(b.substr(1));
    });
}

void RTL_Generator::free_any_reg(const string &reg) {
    if (reg.empty()) return;

    if (reg[0] == 'f')
        free_float_reg(reg);
    else
        free_reg(reg);
}

RTL_Generator* RTL_Generator::get_instance() {
    if (instance == NULL)
        instance = new RTL_Generator();

    return instance;
}

Compute_RTL_Stmt::RTL_Op
RTL_Generator::map_tac_to_rtl_op(Compute_TAC_Stmt::TAC_Op op) {
    switch (op) {
        case Compute_TAC_Stmt::TAC_OP_PLUS:
            return Compute_RTL_Stmt::RTL_OP_ADD;
        case Compute_TAC_Stmt::TAC_OP_MINUS:
            return Compute_RTL_Stmt::RTL_OP_SUB;
        case Compute_TAC_Stmt::TAC_OP_MULT:
            return Compute_RTL_Stmt::RTL_OP_MUL;
        case Compute_TAC_Stmt::TAC_OP_DIV:
            return Compute_RTL_Stmt::RTL_OP_DIV;
        case Compute_TAC_Stmt::TAC_OP_LT:
            return Compute_RTL_Stmt::RTL_OP_SLT;
        case Compute_TAC_Stmt::TAC_OP_LE:
            return Compute_RTL_Stmt::RTL_OP_SLE;
        case Compute_TAC_Stmt::TAC_OP_GT:
            return Compute_RTL_Stmt::RTL_OP_SGT;
        case Compute_TAC_Stmt::TAC_OP_GE:
            return Compute_RTL_Stmt::RTL_OP_SGE;
        case Compute_TAC_Stmt::TAC_OP_EQ:
            return Compute_RTL_Stmt::RTL_OP_SEQ;
        case Compute_TAC_Stmt::TAC_OP_NE:
            return Compute_RTL_Stmt::RTL_OP_SNE;
        case Compute_TAC_Stmt::TAC_OP_AND:
            return Compute_RTL_Stmt::RTL_OP_AND;
        case Compute_TAC_Stmt::TAC_OP_OR:
            return Compute_RTL_Stmt::RTL_OP_OR;
        case Compute_TAC_Stmt::TAC_OP_NOT:
            return Compute_RTL_Stmt::RTL_OP_NOT;
        case Compute_TAC_Stmt::TAC_OP_UMINUS:
            return Compute_RTL_Stmt::RTL_OP_UMINUS;
    }

    return Compute_RTL_Stmt::RTL_OP_ADD;
}


string RTL_Generator::allocate_reg() {
    if (free_pool.empty()) {
        cerr << "Register pool exhausted\n";
        exit(1);
    }

    string reg = free_pool.front();
    free_pool.erase(free_pool.begin());
    return reg;
}

void RTL_Generator::free_reg(const string &reg) {
    free_pool.push_back(reg);

    sort(free_pool.begin(), free_pool.end(), [](string a, string b) {
        auto rank = [](string r) {
            if (r == "v0") return -1;
            return stoi(r.substr(1));
        };
        return rank(a) < rank(b);
    });
}

RTL_Opd* RTL_Generator::materialize_operand(
    TAC_Opd *opd,
    string &used_reg,
    bool &is_temp
) {
    is_temp = false;
    used_reg = "";

    // active temp reuse - ONLY reuse for temps, not for loaded values
    if (dynamic_cast<Temp_TAC_Opd*>(opd)) {
        string name = opd->to_string();
        if (active_temp_map.count(name)) {
            // For temp reuse: return the register WITHOUT generating a load
            used_reg = active_temp_map[name];
            is_temp = true;
            
            // IMPORTANT: This register should NOT be in free_pool if it's in active_temp_map
            // If it somehow is, remove it to prevent double allocation
            if (used_reg.find('f') != 0) {  // integer register
                auto it = find(free_pool.begin(), free_pool.end(), used_reg);
                if (it != free_pool.end()) {
                    free_pool.erase(it);
                }
            } else {  // float register
                auto it = find(float_pool.begin(), float_pool.end(), used_reg);
                if (it != float_pool.end()) {
                    float_pool.erase(it);
                }
            }
            
            return new Register_RTL_Opd(used_reg);
        }
    }

bool is_float =
    (opd->get_data_type() == FLOAT_DATA_TYPE);

used_reg = is_float ? allocate_float_reg() : allocate_reg();

RTL_Opd *reg = new Register_RTL_Opd(used_reg);

if (dynamic_cast<Const_TAC_Opd*>(opd)) {
    auto c = dynamic_cast<Const_TAC_Opd*>(opd);

    if (is_float) {
    rtl_list.push_back(
        new Load_RTL_Stmt(
            reg,
            new Const_RTL_Opd(c->get_float_value()),
            true
        )
    );
}else {
        rtl_list.push_back(
            new Load_RTL_Stmt(
                reg,
                new Const_RTL_Opd(c->get_int_value())
            )
        );
    }
}
else {
    // For string literals (_str_0, _str_1, etc.), use load_addr to get the address
    if (opd->to_string().find("_str_") == 0) {
        rtl_list.push_back(
            new Loadaddr_RTL_Stmt(
                reg,
                new Memory_RTL_Opd(opd->to_string())
            )
        );
    } else {
        rtl_list.push_back(
            new Load_RTL_Stmt(
                reg,
                new Memory_RTL_Opd(opd->to_string()),
                is_float
            )
        );
    }
}

    return reg;
}

list<RTL_Stmt*> RTL_Generator::generate_rtl(list<TAC_Stmt*>& tac_stmts) {
    reset();
    for (auto stmt : tac_stmts) {

if (auto comp = dynamic_cast<Compute_TAC_Stmt*>(stmt)) {
    string left_reg, right_reg, out_reg;
    bool left_temp = false, right_temp = false;

    RTL_Opd *lhs = materialize_operand(comp->get_opd1(), left_reg, left_temp);

bool operands_float =
    comp->get_opd1()->get_data_type() == FLOAT_DATA_TYPE;

bool is_compare =
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_LT ||
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_LE ||
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_GT ||
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_GE ||
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_EQ ||
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_NE;

bool use_float_dest = operands_float && !is_compare;

// ALLOCATE DESTINATION REGISTER BEFORE MATERIALIZING RIGHT OPERAND
// This ensures the destination register comes from the pool AFTER left operand is removed
if (operands_float && is_compare) {
    // For floating-point comparisons, the result is a BOOLEAN (integer), not float
    // Allocate an integer register for the final materialized result
    out_reg = allocate_reg();
    // Skip v0 if it was allocated (v0 is used as temp for comparison flag)
    if (out_reg == "v0") {
        out_reg = allocate_reg();
    }
}
else {
    out_reg = use_float_dest ? allocate_float_reg() : allocate_reg();
}
    // Create dest operand only for non-float-comparisons (float comparisons create their own temp)
    RTL_Opd *dest = nullptr;
    if (!(operands_float && is_compare)) {
        dest = new Register_RTL_Opd(out_reg);
    }

    // NOW materialize right operand AFTER destination is allocated
    RTL_Opd *rhs = nullptr;
    if (comp->get_opd2())
        rhs = materialize_operand(comp->get_opd2(), right_reg, right_temp);

    if (dynamic_cast<Temp_TAC_Opd*>(comp->get_result())) {
    active_temp_map[comp->get_result()->to_string()] = out_reg;
}

    Compute_RTL_Stmt::RTL_Op float_cmp_op;

switch (comp->get_op()) {
    case Compute_TAC_Stmt::TAC_OP_GT:
        float_cmp_op = Compute_RTL_Stmt::RTL_OP_SLE_D;
        break;
    case Compute_TAC_Stmt::TAC_OP_GE:
        float_cmp_op = Compute_RTL_Stmt::RTL_OP_SLT_D;
        break;
    case Compute_TAC_Stmt::TAC_OP_LT:
        float_cmp_op = Compute_RTL_Stmt::RTL_OP_SLT_D;
        break;
    case Compute_TAC_Stmt::TAC_OP_LE:
        float_cmp_op = Compute_RTL_Stmt::RTL_OP_SLE_D;
        break;
    case Compute_TAC_Stmt::TAC_OP_EQ:
        float_cmp_op = Compute_RTL_Stmt::RTL_OP_SEQ;
        break;
    case Compute_TAC_Stmt::TAC_OP_NE:
        float_cmp_op = Compute_RTL_Stmt::RTL_OP_SEQ;
        break;
    default:
        float_cmp_op = map_tac_to_rtl_op(comp->get_op());
}
    
    if (operands_float && is_compare) {
    // For floating-point comparisons, we need TWO registers:
    // 1. A temporary float register for the seq.d result (always use f7)
    // 2. The final integer register for the boolean result
    string temp_float_reg = "f7";  // Explicitly use f7 for comparison temporaries, not from pool
    RTL_Opd *temp_float_dest = new Register_RTL_Opd(temp_float_reg);
    
    // emit compare directly on float operands, storing result in temp float register
    rtl_list.push_back(
        new Compute_RTL_Stmt(
            temp_float_dest,    // Temporary float register for seq.d result (f7)
            lhs,                // Left operand (f2)
            float_cmp_op,
            rhs                 // Right operand (f4)
        )
    );
    
    // Note: f7 is not freed because it's a temporary for the comparison, not a general-purpose register
    // The boolean result is materialized into an integer register (see below)

    // materialize boolean result into integer register
    // Use v0 (reserved for syscalls/returns) for float compare results
    // This is MIPS architecture-specific, not user-allocated
    string cmp_reg = "v0";
    rtl_list.push_back(
        new Load_RTL_Stmt(
            new Register_RTL_Opd(cmp_reg),
            new Const_RTL_Opd(1)
        )
    );

   rtl_list.push_back(
    new Compute_RTL_Stmt(
        new Register_RTL_Opd(out_reg),
        new Register_RTL_Opd("zero"),
        Compute_RTL_Stmt::RTL_OP_MOVE,  // Integer MOVE, not float move.d
        nullptr
    )
);

bool use_movt =
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_LT ||
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_LE ||
    comp->get_op() == Compute_TAC_Stmt::TAC_OP_EQ;

rtl_list.push_back(
    new Compute_RTL_Stmt(
        new Register_RTL_Opd(out_reg),
        new Register_RTL_Opd(cmp_reg),
        use_movt
            ? Compute_RTL_Stmt::RTL_OP_MOVT
            : Compute_RTL_Stmt::RTL_OP_MOVF,
        nullptr
    )
);

// Free all registers after using them for comparison
// v0 is reserved for syscalls but still part of the pool when not in use
free_any_reg(cmp_reg);

// Free left input register
if (left_reg != out_reg) {
    if (left_temp) {
        active_temp_map.erase(comp->get_opd1()->to_string());
    }
    free_any_reg(left_reg);
}

// Free right input register
if (rhs && right_reg != out_reg) {
    if (right_temp) {
        active_temp_map.erase(comp->get_opd2()->to_string());
    }
    free_any_reg(right_reg);
}
}
else {
    rtl_list.push_back(
        new Compute_RTL_Stmt(
            dest,
            lhs,
            map_tac_to_rtl_op(comp->get_op()),
            rhs
        )
    );

    // free LEFT input register after use
    if (left_reg != out_reg) {
        if (left_temp) {
            active_temp_map.erase(comp->get_opd1()->to_string());
        }
        free_any_reg(left_reg);
    }

    // free RIGHT input register after use
    if (comp->get_opd2() && right_reg != out_reg) {
        if (right_temp) {
            active_temp_map.erase(comp->get_opd2()->to_string());
        }
        free_any_reg(right_reg);
    }
}
  if (dynamic_cast<Var_TAC_Opd*>(comp->get_result())) {
    bool is_float_result =
        comp->get_result()->get_data_type() == FLOAT_DATA_TYPE;

    rtl_list.push_back(
        new Store_RTL_Stmt(
            new Memory_RTL_Opd(comp->get_result()->to_string()),
            new Register_RTL_Opd(out_reg),
            is_float_result
        )
    );

    free_any_reg(out_reg);
}
}

else if (auto asg = dynamic_cast<Assign_TAC_Stmt*>(stmt)) {
    string src_reg;
    bool src_temp;

    RTL_Opd *src =
        materialize_operand(asg->get_opd1(), src_reg, src_temp);
        bool is_float =
    asg->get_result()->get_data_type() == FLOAT_DATA_TYPE;

    rtl_list.push_back(
        new Store_RTL_Stmt(
    new Memory_RTL_Opd(asg->get_result()->to_string()),
    src,
    is_float
)
    );

    if (src_temp) {
        active_temp_map.erase(asg->get_opd1()->to_string());
    }

    free_any_reg(src_reg);
}

        else if (auto label = dynamic_cast<Label_TAC_Stmt*>(stmt)) {
                    rtl_list.push_back(
            new Label_RTL_Stmt(label->get_label()->to_string())
        );
        
        }

        else if (auto go = dynamic_cast<Goto_TAC_Stmt*>(stmt)) {
            rtl_list.push_back(
                new Goto_RTL_Stmt(go->get_label()->to_string())
            );
            
        }
        
        else if (auto cgoto = dynamic_cast<Cond_Goto_TAC_Stmt*>(stmt)) {
    string cond_reg;
    bool cond_temp;

    RTL_Opd *cond =
        materialize_operand(cgoto->get_cond(), cond_reg, cond_temp);

    rtl_list.push_back(
        new Bgtz_RTL_Stmt(
            cond,
            cgoto->get_label()->to_string()
        )
    );

    // cleanup current block FIRST
    if (cond_temp) {
        active_temp_map.erase(cgoto->get_cond()->to_string());
    }

    free_any_reg(cond_reg);

    // THEN reset for next basic block
    
}


else if (auto p = dynamic_cast<Print_TAC_Stmt*>(stmt)) {
    string opd_name = p->get_opd()->to_string();

    // ✅ STRING LITERAL PRINT (_str_0, _str_1, etc.)
    if (opd_name.find("_str_") == 0) {
        rtl_list.push_back(
            new Load_RTL_Stmt(
                new Register_RTL_Opd("v0"),
                new Const_RTL_Opd(4)
            )
        );

        rtl_list.push_back(
            new Loadaddr_RTL_Stmt(
                new Register_RTL_Opd("a0"),
                new Memory_RTL_Opd(opd_name)
            )
        );

        rtl_list.push_back(new Write_RTL_Stmt());
    }

    // ✅ STRING VARIABLE PRINT (variable holding string address)
    else if (p->get_opd()->get_data_type() == STRING_DATA_TYPE) {
        rtl_list.push_back(
            new Load_RTL_Stmt(
                new Register_RTL_Opd("v0"),
                new Const_RTL_Opd(4)
            )
        );

        rtl_list.push_back(
            new Load_RTL_Stmt(
                new Register_RTL_Opd("a0"),
                new Memory_RTL_Opd(opd_name)
            )
        );

        rtl_list.push_back(new Write_RTL_Stmt());
    }

    // ✅ INTEGER / FLOAT / VARIABLE PRINT
    else {
    bool is_float =
        p->get_opd()->get_data_type() == FLOAT_DATA_TYPE;
    bool is_const = dynamic_cast<Const_TAC_Opd*>(p->get_opd()) != nullptr;

    rtl_list.push_back(
        new Load_RTL_Stmt(
            new Register_RTL_Opd("v0"),
            new Const_RTL_Opd(is_float ? 3 : 1)
        )
    );

    // Handle constants differently - use immediate load, not memory load
    if (is_const) {
        auto const_opd = dynamic_cast<Const_TAC_Opd*>(p->get_opd());
        if (is_float) {
            rtl_list.push_back(
                new Load_RTL_Stmt(
                    new Register_RTL_Opd("f12"),
                    new Const_RTL_Opd(const_opd->get_float_value()),
                    true
                )
            );
        } else {
            rtl_list.push_back(
                new Load_RTL_Stmt(
                    new Register_RTL_Opd("a0"),
                    new Const_RTL_Opd(const_opd->get_int_value())
                )
            );
        }
    } else {
                if (active_temp_map.count(opd_name)) {
            rtl_list.push_back(
                new Compute_RTL_Stmt(
                    new Register_RTL_Opd(is_float ? "f12" : "a0"),
                    new Register_RTL_Opd(active_temp_map[opd_name]),
                    Compute_RTL_Stmt::RTL_OP_MOVE,
                    nullptr
                )
            );
            active_temp_map.erase(opd_name);
        }
        else {
            rtl_list.push_back(
                new Load_RTL_Stmt(
                    new Register_RTL_Opd(is_float ? "f12" : "a0"),
                    new Memory_RTL_Opd(opd_name)
                )
            );
        }
    }

    rtl_list.push_back(new Write_RTL_Stmt());
}
}

else if (auto r = dynamic_cast<Read_TAC_Stmt*>(stmt)) {
    // syscall code 5 for integer read, 7 for double read
    bool is_float = r->get_var()->get_data_type() == FLOAT_DATA_TYPE;
    
    rtl_list.push_back(
        new Load_RTL_Stmt(
            new Register_RTL_Opd("v0"),
            new Const_RTL_Opd(is_float ? 7 : 5)
        )
    );

    rtl_list.push_back(
        new Read_RTL_Stmt()
    );
    
    rtl_list.push_back(
        new Store_RTL_Stmt(
            new Memory_RTL_Opd(r->get_var()->to_string()),
            new Register_RTL_Opd(is_float ? "f0" : "v0"),
            is_float
        )
    );
}

else if (auto call = dynamic_cast<Call_TAC_Stmt*>(stmt)) {
    // Function call without assignment
    string func_name = call->get_func()->to_string();
    rtl_list.push_back(
        new Call_RTL_Stmt(func_name)
    );
}

else if (auto acall = dynamic_cast<AssignCall_TAC_Stmt*>(stmt)) {
    // Function call with assignment: temp = f_(args)
    string func_name = acall->get_func_name()->to_string();
    bool result_float = acall->get_lhs()->get_data_type() == FLOAT_DATA_TYPE;
    
    // Push arguments in REVERSE order (right-to-left for stack convention)
    vector<TAC_Opd*> args(acall->get_arguments().begin(), acall->get_arguments().end());
    for (int i = args.size() - 1; i >= 0; i--) {
        auto arg = args[i];
        bool arg_float = arg->get_data_type() == FLOAT_DATA_TYPE;
        string arg_reg;
        bool arg_is_temp;
        
        // Use materialize_operand to handle temps from registers or load other operands
        RTL_Opd *arg_opd = materialize_operand(arg, arg_reg, arg_is_temp);
        
        if (arg_opd) {
            // Push to stack (arg_opd could be a register from active_temp_map or a freshly loaded value)
            rtl_list.push_back(
                new Push_RTL_Stmt(arg_opd)
            );
            
            // Don't erase yet - temp might be used again, free will happen naturally
            if (!arg_is_temp) {
                free_any_reg(arg_reg);
            }
        }
    }
    
    // Call function with result assignment
    string result_reg = result_float ? "f0" : "v1";
    rtl_list.push_back(
        new CallAssign_RTL_Stmt(new Register_RTL_Opd(result_reg), func_name)
    );
    
    // Pop arguments
    for (size_t i = 0; i < acall->get_arguments().size(); i++) {
        rtl_list.push_back(
            new Pop_RTL_Stmt()
        );
    }
    
    // Add move statement: move v0 <- v1 (or move.d f2 <- f0 for float)
    string dest_reg = result_float ? "f2" : "v0";
    rtl_list.push_back(
        new Move_RTL_Stmt(
            new Register_RTL_Opd(dest_reg),
            new Register_RTL_Opd(result_reg),
            result_float
        )
    );
    
    // Track result in temp map
    if (dynamic_cast<Temp_TAC_Opd*>(acall->get_lhs())) {
        active_temp_map[acall->get_lhs()->to_string()] = dest_reg;
    }
    
    // After function call, reset register allocator
    // All t0-t9 registers may have been clobbered by the called function
    // Only v0 is guaranteed to be safe (holds the return value)
    active_temp_map.clear();
    free_pool = {"v0", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};
    
    // Re-add the result to active map after reset
    if (dynamic_cast<Temp_TAC_Opd*>(acall->get_lhs())) {
        active_temp_map[acall->get_lhs()->to_string()] = dest_reg;
    }
}

else if (auto ret = dynamic_cast<Return_TAC_Stmt*>(stmt)) {
    // Return statement - load return value directly to v1/f0
    if (ret->get_return_val()) {
        bool is_float = ret->get_return_val()->get_data_type() == FLOAT_DATA_TYPE;
        string return_reg = is_float ? "f0" : "v1";
        
        // Create RTL operand for the return value
        RTL_Opd *rtl_opd = NULL;
        string ret_str = ret->get_return_val()->to_string();
        
        if (dynamic_cast<Temp_TAC_Opd*>(ret->get_return_val())) {
            // For temps, load directly from memory (stemp0, etc.)
            rtl_opd = new Memory_RTL_Opd(ret_str);
        } else if (dynamic_cast<Var_TAC_Opd*>(ret->get_return_val())) {
            rtl_opd = new Memory_RTL_Opd(ret_str);
        } else if (dynamic_cast<Const_TAC_Opd*>(ret->get_return_val())) {
            if (is_float) {
                rtl_opd = new Const_RTL_Opd(atoi(ret_str.c_str()));  // For now, treat as int
            } else {
                rtl_opd = new Const_RTL_Opd(atoi(ret_str.c_str()));
            }
        }
        
        if (rtl_opd) {
            rtl_list.push_back(
                new Load_RTL_Stmt(
                    new Register_RTL_Opd(return_reg),
                    rtl_opd,
                    is_float
                )
            );
        }
        
        rtl_list.push_back(
            new Return_RTL_Stmt(new Register_RTL_Opd(return_reg))
        );
    } else {
        // Void return
        rtl_list.push_back(
            new Return_RTL_Stmt()
        );
    }
}


    }

    return rtl_list;
}

void RTL_Generator::print_rtl(FILE *file) {
    for (auto stmt : rtl_list)
        stmt->print(file);
}