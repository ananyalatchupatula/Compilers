#include "rtl_generator_new.h"

RTL_Generator* RTL_Generator::instance = nullptr;

RTL_Generator::RTL_Generator() {
    scratch_idx = 0;
    last_compute_dest = "";
    last_stmt_was_not = false;
    scratch_regs = {
        "v0", "t1", "t2", "t3", "t4",
        "t5", "t6", "t7", "t8", "t9"
    };
}

RTL_Generator* RTL_Generator::get_instance() {
    if (instance == nullptr) {
        instance = new RTL_Generator();
    }
    return instance;
}

RTL_Generator::~RTL_Generator() {
    reset();
}

void RTL_Generator::reset() {
    for (auto stmt : rtl_stmts) delete stmt;
    rtl_stmts.clear();
    temp_register_map.clear();
}

string RTL_Generator::get_register_for_temp(string name, bool is_float) {
    if (temp_register_map.count(name)) {
        return temp_register_map[name];
    }

    vector<string> int_regs = {
    "v1",
    "t0", "t1", "t2", "t3", "t4",
    "t5", "t6", "t7", "t8", "t9",
    "s0", "s1", "s2", "s3", "s4",
    "s5", "s6", "s7"
};

    static vector<string> float_regs = {"f0", "f2", "f4", "f6"};

    string reg;
    if (is_float) {
        reg = float_regs[temp_register_map.size() % float_regs.size()];
    } else {
        reg = int_regs[temp_register_map.size() % int_regs.size()];
    }

    temp_register_map[name] = reg;
    return reg;
}
RTL_Opd* RTL_Generator::materialize_operand(TAC_Opd *opd) {
    if (!opd) return nullptr;

    bool is_float = (opd->get_data_type() == FLOAT_DATA_TYPE);

    vector<string> float_regs = {"f2", "f4", "f6", "f8"};
    string reg = is_float
        ? float_regs[scratch_idx % float_regs.size()]
    : scratch_regs[scratch_idx % scratch_regs.size()];
    scratch_idx++;

    if (auto c = dynamic_cast<Const_TAC_Opd*>(opd)) {
        rtl_stmts.push_back(
            new Compute_RTL_Stmt(
                new Register_RTL_Opd(reg),
                nullptr,
                Compute_RTL_Stmt::RTL_OP_ILOAD,
                (opd->get_data_type() == FLOAT_DATA_TYPE)
            ? (RTL_Opd*)new Const_RTL_Opd(atof(c->get_name().c_str()))
            : (RTL_Opd*)new Const_RTL_Opd(atoi(c->get_name().c_str()))
            )
        );
        return new Register_RTL_Opd(reg);
    }

    if (auto v = dynamic_cast<Var_TAC_Opd*>(opd)) {
        rtl_stmts.push_back(
            new Load_RTL_Stmt(
                new Register_RTL_Opd(reg),
                new Memory_RTL_Opd(v->get_name())
            )
        );
        return new Register_RTL_Opd(reg);
    }

    if (auto t = dynamic_cast<Temp_TAC_Opd*>(opd)) {
    string name = t->get_name();

    if (name.find("stemp") != string::npos) {
        rtl_stmts.push_back(
            new Load_RTL_Stmt(
                new Register_RTL_Opd("v0"),
                new Memory_RTL_Opd(name)
            )
        );
        return new Register_RTL_Opd("v0");
    }

    return new Register_RTL_Opd("t0");
}

    return convert_tac_operand(opd);
}
RTL_Opd* RTL_Generator::convert_tac_operand(TAC_Opd *opd) {
    if (!opd) return nullptr;

    if (auto c = dynamic_cast<Const_TAC_Opd*>(opd)) {
        if (opd->get_data_type() == FLOAT_DATA_TYPE)
            return new Const_RTL_Opd(atof(c->get_name().c_str()));
        return new Const_RTL_Opd(atoi(c->get_name().c_str()));
    }

    if (auto v = dynamic_cast<Var_TAC_Opd*>(opd)) {
        return new Memory_RTL_Opd(v->get_name());
    }

    if (auto t = dynamic_cast<Temp_TAC_Opd*>(opd)) {
    string name = t->get_name();

    if (name.find("stemp") != string::npos) {
        return new Memory_RTL_Opd(name);
    }

    // normal TAC temps should remain as their original name
    return new Register_RTL_Opd(name);
}

    return nullptr;
}

