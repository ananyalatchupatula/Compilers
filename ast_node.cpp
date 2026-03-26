#include "ast_node.h"
#include "tac_str.h"
#include "tac_generator.h"
#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

// ============================================================================
// AST BASE CLASS
// ============================================================================

Ast::Ast() : node_data_type(VOID_DATA_TYPE), lineno(0) {}

// ============================================================================
// EXPRESSION_AST CLASS
// ============================================================================

Expression_Ast::Expression_Ast() : Ast() {}

// ============================================================================
// UNARY_EXPR_AST CLASS
// ============================================================================

Unary_Expr_Ast::Unary_Expr_Ast(Unary_Op op, Expression_Ast *opd) 
    : Expression_Ast(), oper(op), operand(opd) {
    if (opd) {
        node_data_type = opd->get_data_type();
    }
}

Unary_Expr_Ast::~Unary_Expr_Ast() {
    if (operand) delete operand;
}

void Unary_Expr_Ast::print(FILE *file) {
    fprintf(file, "(");
    if (oper == UMINUS_OP) {
        fprintf(file, "Uminus: ");
    } else if (oper == NOT_OP) {
        fprintf(file, "NOT: ");
    }
    if (operand) operand->print(file);
    fprintf(file, ")");
}

TAC_Opd* Unary_Expr_Ast::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!operand) return NULL;
    
    // Generate TAC for operand first
    TAC_Opd *opd1_result = operand->generate_tac(tac_list);
    if (!opd1_result) return NULL;
    
    // Create result temporary
    TAC_Generator *gen = TAC_Generator::get_instance();
    TAC_Opd *result = gen->create_new_temp();
    
    // Generate compute statement
    Compute_TAC_Stmt::TAC_Op op = (oper == UMINUS_OP) ? Compute_TAC_Stmt::UMINUS : Compute_TAC_Stmt::NOT;
    TAC_Stmt *stmt = new Compute_TAC_Stmt(result, opd1_result, op);
    tac_list.push_back(stmt);
    
    return result;
}

// ============================================================================
// BINARY_EXPR_AST CLASS
// ============================================================================

Binary_Expr_Ast::Binary_Expr_Ast(Expression_Ast *l, Binary_Op op, Expression_Ast *r) 
    : Expression_Ast(), oper(op), lhs(l), rhs(r) {
    
    // Determine result type based on operation
    if (op >= GT_OP && op <= NE_OP) {
        // Relational operators return bool
        node_data_type = BOOL_DATA_TYPE;
    } else if (op >= AND_OP && op <= OR_OP) {
        // Logical operators return bool
        node_data_type = BOOL_DATA_TYPE;
    } else {
        // Arithmetic operators
        if (lhs) node_data_type = lhs->get_data_type();
    }
}

Binary_Expr_Ast::~Binary_Expr_Ast() {
    if (lhs) delete lhs;
    if (rhs) delete rhs;
}

void Binary_Expr_Ast::print(FILE *file) {
    fprintf(file, "(");
    if (lhs) lhs->print(file);
    fprintf(file, " ");
    
    switch (oper) {
        case PLUS_OP: fprintf(file, "Plus"); break;
        case MINUS_OP: fprintf(file, "Minus"); break;
        case MULT_OP: fprintf(file, "Mult"); break;
        case DIV_OP: fprintf(file, "Div"); break;
        case GT_OP: fprintf(file, "GT"); break;
        case GE_OP: fprintf(file, "GE"); break;
        case LT_OP: fprintf(file, "LT"); break;
        case LE_OP: fprintf(file, "LE"); break;
        case EQ_OP: fprintf(file, "EQ"); break;
        case NE_OP: fprintf(file, "NE"); break;
        case AND_OP: fprintf(file, "AND"); break;
        case OR_OP: fprintf(file, "OR"); break;
        default: fprintf(file, "Unknown"); break;
    }
    
    fprintf(file, " ");
    if (rhs) rhs->print(file);
    fprintf(file, ")");
}

