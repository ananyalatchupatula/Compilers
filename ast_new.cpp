#include "ast_new.h"
#include "tac_str.h"
#include "tac_generator.h"
#include "rtl_generator.h"
#include <cstdio>
#include <iostream>

using std::cout;
using std::endl;

extern FILE *ast_file;
extern FILE *tac_file;
extern FILE *rtl_file;
extern int show_ast;
extern int show_tac;
extern int show_rtl;

extern list<Statement_Ast*> main_stmt_list;

/* ============================================================================
   BASE AST CLASS IMPLEMENTATION
   ============================================================================ */

Ast::Ast() : node_data_type(VOID_DATA_TYPE), temp_id(-1), stemp_id(-1), not_temp_id(-1) {}

Ast::~Ast() {}

void Ast::set_data_type(DataType d) {
    node_data_type = d;
}

DataType Ast::get_data_type() {
    return node_data_type;
}

/* ============================================================================
   EXPRESSION AST IMPLEMENTATION
   ============================================================================ */

Expression_Ast::Expression_Ast() : Ast() {}
Expression_Ast::~Expression_Ast() {}

/* --- CONSTANT EXPRESSION --- */

Const_Expr_Ast::Const_Expr_Ast(string val, DataType type) : value(val) {
    set_data_type(type);
}

Const_Expr_Ast::~Const_Expr_Ast() {}

string Const_Expr_Ast::get_value() {
    return value;
}

void Const_Expr_Ast::print(int indent) {
    if(node_data_type == FLOAT_DATA_TYPE) {
        char buf[50];
        snprintf(buf, sizeof(buf), "%.2f", atof(value.c_str()));
        fprintf(ast_file, "Num : %s<%s>", buf, data_type_to_string(node_data_type));
    } else if(node_data_type == STRING_DATA_TYPE) {
        fprintf(ast_file, "String : %s<%s>", value.c_str(), data_type_to_string(node_data_type));
    } else {
        fprintf(ast_file, "Num : %s<%s>", value.c_str(), data_type_to_string(node_data_type));
    }
}

TAC_Opd* Const_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    if(node_data_type == INT_DATA_TYPE) {
        return new Const_TAC_Opd(atoi(value.c_str()));
    } else if(node_data_type == FLOAT_DATA_TYPE) {
    return new Const_TAC_Opd(atof(value.c_str()));
}else if(node_data_type == STRING_DATA_TYPE) {
    string label = TAC_Generator::get_instance()->get_or_create_string_label(value);
    return new Var_TAC_Opd(label, STRING_DATA_TYPE);
    }else {
        return new Const_TAC_Opd(0);
    }
}

/* --- NAME EXPRESSION --- */

Name_Expr_Ast::Name_Expr_Ast(string var_name) : name(var_name) {}

Name_Expr_Ast::~Name_Expr_Ast() {}

string Name_Expr_Ast::get_name() {
    return name;
}

void Name_Expr_Ast::print(int indent) {
    fprintf(ast_file, "Name : %s<%s>", name.c_str(), data_type_to_string(node_data_type));
}

TAC_Opd* Name_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    return new Var_TAC_Opd(name, node_data_type);
}

/* --- UNARY EXPRESSION --- */

Unary_Expr_Ast::Unary_Expr_Ast(UnaryOp operation, Expression_Ast* opd)
    : op(operation), operand(opd) {}

Unary_Expr_Ast::~Unary_Expr_Ast() {
    delete operand;
}

void Unary_Expr_Ast::print(int indent) {
    if(op == NOT_OP) {
        fprintf(ast_file, "Condition: NOT<bool>\n");
    } else {
        fprintf(ast_file, "Arith: %s<%s>\n", "Uminus", data_type_to_string(node_data_type));
    }
    for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "L_Opd (");
    
    // Check if operand is complex
    Binary_Expr_Ast* opd_bin = dynamic_cast<Binary_Expr_Ast*>(operand);
    Ternary_Expr_Ast* opd_tern = dynamic_cast<Ternary_Expr_Ast*>(operand);
    Unary_Expr_Ast* opd_unary = dynamic_cast<Unary_Expr_Ast*>(operand);
    if(opd_bin || opd_tern || opd_unary) {
        fprintf(ast_file, "\n");
        for(int i = 0; i < indent + 3; i++) fprintf(ast_file, " ");
    }
    operand->print(indent+2);
    fprintf(ast_file, ")");
}

void Unary_Expr_Ast::pre_allocate_temps() {
    // Pre-allocate temps in operand first
    operand->pre_allocate_temps();
    // Then allocate for this unary operation
    if (temp_id == -1) {
        temp_id = TAC_Generator::get_instance()->get_temp_counter();
        TAC_Generator::get_instance()->create_new_temp(
    node_data_type == FLOAT_DATA_TYPE
);
    }
}

