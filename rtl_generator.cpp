#include "rtl_generator.h"
#include <iostream>
#include <algorithm>



RTL_Generator::RTL_Generator() {
    free_pool = {"v0", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};
}

RTL_Generator* RTL_Generator::instance = NULL;

void RTL_Generator::reset() {
    rtl_list.clear();
    active_temp_map.clear();
    free_pool = {"v0", "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7", "t8", "t9"};
}

void RTL_Generator::reset_block_allocator() {
    active_temp_map.clear();
    free_pool = {"v0","t0","t1","t2","t3","t4","t5","t6","t7","t8","t9"};
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

    // active temp reuse
    if (dynamic_cast<Temp_TAC_Opd*>(opd)) {
        string name = opd->to_string();
        if (active_temp_map.count(name)) {
            used_reg = active_temp_map[name];
            is_temp = true;
            return new Register_RTL_Opd(used_reg);
        }
    }

    // fresh allocation
    used_reg = allocate_reg();

    RTL_Opd *reg = new Register_RTL_Opd(used_reg);

    if (dynamic_cast<Const_TAC_Opd*>(opd)) {
        auto c = dynamic_cast<Const_TAC_Opd*>(opd);
        rtl_list.push_back(
            new Load_RTL_Stmt(reg, new Const_RTL_Opd(c->get_int_value()))
        );
    }
    else {
        rtl_list.push_back(
            new Load_RTL_Stmt(reg, new Memory_RTL_Opd(opd->to_string()))
        );
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

    out_reg = allocate_reg();
    RTL_Opd *dest = new Register_RTL_Opd(out_reg);

    RTL_Opd *rhs = nullptr;
    if (comp->get_opd2())
        rhs = materialize_operand(comp->get_opd2(), right_reg, right_temp);

    
    if (dynamic_cast<Temp_TAC_Opd*>(comp->get_result())) {
    active_temp_map[comp->get_result()->to_string()] = out_reg;
}

    rtl_list.push_back(
        new Compute_RTL_Stmt(
            dest,
            lhs,
            map_tac_to_rtl_op(comp->get_op()),
            rhs
        )
    );
    if (dynamic_cast<Var_TAC_Opd*>(comp->get_result())) {
    rtl_list.push_back(
        new Store_RTL_Stmt(
            new Memory_RTL_Opd(comp->get_result()->to_string()),
            new Register_RTL_Opd(out_reg)
        )
    );

    free_reg(out_reg);
}

    // use-and-lose input temps
// free LEFT input register after use
if (left_reg != out_reg) {
    if (left_temp) {
        active_temp_map.erase(comp->get_opd1()->to_string());
    }
    free_reg(left_reg);
}

// free RIGHT input register after use
if (comp->get_opd2() && right_reg != out_reg) {
    if (right_temp) {
        active_temp_map.erase(comp->get_opd2()->to_string());
    }
    free_reg(right_reg);
}
}

else if (auto asg = dynamic_cast<Assign_TAC_Stmt*>(stmt)) {
    string src_reg;
    bool src_temp;

    RTL_Opd *src =
        materialize_operand(asg->get_opd1(), src_reg, src_temp);

    rtl_list.push_back(
        new Store_RTL_Stmt(
            new Memory_RTL_Opd(asg->get_result()->to_string()),
            src
        )
    );

    if (src_temp) {
        active_temp_map.erase(asg->get_opd1()->to_string());
    }

    free_reg(src_reg);
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

    free_reg(cond_reg);

    // THEN reset for next basic block
    
}


else if (auto p = dynamic_cast<Print_TAC_Stmt*>(stmt)) {
    string opd_name = p->get_opd()->to_string();

    // ✅ STRING PRINT
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

    // ✅ INTEGER / VARIABLE PRINT
    else {
        rtl_list.push_back(
            new Load_RTL_Stmt(
                new Register_RTL_Opd("v0"),
                new Const_RTL_Opd(1)
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
}

else if (auto r = dynamic_cast<Read_TAC_Stmt*>(stmt)) {

    // syscall code 5 for integer read
    rtl_list.push_back(
        new Load_RTL_Stmt(
            new Register_RTL_Opd("v0"),
            new Const_RTL_Opd(5)
        )
    );

    rtl_list.push_back(
        new Read_RTL_Stmt()
    );
    
    rtl_list.push_back(
        new Store_RTL_Stmt(
            new Memory_RTL_Opd(r->get_var()->to_string()),
            new Register_RTL_Opd("v0")
        )
    );
}

    }

    return rtl_list;
}

void RTL_Generator::print_rtl(FILE *file) {
    for (auto stmt : rtl_list)
        stmt->print(file);
}