TAC_Opd* Binary_Expr_Ast::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!lhs || !rhs) return NULL;
    
    // Generate TAC for left and right operands
    TAC_Opd *lhs_result = lhs->generate_tac(tac_list);
    TAC_Opd *rhs_result = rhs->generate_tac(tac_list);
    if (!lhs_result || !rhs_result) return NULL;
    
    // Create result temporary
    TAC_Generator *gen = TAC_Generator::get_instance();
    bool is_float = (node_data_type == FLOAT_DATA_TYPE);
    TAC_Opd *result = gen->create_new_temp(is_float);
    
    // Map Binary_Op to Compute_TAC_Stmt::TAC_Op
    Compute_TAC_Stmt::TAC_Op tac_op;
    switch (oper) {
        case PLUS_OP: tac_op = Compute_TAC_Stmt::PLUS; break;
        case MINUS_OP: tac_op = Compute_TAC_Stmt::MINUS; break;
        case MULT_OP: tac_op = Compute_TAC_Stmt::MULT; break;
        case DIV_OP: tac_op = Compute_TAC_Stmt::DIV; break;
        case GT_OP: tac_op = Compute_TAC_Stmt::GT; break;
        case GE_OP: tac_op = Compute_TAC_Stmt::GE; break;
        case LT_OP: tac_op = Compute_TAC_Stmt::LT; break;
        case LE_OP: tac_op = Compute_TAC_Stmt::LE; break;
        case EQ_OP: tac_op = Compute_TAC_Stmt::EQ; break;
        case NE_OP: tac_op = Compute_TAC_Stmt::NE; break;
        case AND_OP: tac_op = Compute_TAC_Stmt::AND; break;
        case OR_OP: tac_op = Compute_TAC_Stmt::OR; break;
        default: return NULL;
    }
    
    // Generate compute statement
    TAC_Stmt *stmt = new Compute_TAC_Stmt(result, lhs_result, tac_op, rhs_result);
    tac_list.push_back(stmt);
    
    return result;
}

// ============================================================================
// TERNARY_EXPR_AST CLASS
// ============================================================================

Ternary_Expr_Ast::Ternary_Expr_Ast(Expression_Ast *c, Expression_Ast *t, Expression_Ast *f) 
    : Expression_Ast(), cond(c), true_expr(t), false_expr(f) {
    if (true_expr) {
        node_data_type = true_expr->get_data_type();
    }
}

Ternary_Expr_Ast::~Ternary_Expr_Ast() {
    if (cond) delete cond;
    if (true_expr) delete true_expr;
    if (false_expr) delete false_expr;
}

void Ternary_Expr_Ast::print(FILE *file) {
    fprintf(file, "(");
    if (cond) cond->print(file);
    fprintf(file, " ? ");
    if (true_expr) true_expr->print(file);
    fprintf(file, " : ");
    if (false_expr) false_expr->print(file);
    fprintf(file, ")");
}

TAC_Opd* Ternary_Expr_Ast::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!cond || !true_expr || !false_expr) return NULL;
    
    TAC_Generator *gen = TAC_Generator::get_instance();
    
    // Generate condition
    TAC_Opd *cond_result = cond->generate_tac(tac_list);
    if (!cond_result) return NULL;
    
    // Create labels for true and false branches
    Label_TAC_Opd *true_label = gen->create_new_label();
    Label_TAC_Opd *false_label = gen->create_new_label();
    Label_TAC_Opd *end_label = gen->create_new_label();
    
    // If condition is true, goto true_label; else goto false_label
    TAC_Stmt *cond_goto = new Cond_Goto_TAC_Stmt(cond_result, true_label);
    tac_list.push_back(cond_goto);
    
    // Generate false branch
    TAC_Stmt *false_lbl_stmt = new Label_TAC_Stmt(false_label);
    tac_list.push_back(false_lbl_stmt);
    
    TAC_Opd *false_result = false_expr->generate_tac(tac_list);
    if (!false_result) return NULL;
    
    // Create result temporary
    bool is_float = (node_data_type == FLOAT_DATA_TYPE);
    TAC_Opd *result = gen->create_new_temp(is_float);
    
    // Assign false result to temp
    TAC_Stmt *false_assign = new Assign_TAC_Stmt(result, false_result);
    tac_list.push_back(false_assign);
    
    // Goto end
    TAC_Stmt *false_goto = new Goto_TAC_Stmt(end_label);
    tac_list.push_back(false_goto);
    
    // Generate true branch
    TAC_Stmt *true_lbl_stmt = new Label_TAC_Stmt(true_label);
    tac_list.push_back(true_lbl_stmt);
    
    TAC_Opd *true_result = true_expr->generate_tac(tac_list);
    if (!true_result) return NULL;
    
    // Assign true result to temp
    TAC_Stmt *true_assign = new Assign_TAC_Stmt(result, true_result);
    tac_list.push_back(true_assign);
    
    // End label
    TAC_Stmt *end_lbl_stmt = new Label_TAC_Stmt(end_label);
    tac_list.push_back(end_lbl_stmt);
    
    return result;
}