list<RTL_Stmt*> RTL_Generator::generate_rtl(list<TAC_Stmt*> &tac_stmts) {
    reset();

    for (auto stmt : tac_stmts) {
            scratch_idx = 0;
        if (auto lbl = dynamic_cast<Label_TAC_Stmt*>(stmt)) {
            rtl_stmts.push_back(
                new Label_RTL_Stmt(lbl->get_label()->get_name())
            );
        }

        else if (auto go = dynamic_cast<Goto_TAC_Stmt*>(stmt)) {
            rtl_stmts.push_back(
                new Goto_RTL_Stmt(go->get_label()->get_name())
            );
        }
else if (auto cond = dynamic_cast<Cond_Goto_TAC_Stmt*>(stmt)) {
    RTL_Opd *branch_reg;

    if (auto t = dynamic_cast<Temp_TAC_Opd*>(cond->get_cond())) {
        string name = t->get_name();

        if (name.find("stemp") != string::npos)
            branch_reg = materialize_operand(cond->get_cond());
        else
            branch_reg = new Register_RTL_Opd(last_compute_dest);
    }
    else {
        branch_reg = materialize_operand(cond->get_cond());
    }

    rtl_stmts.push_back(
        new Bgtz_RTL_Stmt(
            branch_reg,
            cond->get_label()->get_name()
        )
    );

    last_stmt_was_not = false;
}
        else if (auto comp = dynamic_cast<Compute_TAC_Stmt*>(stmt)) {
            RTL_Opd *dest;
auto tac_op = comp->get_op();

// statement temps must use fixed RTL scratch regs,
// NEVER TAC temp names like temp1/temp2
if (dynamic_cast<Temp_TAC_Opd*>(comp->get_result())) {

    if (tac_op == Compute_TAC_Stmt::TAC_OP_NOT)
        dest = new Register_RTL_Opd("v0");

    else if (
    tac_op == Compute_TAC_Stmt::TAC_OP_AND ||
    tac_op == Compute_TAC_Stmt::TAC_OP_OR
)
   dest = new Register_RTL_Opd("v0");

    else if (
    tac_op == Compute_TAC_Stmt::TAC_OP_LT ||
    tac_op == Compute_TAC_Stmt::TAC_OP_GT ||
    tac_op == Compute_TAC_Stmt::TAC_OP_LE ||
    tac_op == Compute_TAC_Stmt::TAC_OP_GE ||
    tac_op == Compute_TAC_Stmt::TAC_OP_EQ ||
    tac_op == Compute_TAC_Stmt::TAC_OP_NE
) {
    dest = new Register_RTL_Opd("t0");
}

else if (
    tac_op == Compute_TAC_Stmt::TAC_OP_PLUS ||
    tac_op == Compute_TAC_Stmt::TAC_OP_MINUS ||
    tac_op == Compute_TAC_Stmt::TAC_OP_MULT ||
    tac_op == Compute_TAC_Stmt::TAC_OP_DIV ||
    tac_op == Compute_TAC_Stmt::TAC_OP_UMINUS
) {
    if (dynamic_cast<Temp_TAC_Opd*>(comp->get_opd1()))
        dest = new Register_RTL_Opd("v0");
    else
        dest = new Register_RTL_Opd("t0");
}
}
else {
    dest = convert_tac_operand(comp->get_result());
}
RTL_Opd *src1;

if (
    tac_op == Compute_TAC_Stmt::TAC_OP_AND ||
    tac_op == Compute_TAC_Stmt::TAC_OP_OR
) {
    src1 = new Register_RTL_Opd(last_compute_dest);
}
else if (auto t = dynamic_cast<Temp_TAC_Opd*>(comp->get_opd1())) {
    if (t->get_name().find("stemp") != string::npos) {
        src1 = materialize_operand(comp->get_opd1());
    }
    else {
        if (
            tac_op == Compute_TAC_Stmt::TAC_OP_PLUS ||
            tac_op == Compute_TAC_Stmt::TAC_OP_MINUS ||
            tac_op == Compute_TAC_Stmt::TAC_OP_MULT ||
            tac_op == Compute_TAC_Stmt::TAC_OP_DIV
        )
            src1 = new Register_RTL_Opd("t0");
        else
            src1 = new Register_RTL_Opd(last_compute_dest);
    }
}

else {
    src1 = materialize_operand(comp->get_opd1());
}
        
RTL_Opd *src2 = nullptr;

if (comp->get_opd2()) {
    auto op = comp->get_op();

    bool force_t1 =
        op == Compute_TAC_Stmt::TAC_OP_MULT ||
        op == Compute_TAC_Stmt::TAC_OP_DIV  ||
        op == Compute_TAC_Stmt::TAC_OP_PLUS ||
        op == Compute_TAC_Stmt::TAC_OP_MINUS ||
        op == Compute_TAC_Stmt::TAC_OP_LT   ||
        op == Compute_TAC_Stmt::TAC_OP_GT   ||
        op == Compute_TAC_Stmt::TAC_OP_LE   ||
        op == Compute_TAC_Stmt::TAC_OP_GE   ||
        op == Compute_TAC_Stmt::TAC_OP_EQ   ||
        op == Compute_TAC_Stmt::TAC_OP_NE;

    if (force_t1) {
        int old_idx = scratch_idx;
        scratch_idx = 1;   // force register t1
        src2 = materialize_operand(comp->get_opd2());
        scratch_idx = old_idx;
    }
    else {
        src2 = materialize_operand(comp->get_opd2());
    }
}

            Compute_RTL_Stmt::RTL_Op rtl_op = Compute_RTL_Stmt::RTL_OP_ADD;
            bool is_float =
    comp->get_result()->get_data_type() == FLOAT_DATA_TYPE;
                switch (comp->get_op()) {
    case Compute_TAC_Stmt::TAC_OP_PLUS:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_ADD_D
        : Compute_RTL_Stmt::RTL_OP_ADD;
    break;

    case Compute_TAC_Stmt::TAC_OP_MINUS:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_SUB_D
        : Compute_RTL_Stmt::RTL_OP_SUB;
    break;

    case Compute_TAC_Stmt::TAC_OP_MULT:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_MUL_D
        : Compute_RTL_Stmt::RTL_OP_MUL;
    break;

    case Compute_TAC_Stmt::TAC_OP_DIV:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_DIV_D
        : Compute_RTL_Stmt::RTL_OP_DIV;
    break;

    case Compute_TAC_Stmt::TAC_OP_LT:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_SLT_D
        : Compute_RTL_Stmt::RTL_OP_SLT;
    break;

    case Compute_TAC_Stmt::TAC_OP_GT:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_SLE_D
        : Compute_RTL_Stmt::RTL_OP_SGT;
    break;

case Compute_TAC_Stmt::TAC_OP_LE:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_SLE_D
        : Compute_RTL_Stmt::RTL_OP_SLE;
    break;

    case Compute_TAC_Stmt::TAC_OP_GE:
    rtl_op = is_float
        ? Compute_RTL_Stmt::RTL_OP_SLT_D
        : Compute_RTL_Stmt::RTL_OP_SGE;
    break;

    case Compute_TAC_Stmt::TAC_OP_EQ:
        rtl_op = Compute_RTL_Stmt::RTL_OP_SEQ; break;

    case Compute_TAC_Stmt::TAC_OP_NE:
        rtl_op = Compute_RTL_Stmt::RTL_OP_SNE; break;

    case Compute_TAC_Stmt::TAC_OP_AND:
        rtl_op = Compute_RTL_Stmt::RTL_OP_AND; break;

    case Compute_TAC_Stmt::TAC_OP_OR:
        rtl_op = Compute_RTL_Stmt::RTL_OP_OR; break;

    case Compute_TAC_Stmt::TAC_OP_UMINUS:
        rtl_op = Compute_RTL_Stmt::RTL_OP_UMINUS; break;

    case Compute_TAC_Stmt::TAC_OP_NOT:
        rtl_op = Compute_RTL_Stmt::RTL_OP_NOT; break;

    default:
        rtl_op = Compute_RTL_Stmt::RTL_OP_ADD;
}

            rtl_stmts.push_back(
                new Compute_RTL_Stmt(dest, src1, rtl_op, src2)
            );
            if (auto reg_dest = dynamic_cast<Register_RTL_Opd*>(dest))
    last_compute_dest = reg_dest->get_name();
            last_stmt_was_not = (tac_op == Compute_TAC_Stmt::TAC_OP_NOT);
        }
        else if (auto read_stmt = dynamic_cast<Read_TAC_Stmt*>(stmt)) {
    rtl_stmts.push_back(
        new Compute_RTL_Stmt(
            new Register_RTL_Opd("v0"),
            nullptr,
            Compute_RTL_Stmt::RTL_OP_ILOAD,
            new Const_RTL_Opd(5)
        )
    
    );

    rtl_stmts.push_back(new Read_RTL_Stmt());

    rtl_stmts.push_back(
        new Store_RTL_Stmt(
            convert_tac_operand(read_stmt->get_var()),
            new Register_RTL_Opd("v0")
        )
    );
}
        else if (auto write_stmt = dynamic_cast<Print_TAC_Stmt*>(stmt)) {
    rtl_stmts.push_back(
        new Compute_RTL_Stmt(
            new Register_RTL_Opd("v0"),
            nullptr,
            Compute_RTL_Stmt::RTL_OP_ILOAD,
            new Const_RTL_Opd(1)
        )
      
    );

    rtl_stmts.push_back(
        new Load_RTL_Stmt(
            new Register_RTL_Opd("a0"),
            convert_tac_operand(write_stmt->get_opd())
        )
    );

    rtl_stmts.push_back(new Write_RTL_Stmt());
}

else if (auto assign = dynamic_cast<Assign_TAC_Stmt*>(stmt)) {
    RTL_Opd *rhs;

    if (auto t = dynamic_cast<Temp_TAC_Opd*>(assign->get_opd1())) {
        if (t->get_name().find("stemp") != string::npos)
            rhs = materialize_operand(assign->get_opd1());
        else
            rhs = new Register_RTL_Opd(last_compute_dest);
    }
    else {
        rhs = materialize_operand(assign->get_opd1());
    }

    rtl_stmts.push_back(
        new Store_RTL_Stmt(
            convert_tac_operand(assign->get_result()),
            rhs
        )
    );
}
    }

    return rtl_stmts;
}

void RTL_Generator::print_rtl(FILE *file) {
    fprintf(file, "**PROCEDURE: main\n");
    fprintf(file, "**BEGIN: RTL Statements\n");

    for (auto stmt : rtl_stmts) {
        stmt->print(file);
    }

    fprintf(file, "**END: RTL Statements\n");
}