TAC_Opd* Unary_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    TAC_Opd* opd_tac = operand->generate_tac(statements);
    
    // Use pre-allocated temp ID if available, otherwise create new
    TAC_Opd* result;
    if (temp_id != -1) {
        result = new Temp_TAC_Opd(temp_id, node_data_type);
    } else {
        result = TAC_Generator::get_instance()->create_new_temp();
    }
    
    if(op == NOT_OP) {
        statements.push_back(new Compute_TAC_Stmt(result, opd_tac, Compute_TAC_Stmt::TAC_OP_NOT, NULL));
    } else if(op == UMINUS_OP) {
        statements.push_back(new Compute_TAC_Stmt(result, opd_tac, Compute_TAC_Stmt::TAC_OP_UMINUS, NULL));
    }
    
    return result;
}

/* --- BINARY EXPRESSION --- */

Binary_Expr_Ast::Binary_Expr_Ast(Expression_Ast* l, BinaryOp operation, Expression_Ast* r)
    : left(l), op(operation), right(r) {}

Binary_Expr_Ast::~Binary_Expr_Ast() {
    delete left;
    delete right;
}

void Binary_Expr_Ast::print(int indent) {
    const char* op_str = "";
    bool is_condition = false;  // Whether this is a logical/comparison operator
    
    switch(op) {
        case PLUS_OP:  op_str = "Plus"; break;
        case MINUS_OP: op_str = "Minus"; break;
        case MULT_OP:  op_str = "Mult"; break;
        case DIV_OP:   op_str = "Div"; break;
        case LT_OP:    op_str = "LT"; is_condition = true; break;
        case GT_OP:    op_str = "GT"; is_condition = true; break;
        case LE_OP:    op_str = "LE"; is_condition = true; break;
        case GE_OP:    op_str = "GE"; is_condition = true; break;
        case EQ_OP:    op_str = "EQ"; is_condition = true; break;
        case NE_OP:    op_str = "NE"; is_condition = true; break;
        case AND_OP:   op_str = "AND"; is_condition = true; break;
        case OR_OP:    op_str = "OR"; is_condition = true; break;
    }
    
    if(is_condition) {
        fprintf(ast_file, "Condition: %s<bool>\n", op_str);
    } else {
        fprintf(ast_file, "Arith: %s<%s>\n", op_str, data_type_to_string(node_data_type));
    }
    
    for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "L_Opd (");
    
    // Check if left operand is complex
    Binary_Expr_Ast* left_bin = dynamic_cast<Binary_Expr_Ast*>(left);
    Ternary_Expr_Ast* left_tern = dynamic_cast<Ternary_Expr_Ast*>(left);
    Unary_Expr_Ast* left_unary = dynamic_cast<Unary_Expr_Ast*>(left);
    if(left_bin || left_tern || left_unary) {
        fprintf(ast_file, "\n");
        for(int i = 0; i < indent + 3; i++) fprintf(ast_file, " ");
    }
    left->print(indent+2);
    fprintf(ast_file, ")\n");
    
    for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "R_Opd (");
    
    // Check if right operand is complex
    Binary_Expr_Ast* right_bin = dynamic_cast<Binary_Expr_Ast*>(right);
    Ternary_Expr_Ast* right_tern = dynamic_cast<Ternary_Expr_Ast*>(right);
    Unary_Expr_Ast* right_unary = dynamic_cast<Unary_Expr_Ast*>(right);
    if(right_bin || right_tern || right_unary) {
        fprintf(ast_file, "\n");
        for(int i = 0; i < indent + 3; i++) fprintf(ast_file, " ");
    }
    right->print(indent+2);
    fprintf(ast_file, ")");
}

void Binary_Expr_Ast::pre_allocate_temps() {
    // Pre-allocate temps in operands first (depth-first)
    left->pre_allocate_temps();
    right->pre_allocate_temps();
    // Then allocate for this binary operation
    if (temp_id == -1) {
        temp_id = TAC_Generator::get_instance()->get_temp_counter();
        TAC_Generator::get_instance()->create_new_temp(
    node_data_type == FLOAT_DATA_TYPE
);
    }
}