// ============================================================================
// CONST_EXPR_AST CLASS
// ============================================================================

Const_Expr_Ast::Const_Expr_Ast(int val) 
    : Expression_Ast(), int_value(val), float_value(0), string_value("") {
    node_data_type = INT_DATA_TYPE;
}

Const_Expr_Ast::Const_Expr_Ast(double val) 
    : Expression_Ast(), int_value(0), float_value(val), string_value("") {
    node_data_type = FLOAT_DATA_TYPE;
}

Const_Expr_Ast::Const_Expr_Ast(string val) 
    : Expression_Ast(), int_value(0), float_value(0), string_value(val) {
    node_data_type = STRING_DATA_TYPE;
}

Const_Expr_Ast::~Const_Expr_Ast() {}

void Const_Expr_Ast::print(FILE *file) {
    if (node_data_type == STRING_DATA_TYPE) {
        fprintf(file, "Num: \"%s\"", string_value.c_str());
    } else if (node_data_type == FLOAT_DATA_TYPE) {
        fprintf(file, "Num: %g", float_value);
    } else {
        fprintf(file, "Num: %d", int_value);
    }
}

TAC_Opd* Const_Expr_Ast::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (node_data_type == STRING_DATA_TYPE) {
        return new Const_TAC_Opd(string_value);
    } else if (node_data_type == FLOAT_DATA_TYPE) {
        return new Const_TAC_Opd(float_value);
    } else {
        return new Const_TAC_Opd(int_value);
    }
}

// ============================================================================
// NAME_EXPR_AST CLASS
// ============================================================================

Name_Expr_Ast::Name_Expr_Ast(string name) 
    : Expression_Ast(), var_name(name) {}

Name_Expr_Ast::~Name_Expr_Ast() {}

void Name_Expr_Ast::print(FILE *file) {
    fprintf(file, "Name: %s", var_name.c_str());
}

TAC_Opd* Name_Expr_Ast::generate_tac(list<TAC_Stmt*> &tac_list) {
    return new Var_TAC_Opd(var_name);
}

// ============================================================================
// STATEMENT_AST CLASS
// ============================================================================

Statement_Ast::Statement_Ast() : Ast() {}

// ============================================================================
// ASSIGNMENT_STMT CLASS
// ============================================================================

Assignment_Stmt::Assignment_Stmt(string name, Expression_Ast *expr) 
    : Statement_Ast(), lhs_name(name), rhs(expr) {}

Assignment_Stmt::~Assignment_Stmt() {
    if (rhs) delete rhs;
}

void Assignment_Stmt::print(FILE *file) {
    fprintf(file, "Asgn: ");
    fprintf(file, "%s ", lhs_name.c_str());
    if (rhs) rhs->print(file);
}

TAC_Opd* Assignment_Stmt::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!rhs) return NULL;
    
    // Generate TAC for RHS
    TAC_Opd *rhs_result = rhs->generate_tac(tac_list);
    if (!rhs_result) return NULL;
    
    // Create LHS variable
    TAC_Opd *lhs_var = new Var_TAC_Opd(lhs_name);
    
    // Generate assignment statement
    TAC_Stmt *stmt = new Assign_TAC_Stmt(lhs_var, rhs_result);
    tac_list.push_back(stmt);
    
    return lhs_var;
}