TAC_Opd* Binary_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    TAC_Opd* left_tac = left->generate_tac(statements);
    TAC_Opd* right_tac = right->generate_tac(statements);
    
    // Use pre-allocated temp ID if available, otherwise create new
    TAC_Opd* result;
    if (temp_id != -1) {
        result = new Temp_TAC_Opd(temp_id, node_data_type);
    } else {
        result = TAC_Generator::get_instance()->create_new_temp();
    }
    
    // Convert operator to TAC_Op
    Compute_TAC_Stmt::TAC_Op tac_op;
    switch(op) {
        case PLUS_OP:  tac_op = Compute_TAC_Stmt::TAC_OP_PLUS; break;
        case MINUS_OP: tac_op = Compute_TAC_Stmt::TAC_OP_MINUS; break;
        case MULT_OP:  tac_op = Compute_TAC_Stmt::TAC_OP_MULT; break;
        case DIV_OP:   tac_op = Compute_TAC_Stmt::TAC_OP_DIV; break;
        case LT_OP:    tac_op = Compute_TAC_Stmt::TAC_OP_LT; break;
        case GT_OP:    tac_op = Compute_TAC_Stmt::TAC_OP_GT; break;
        case LE_OP:    tac_op = Compute_TAC_Stmt::TAC_OP_LE; break;
        case GE_OP:    tac_op = Compute_TAC_Stmt::TAC_OP_GE; break;
        case EQ_OP:    tac_op = Compute_TAC_Stmt::TAC_OP_EQ; break;
        case NE_OP:    tac_op = Compute_TAC_Stmt::TAC_OP_NE; break;
        case AND_OP:   tac_op = Compute_TAC_Stmt::TAC_OP_AND; break;
        case OR_OP:    tac_op = Compute_TAC_Stmt::TAC_OP_OR; break;
        default:       tac_op = Compute_TAC_Stmt::TAC_OP_PLUS; break;
    }
    
    statements.push_back(new Compute_TAC_Stmt(result, left_tac, tac_op, right_tac));
    
    return result;
}

/* --- TERNARY EXPRESSION --- */

Ternary_Expr_Ast::Ternary_Expr_Ast(Expression_Ast* cond, Expression_Ast* true_e, Expression_Ast* false_e)
    : condition(cond), true_expr(true_e), false_expr(false_e) {}

Ternary_Expr_Ast::~Ternary_Expr_Ast() {
    delete condition;
    delete true_expr;
    delete false_expr;
}

void Ternary_Expr_Ast::print(int indent) {
    // Get the operator from condition
    const char* cond_op_str = "";
    Binary_Expr_Ast* bin_cond = dynamic_cast<Binary_Expr_Ast*>(condition);
    
    if(bin_cond) {
        switch(bin_cond->op) {
            case Binary_Expr_Ast::PLUS_OP:  cond_op_str = "Plus"; break;
            case Binary_Expr_Ast::MINUS_OP: cond_op_str = "Minus"; break;
            case Binary_Expr_Ast::MULT_OP:  cond_op_str = "Mult"; break;
            case Binary_Expr_Ast::DIV_OP:   cond_op_str = "Div"; break;
            case Binary_Expr_Ast::LT_OP:    cond_op_str = "LT"; break;
            case Binary_Expr_Ast::GT_OP:    cond_op_str = "GT"; break;
            case Binary_Expr_Ast::LE_OP:    cond_op_str = "LE"; break;
            case Binary_Expr_Ast::GE_OP:    cond_op_str = "GE"; break;
            case Binary_Expr_Ast::EQ_OP:    cond_op_str = "EQ"; break;
            case Binary_Expr_Ast::NE_OP:    cond_op_str = "NE"; break;
            case Binary_Expr_Ast::AND_OP:   cond_op_str = "AND"; break;
            case Binary_Expr_Ast::OR_OP:    cond_op_str = "OR"; break;
        }
        
        fprintf(ast_file, "Condition: %s<bool>\n", cond_op_str);
        
        // Print the left operand of the condition
        for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
        fprintf(ast_file, "L_Opd (");
        
        // Check if left operand is complex
        Binary_Expr_Ast* left_bin = dynamic_cast<Binary_Expr_Ast*>(bin_cond->left);
        Ternary_Expr_Ast* left_tern = dynamic_cast<Ternary_Expr_Ast*>(bin_cond->left);
        if(left_bin || left_tern) {
            fprintf(ast_file, "\n");
            for(int i = 0; i < indent + 3; i++) fprintf(ast_file, " ");
        }
        bin_cond->left->print(indent+2);
        fprintf(ast_file, ")\n");
        
        // Print the right operand of the condition
        for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
        fprintf(ast_file, "R_Opd (");
        
        // Check if right operand is complex
        Binary_Expr_Ast* right_bin = dynamic_cast<Binary_Expr_Ast*>(bin_cond->right);
        Ternary_Expr_Ast* right_tern = dynamic_cast<Ternary_Expr_Ast*>(bin_cond->right);
        Unary_Expr_Ast* right_unary = dynamic_cast<Unary_Expr_Ast*>(bin_cond->right);
        if(right_bin || right_tern || right_unary) {
            fprintf(ast_file, "\n");
            for(int i = 0; i < indent + 3; i++) fprintf(ast_file, " ");
        }
        bin_cond->right->print(indent+2);
        fprintf(ast_file, ")\n");
    } else {
        // For non-binary conditions, just print the condition (typically a Name_Expr_Ast)
        condition->print(indent);
        fprintf(ast_file, "\n");
    }
    
    // Print True part - check if it's complex
    for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "True_Part (");
    
    Binary_Expr_Ast* true_bin = dynamic_cast<Binary_Expr_Ast*>(true_expr);
    Ternary_Expr_Ast* true_tern = dynamic_cast<Ternary_Expr_Ast*>(true_expr);
    
    bool wrap_true = false;
    if(true_bin) {
        wrap_true = true;
    } else if(true_tern) {
        // Wrap only if nested ternary has binary condition
        Binary_Expr_Ast* true_cond_bin = dynamic_cast<Binary_Expr_Ast*>(true_tern->get_condition());
        wrap_true = (true_cond_bin != nullptr);
    }
    
    if(wrap_true) {
        fprintf(ast_file, "\n");
        for(int i = 0; i < indent + 2; i++) fprintf(ast_file, " ");
    }
    true_expr->print(indent);
    fprintf(ast_file, ")\n");
    
    // Print False part - check if it's complex
    for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "False_Part (");
    
    Binary_Expr_Ast* false_bin = dynamic_cast<Binary_Expr_Ast*>(false_expr);
    Ternary_Expr_Ast* false_tern = dynamic_cast<Ternary_Expr_Ast*>(false_expr);
    
    bool wrap_false = false;
    if(false_bin) {
        wrap_false = true;
    } else if(false_tern) {
        // Wrap only if nested ternary has binary condition
        Binary_Expr_Ast* false_cond_bin = dynamic_cast<Binary_Expr_Ast*>(false_tern->get_condition());
        wrap_false = (false_cond_bin != nullptr);
    }
    
    if(wrap_false) {
        fprintf(ast_file, "\n");
        for(int i = 0; i < indent + 2; i++) fprintf(ast_file, " ");
    }
    false_expr->print(indent);
    fprintf(ast_file, ")");
}

void Ternary_Expr_Ast::pre_allocate_temps() {
    // Pre-allocate condition first
    condition->pre_allocate_temps();
    
    // Pre-allocate true branch
    true_expr->pre_allocate_temps();
    
    // Pre-allocate false branch
    false_expr->pre_allocate_temps();
    
    // Allocate the NOT temp for the condition (AFTER all branches)
    if (not_temp_id == -1) {
        not_temp_id = TAC_Generator::get_instance()->get_temp_counter();
        TAC_Generator::get_instance()->create_new_temp();
    }
    
    // Allocate labels for this ternary AFTER temps
    Label_TAC_Opd* false_lbl = TAC_Generator::get_instance()->create_new_label();
    false_label_id = false_lbl->get_label_id();
    delete false_lbl;
    
    Label_TAC_Opd* end_lbl = TAC_Generator::get_instance()->create_new_label();
    end_label_id = end_lbl->get_label_id();
    delete end_lbl;
}

TAC_Opd* Ternary_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    // Create result temporary (using stemp for ternary results)
    TAC_Opd* result = TAC_Generator::get_instance()->create_new_stemp(
        node_data_type == FLOAT_DATA_TYPE
    );
    
    // Evaluate condition
    TAC_Opd* cond_tac = condition->generate_tac(statements);
    
    // Use pre-allocated NOT temp ID (allocated in pre_allocate_temps)
    TAC_Opd* cond_not;
    if (not_temp_id == -1) {
        // Fallback if pre_allocate_temps wasn't called - shouldn't happen
        cond_not = TAC_Generator::get_instance()->create_new_temp();
    } else {
        // Use pre-allocated temp ID
        cond_not = new Temp_TAC_Opd(not_temp_id);
    }
    
    // Use pre-allocated labels
    Label_TAC_Opd* false_label = new Label_TAC_Opd(false_label_id);
    Label_TAC_Opd* end_label = new Label_TAC_Opd(end_label_id);
    
    // NOT condition
    statements.push_back(new Compute_TAC_Stmt(cond_not, cond_tac, Compute_TAC_Stmt::TAC_OP_NOT, NULL));
    
    // Conditional jump to false branch
    statements.push_back(new Cond_Goto_TAC_Stmt(cond_not, false_label));
    
    // True branch
    TAC_Opd* true_tac = true_expr->generate_tac(statements);
    statements.push_back(new Assign_TAC_Stmt(result, true_tac));
    
    // Jump to end
    statements.push_back(new Goto_TAC_Stmt(end_label));
    
    // False label
    statements.push_back(new Label_TAC_Stmt(false_label));
    
    // False branch
    TAC_Opd* false_tac = false_expr->generate_tac(statements);
    statements.push_back(new Assign_TAC_Stmt(result, false_tac));
    
    // End label
    statements.push_back(new Label_TAC_Stmt(end_label));
    
    return result;
}