// ============================================================================
// PRINT_STMT CLASS
// ============================================================================

Print_Stmt::Print_Stmt(Expression_Ast *e) 
    : Statement_Ast(), expr(e) {}

Print_Stmt::~Print_Stmt() {
    if (expr) delete expr;
}

void Print_Stmt::print(FILE *file) {
    fprintf(file, "Write: ");
    if (expr) expr->print(file);
}

TAC_Opd* Print_Stmt::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!expr) return NULL;
    
    // Generate TAC for expression
    TAC_Opd *expr_result = expr->generate_tac(tac_list);
    if (!expr_result) return NULL;
    
    // Generate print statement
    TAC_Stmt *stmt = new Print_TAC_Stmt(expr_result);
    tac_list.push_back(stmt);
    
    return expr_result;
}

// ============================================================================
// READ_STMT CLASS
// ============================================================================

Read_Stmt::Read_Stmt(string name) 
    : Statement_Ast(), var_name(name) {}

Read_Stmt::~Read_Stmt() {}

void Read_Stmt::print(FILE *file) {
    fprintf(file, "Read: %s", var_name.c_str());
}

TAC_Opd* Read_Stmt::generate_tac(list<TAC_Stmt*> &tac_list) {
    // Create variable operand
    TAC_Opd *var = new Var_TAC_Opd(var_name);
    
    // Generate read statement
    TAC_Stmt *stmt = new Read_TAC_Stmt(var);
    tac_list.push_back(stmt);
    
    return var;
}

// ============================================================================
// IF_STMT CLASS
// ============================================================================

If_Stmt::If_Stmt(Expression_Ast *c, Statement_Ast *t, Statement_Ast *e) 
    : Statement_Ast(), cond(c), then_stmt(t), else_stmt(e) {}

If_Stmt::~If_Stmt() {
    if (cond) delete cond;
    if (then_stmt) delete then_stmt;
    if (else_stmt) delete else_stmt;
}

void If_Stmt::print(FILE *file) {
    fprintf(file, "If: ");
    if (cond) cond->print(file);
    fprintf(file, "\n");
    fprintf(file, "Then: ");
    if (then_stmt) then_stmt->print(file);
    if (else_stmt) {
        fprintf(file, "\n");
        fprintf(file, "Else: ");
        else_stmt->print(file);
    }
}

TAC_Opd* If_Stmt::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!cond || !then_stmt) return NULL;
    
    TAC_Generator *gen = TAC_Generator::get_instance();
    
    // Generate condition
    TAC_Opd *cond_result = cond->generate_tac(tac_list);
    if (!cond_result) return NULL;
    
    // Create labels
    Label_TAC_Opd *else_label = gen->create_new_label();
    Label_TAC_Opd *end_label = gen->create_new_label();
    
    // If condition is false, goto else_label
    TAC_Stmt *cond_goto = new Cond_Goto_TAC_Stmt(cond_result, else_label);
    tac_list.push_back(cond_goto);
    
    // Generate then branch
    then_stmt->generate_tac(tac_list);
    
    // Goto end
    TAC_Stmt *then_goto = new Goto_TAC_Stmt(end_label);
    tac_list.push_back(then_goto);
    
    // Else label
    TAC_Stmt *else_lbl_stmt = new Label_TAC_Stmt(else_label);
    tac_list.push_back(else_lbl_stmt);
    
    // Generate else branch if it exists
    if (else_stmt) {
        else_stmt->generate_tac(tac_list);
    }
    
    // End label
    TAC_Stmt *end_lbl_stmt = new Label_TAC_Stmt(end_label);
    tac_list.push_back(end_lbl_stmt);
    
    return NULL;
}

// ============================================================================
// WHILE_STMT CLASS
// ============================================================================

While_Stmt::While_Stmt(Expression_Ast *c, Statement_Ast *b) 
    : Statement_Ast(), cond(c), body(b) {}