/* ============================================================================
   STATEMENT AST IMPLEMENTATION
   ============================================================================ */

Statement_Ast::Statement_Ast() : Ast() {}
Statement_Ast::~Statement_Ast() {}

/* --- ASSIGNMENT STATEMENT --- */

Assignment_Stmt::Assignment_Stmt(string var_name, Expression_Ast* rhs_expr)
    : lhs_name(var_name), rhs(rhs_expr) {}

Assignment_Stmt::~Assignment_Stmt() {
    delete rhs;
}

void Assignment_Stmt::pre_allocate_temps() {
    // Pre-allocate temps for the RHS expression
    if (rhs) {
        rhs->pre_allocate_temps();
    }
}

void Assignment_Stmt::print(int indent) {
    fprintf(ast_file, "%*sAsgn:\n", indent, "");
    fprintf(ast_file, "%*sLHS (Name : %s<%s>)\n", indent+2, "", lhs_name.c_str(), data_type_to_string(rhs->get_data_type()));
    fprintf(ast_file, "%*sRHS (", indent+2, "");
    
    // Check if RHS needs wrapping - skip for simple constants
    Const_Expr_Ast* const_expr = dynamic_cast<Const_Expr_Ast*>(rhs);
    if(!const_expr) {
        Binary_Expr_Ast* bin_expr = dynamic_cast<Binary_Expr_Ast*>(rhs);
        Unary_Expr_Ast* unary_expr = dynamic_cast<Unary_Expr_Ast*>(rhs);
        Ternary_Expr_Ast* tern_expr = dynamic_cast<Ternary_Expr_Ast*>(rhs);
        
        bool should_wrap = false;
        if(bin_expr || unary_expr) {
            should_wrap = true;
        } else if(tern_expr) {
            Binary_Expr_Ast* cond_bin = dynamic_cast<Binary_Expr_Ast*>(tern_expr->get_condition());
            should_wrap = (cond_bin != nullptr);
        }
        
        if(should_wrap) {
            fprintf(ast_file, "\n");
            for(int i = 0; i < indent + 4; i++) fprintf(ast_file, " ");
        }
    }
    
    rhs->print(indent + 2);
    fprintf(ast_file, ")");
}

void Assignment_Stmt::print_no_trailing_newline(int indent) {
    for(int i = 0; i < indent; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "Asgn:\n");
    
    for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "LHS (Name : %s<%s>)\n", lhs_name.c_str(), data_type_to_string(rhs->get_data_type()));
    
    for(int i = 0; i < indent + 1; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "RHS (");
    
    // Check if RHS needs wrapping - skip for simple constants
    Const_Expr_Ast* const_expr = dynamic_cast<Const_Expr_Ast*>(rhs);
    if(!const_expr) {
        Binary_Expr_Ast* bin_expr = dynamic_cast<Binary_Expr_Ast*>(rhs);
        Unary_Expr_Ast* unary_expr = dynamic_cast<Unary_Expr_Ast*>(rhs);
        Ternary_Expr_Ast* tern_expr = dynamic_cast<Ternary_Expr_Ast*>(rhs);
        
        bool should_wrap = false;
        if(bin_expr || unary_expr) {
            should_wrap = true;
        } else if(tern_expr) {
            // Wrap Ternary only if its condition is a Binary expression
            Binary_Expr_Ast* cond_bin = dynamic_cast<Binary_Expr_Ast*>(tern_expr->get_condition());
            should_wrap = (cond_bin != nullptr);
        }
        
        if(should_wrap) {
            fprintf(ast_file, "\n");
            for(int i = 0; i < indent + 3; i++) fprintf(ast_file, " ");
        }
    }
    
    rhs->print(indent + 2);
    fprintf(ast_file, ")");  // NO NEWLINE
}

TAC_Opd* Assignment_Stmt::generate_tac(list<TAC_Stmt*>& statements) {
    TAC_Opd* rhs_tac = rhs->generate_tac(statements);
    Var_TAC_Opd* lhs_tac = new Var_TAC_Opd(lhs_name, rhs->get_data_type());
    
    statements.push_back(new Assign_TAC_Stmt(lhs_tac, rhs_tac));
    
    return NULL;
}

/* --- READ STATEMENT --- */

Read_Stmt::Read_Stmt(string name) : var_name(name) {}

Read_Stmt::~Read_Stmt() {}

void Read_Stmt::print(int indent) {
    fprintf(ast_file, "Read: Name : %s<%s>", var_name.c_str(), data_type_to_string(node_data_type));
}

TAC_Opd* Read_Stmt::generate_tac(list<TAC_Stmt*>& statements) {
    Var_TAC_Opd* var = new Var_TAC_Opd(var_name, node_data_type);
    statements.push_back(new Read_TAC_Stmt(var));
    return NULL;
}

/* --- PRINT STATEMENT --- */

Print_Stmt::Print_Stmt(Expression_Ast* e) : expr(e) {}

Print_Stmt::~Print_Stmt() {
    delete expr;
}

void Print_Stmt::print(int indent) {
    fprintf(ast_file, "Write: ");
    expr->print(indent);
}

void Print_Stmt::print_inline(int indent) {
    for(int i = 0; i < indent; i++) fprintf(ast_file, " ");
    fprintf(ast_file, "Write: ");
    expr->print(indent);
    // No newline for inline printing
}

void Print_Stmt::pre_allocate_temps() {
    if (expr) {
        expr->pre_allocate_temps();
    }
}

TAC_Opd* Print_Stmt::generate_tac(list<TAC_Stmt*>& statements) {
    TAC_Opd* expr_tac = expr->generate_tac(statements);
    statements.push_back(new Print_TAC_Stmt(expr_tac));
    return NULL;
}

/* --- IF STATEMENT --- */

If_Stmt::If_Stmt(Expression_Ast* cond, Statement_Ast* then_s, Statement_Ast* else_s)
    : condition(cond), then_stmt(then_s), else_stmt(else_s) {}

If_Stmt::~If_Stmt() {
    delete condition;
    delete then_stmt;
    if(else_stmt) delete else_stmt;
}

void If_Stmt::print(int indent) {
    fprintf(ast_file, "%*sIf:\n", indent, "");
    fprintf(ast_file, "%*sCondition (\n", indent+2, "");
    fprintf(ast_file, "%*s", indent+4, "");
    condition->print(indent+4);
    fprintf(ast_file, ")\n");
    fprintf(ast_file, "%*sThen (\n", indent+2, "");
    fprintf(ast_file, "%*s", indent+4, "");
    then_stmt->print(indent+4);
    if(else_stmt) {
        fprintf(ast_file, ")");
        fprintf(ast_file, "\n");
        fprintf(ast_file, "%*sElse (\n", indent+2, "");
        fprintf(ast_file, "%*s", indent+4, "");
        else_stmt->print(indent+4);
        fprintf(ast_file, ")");
    } else {
        fprintf(ast_file, ")");
    }
}

void If_Stmt::pre_allocate_temps() {
    // Process condition temps first
    if (condition) {
        condition->pre_allocate_temps();
    }
    // Process then branch
    if (then_stmt) {
        then_stmt->pre_allocate_temps();
    }
    // Pre-allocate NOT temp for condition AFTER then_stmt
    not_temp_id = TAC_Generator::get_instance()->get_temp_counter();
    TAC_Opd* dummy = TAC_Generator::get_instance()->create_new_temp();
    delete dummy;
    
    // ALLOCATE LABELS FOR THIS IF STATEMENT BEFORE ELSE BRANCH
    // If there's an else branch, allocate both else_label and end_label
    // Otherwise, they should be the same
    if (else_stmt) {
        // Allocate end_label FIRST
        Label_TAC_Opd* end_lbl = TAC_Generator::get_instance()->create_new_label();
        end_label_id = end_lbl->get_label_id();
        delete end_lbl;
        
        // Then allocate else_label
        Label_TAC_Opd* else_lbl = TAC_Generator::get_instance()->create_new_label();
        else_label_id = else_lbl->get_label_id();
        delete else_lbl;
        
        // THEN process else branch AFTER allocating labels
        else_stmt->pre_allocate_temps();
    } else {
        // No else branch: allocate only end_label, use it as else_label too
        Label_TAC_Opd* end_lbl = TAC_Generator::get_instance()->create_new_label();
        end_label_id = end_lbl->get_label_id();
        else_label_id = end_label_id;  // Same label for both
        delete end_lbl;
    }
}

TAC_Opd* If_Stmt::generate_tac(list<TAC_Stmt*>& statements) {
    TAC_Opd* cond_tac = condition->generate_tac(statements);
    
    // Create fresh Label_TAC_Opd objects with stored IDs (correct order)
    Label_TAC_Opd* else_label = new Label_TAC_Opd(else_label_id);   // else_label_id for else branch
    Label_TAC_Opd* end_label = new Label_TAC_Opd(end_label_id);     // end_label_id for end
    
    // Use pre-allocated NOT temp
    Temp_TAC_Opd* cond_not = new Temp_TAC_Opd(not_temp_id, INT_DATA_TYPE);
    
    statements.push_back(new Compute_TAC_Stmt(cond_not, cond_tac, Compute_TAC_Stmt::TAC_OP_NOT, NULL));
    statements.push_back(new Cond_Goto_TAC_Stmt(cond_not, else_label));

    // Then branch
    then_stmt->generate_tac(statements);
    statements.push_back(new Goto_TAC_Stmt(end_label));
    
    // Else branch - only print label if it's different from end_label
    if (else_label_id != end_label_id) {
        statements.push_back(new Label_TAC_Stmt(else_label));
        if(else_stmt) {
            else_stmt->generate_tac(statements);
        }
    } else {
        // else_label is the same as end_label, so condition jumps directly to end
        if(else_stmt) {
            else_stmt->generate_tac(statements);
        }
    }
    
    // End label
    statements.push_back(new Label_TAC_Stmt(end_label));
    
    return NULL;
}