While_Stmt::~While_Stmt() {
    if (cond) delete cond;
    if (body) delete body;
}

void While_Stmt::print(FILE *file) {
    fprintf(file, "While: ");
    if (cond) cond->print(file);
    fprintf(file, "\n");
    fprintf(file, "Body: ");
    if (body) body->print(file);
}

TAC_Opd* While_Stmt::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!cond || !body) return NULL;
    
    TAC_Generator *gen = TAC_Generator::get_instance();
    
    // Create labels
    Label_TAC_Opd *loop_label = gen->create_new_label();
    Label_TAC_Opd *end_label = gen->create_new_label();
    
    // Loop label
    TAC_Stmt *loop_lbl_stmt = new Label_TAC_Stmt(loop_label);
    tac_list.push_back(loop_lbl_stmt);
    
    // Generate condition
    TAC_Opd *cond_result = cond->generate_tac(tac_list);
    if (!cond_result) return NULL;
    
    // If condition is false, goto end_label
    TAC_Stmt *cond_goto = new Cond_Goto_TAC_Stmt(cond_result, end_label);
    tac_list.push_back(cond_goto);
    
    // Generate body
    body->generate_tac(tac_list);
    
    // Goto loop
    TAC_Stmt *loop_goto = new Goto_TAC_Stmt(loop_label);
    tac_list.push_back(loop_goto);
    
    // End label
    TAC_Stmt *end_lbl_stmt = new Label_TAC_Stmt(end_label);
    tac_list.push_back(end_lbl_stmt);
    
    return NULL;
}

// ============================================================================
// DO_WHILE_STMT CLASS
// ============================================================================

Do_While_Stmt::Do_While_Stmt(Statement_Ast *b, Expression_Ast *c) 
    : Statement_Ast(), body(b), cond(c) {}

Do_While_Stmt::~Do_While_Stmt() {
    if (body) delete body;
    if (cond) delete cond;
}

void Do_While_Stmt::print(FILE *file) {
    fprintf(file, "Do_While: ");
    fprintf(file, "Body: ");
    if (body) body->print(file);
    fprintf(file, "\n");
    fprintf(file, "While: ");
    if (cond) cond->print(file);
}

TAC_Opd* Do_While_Stmt::generate_tac(list<TAC_Stmt*> &tac_list) {
    if (!body || !cond) return NULL;
    
    TAC_Generator *gen = TAC_Generator::get_instance();
    
    // Create labels
    Label_TAC_Opd *loop_label = gen->create_new_label();
    Label_TAC_Opd *end_label = gen->create_new_label();
    
    // Loop label
    TAC_Stmt *loop_lbl_stmt = new Label_TAC_Stmt(loop_label);
    tac_list.push_back(loop_lbl_stmt);
    
    // Generate body
    body->generate_tac(tac_list);
    
    // Generate condition
    TAC_Opd *cond_result = cond->generate_tac(tac_list);
    if (!cond_result) return NULL;
    
    // If condition is true, goto loop
    TAC_Stmt *cond_goto = new Cond_Goto_TAC_Stmt(cond_result, loop_label);
    tac_list.push_back(cond_goto);
    
    return NULL;
}

// ============================================================================
// COMPOUND_STMT CLASS
// ============================================================================

Compound_Stmt::Compound_Stmt() : Statement_Ast() {}

Compound_Stmt::~Compound_Stmt() {
    for (auto stmt : statements) {
        if (stmt) delete stmt;
    }
    statements.clear();
}

void Compound_Stmt::print(FILE *file) {
    fprintf(file, "Compound: ");
    for (auto stmt : statements) {
        if (stmt) {
            stmt->print(file);
            fprintf(file, "\n");
        }
    }
}

TAC_Opd* Compound_Stmt::generate_tac(list<TAC_Stmt*> &tac_list) {
    for (auto stmt : statements) {
        if (stmt) {
            stmt->generate_tac(tac_list);
        }
    }
    return NULL;
}

void Compound_Stmt::add_statement(Statement_Ast *stmt) {
    if (stmt) {
        statements.push_back(stmt);
    }
}