/* --- WHILE STATEMENT --- */

While_Stmt::While_Stmt(Expression_Ast* cond, Statement_Ast* body_stmt)
    : condition(cond), body(body_stmt) {}

While_Stmt::~While_Stmt() {
    delete condition;
    delete body;
}

void While_Stmt::print(int indent) {
    fprintf(ast_file, "%*sWhile:\n", indent, "");
    fprintf(ast_file, "%*sCondition (\n", indent+2, "");
    fprintf(ast_file, "%*s", indent+4, "");
    condition->print(indent+4);
    fprintf(ast_file, ")\n");
    fprintf(ast_file, "%*sBody (\n", indent+2, "");
    fprintf(ast_file, "%*s", indent+4, "");
    body->print(indent+4);
    fprintf(ast_file, ")");
}

void While_Stmt::pre_allocate_temps() {
    // Process condition first
    if (condition) {
        condition->pre_allocate_temps();
    }
    // Process body BEFORE allocating while labels
    if (body) {
        body->pre_allocate_temps();
    }
    
    // Pre-allocate NOT temp for condition
    not_temp_id = TAC_Generator::get_instance()->get_temp_counter();
    TAC_Opd* dummy = TAC_Generator::get_instance()->create_new_temp();
    delete dummy;
    
    // Pre-allocate labels LAST for this while statement
    Label_TAC_Opd* loop_lbl = TAC_Generator::get_instance()->create_new_label();
    loop_label_id = loop_lbl->get_label_id();
    delete loop_lbl;
    
    Label_TAC_Opd* exit_lbl = TAC_Generator::get_instance()->create_new_label();
    exit_label_id = exit_lbl->get_label_id();
    delete exit_lbl;
}

TAC_Opd* While_Stmt::generate_tac(list<TAC_Stmt*>& statements) {
    // Create fresh Label_TAC_Opd objects with stored IDs
    Label_TAC_Opd* loop_label = new Label_TAC_Opd(loop_label_id);
    Label_TAC_Opd* exit_label = new Label_TAC_Opd(exit_label_id);
    
    statements.push_back(new Label_TAC_Stmt(loop_label));
    
    TAC_Opd* cond_tac = condition->generate_tac(statements);
    
    // Use pre-allocated NOT temp
    Temp_TAC_Opd* cond_not = new Temp_TAC_Opd(not_temp_id, INT_DATA_TYPE);
    
    statements.push_back(new Compute_TAC_Stmt(cond_not, cond_tac, Compute_TAC_Stmt::TAC_OP_NOT, NULL));
    statements.push_back(new Cond_Goto_TAC_Stmt(cond_not, exit_label));

    body->generate_tac(statements);

    statements.push_back(new Goto_TAC_Stmt(loop_label));
    statements.push_back(new Label_TAC_Stmt(exit_label));
    
    return NULL;
}

/* --- DO-WHILE STATEMENT --- */

Do_While_Stmt::Do_While_Stmt(Statement_Ast* body_stmt, Expression_Ast* cond)
    : body(body_stmt), condition(cond) {}

Do_While_Stmt::~Do_While_Stmt() {
    delete body;
    delete condition;
}

void Do_While_Stmt::print(int indent) {
    fprintf(ast_file, "%*sDo:\n", indent, "");
    fprintf(ast_file, "%*sBody (\n", indent+2, "");
    fprintf(ast_file, "%*s", indent+4, "");
    body->print(indent+4);
    fprintf(ast_file, ")\n");
    
    // Check if condition is simple (Name or Const)
    Name_Expr_Ast* name_cond = dynamic_cast<Name_Expr_Ast*>(condition);
    Const_Expr_Ast* const_cond = dynamic_cast<Const_Expr_Ast*>(condition);
    
    if(name_cond || const_cond) {
        // Simple condition - print on same line
        fprintf(ast_file, "%*sWhile Condition (", indent+2, "");
        condition->print(indent+2);
        fprintf(ast_file, ")");
    } else {
        // Complex condition - print on new line with indentation
        fprintf(ast_file, "%*sWhile Condition (\n", indent+2, "");
        fprintf(ast_file, "%*s", indent+4, "");
        condition->print(indent+4);
        fprintf(ast_file, ")");
    }
}

void Do_While_Stmt::pre_allocate_temps() {
    if (body) {
        body->pre_allocate_temps();
    }
    if (condition) {
        condition->pre_allocate_temps();
    }
    // Pre-allocate label ID
    Label_TAC_Opd* loop_lbl = TAC_Generator::get_instance()->create_new_label();
    loop_label_id = loop_lbl->get_label_id();
    delete loop_lbl;
}

TAC_Opd* Do_While_Stmt::generate_tac(list<TAC_Stmt*>& statements) {
    // Create fresh Label_TAC_Opd object with stored ID
    Label_TAC_Opd* loop_label = new Label_TAC_Opd(loop_label_id);
    
    statements.push_back(new Label_TAC_Stmt(loop_label));
    
    body->generate_tac(statements);
    
    TAC_Opd* cond_tac = condition->generate_tac(statements);
    
    // Jump back to loop if condition is true (no NOT needed)
    statements.push_back(new Cond_Goto_TAC_Stmt(cond_tac, loop_label));
    
    return NULL;
}

/* --- COMPOUND STATEMENT --- */

Compound_Stmt::Compound_Stmt() : Statement_Ast() {}

Compound_Stmt::~Compound_Stmt() {
    for(auto stmt : statements) {
        delete stmt;
    }
    statements.clear();
}

void Compound_Stmt::add_stmt(Statement_Ast* stmt) {
    if(stmt) statements.push_back(stmt);
}

void Compound_Stmt::print(int indent) {
    for(auto it = statements.begin(); it != statements.end(); ++it) {
        (*it)->print(indent);
        // Add newline after each statement except the last
        auto next = it;
        ++next;
        if(next != statements.end()) {
            fprintf(ast_file, "\n");
        } else {
            // Last statement - no newline after it
        }
    }
}

void Compound_Stmt::print_inline_if_single_print(int indent) {
    // If this compound statement has exactly one statement and it's a Print_Stmt, print it inline
    if(statements.size() == 1) {
        Print_Stmt* print_stmt = dynamic_cast<Print_Stmt*>(statements.front());
        if(print_stmt) {
            print_stmt->print_inline(indent);
            return;
        }
    }
    // Otherwise, print normally
    print(indent);
}

void Compound_Stmt::print_with_last_no_newline(int indent) {
    // Print all statements except the last normally
    if(statements.size() == 0) return;
    
    // Print all but last
    for(size_t i = 0; i < statements.size() - 1; i++) {
        auto it = statements.begin();
        advance(it, i);
        (*it)->print(indent);
    }
    
    // Print last statement without trailing newline (if it's an Assignment)
    Statement_Ast* last_stmt = statements.back();
    Assignment_Stmt* assign_stmt = dynamic_cast<Assignment_Stmt*>(last_stmt);
    if(assign_stmt) {
        assign_stmt->print_no_trailing_newline(indent);
    } else {
        last_stmt->print(indent);
    }
}

void Compound_Stmt::pre_allocate_temps() {
    for(auto stmt : statements) {
        stmt->pre_allocate_temps();
    }
}

TAC_Opd* Compound_Stmt::generate_tac(list<TAC_Stmt*>& statements_list) {
    for(auto stmt : statements) {
        stmt->generate_tac(statements_list);
    }
    return NULL;
}

/* ============================================================================
   UTILITY FUNCTIONS
   ============================================================================ */

const char* data_type_to_string(DataType t) {
    switch(t) {
        case INT_DATA_TYPE:    return "int";
        case FLOAT_DATA_TYPE:  return "float";
        case BOOL_DATA_TYPE:   return "bool";
        case STRING_DATA_TYPE: return "string";
        case CHAR_DATA_TYPE:   return "char";
        case VOID_DATA_TYPE:   return "void";
        case ERROR_DATA_TYPE:  return "error";
        default:               return "unknown";
    }
}

void print_ast(Ast* root) {
    if(!root) return;
    root->print(0);
}

Print_Stmt* get_single_print_stmt(Statement_Ast* stmt) {
    // Check if stmt is directly a Print_Stmt
    Print_Stmt* print_stmt = dynamic_cast<Print_Stmt*>(stmt);
    if(print_stmt) return print_stmt;
    
    // Check if it's a Compound_Stmt with exactly one statement that is a Print_Stmt
    Compound_Stmt* compound = dynamic_cast<Compound_Stmt*>(stmt);
    if(!compound) return nullptr;
    
    // For now, we can't easily check the size of statements list since it's private
    // So we'll use a different approach: try to cast and check
    // This is a limitation but we can work around it in the calling code
    return nullptr;
}

