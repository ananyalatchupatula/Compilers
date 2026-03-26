// Example: Complete Program Showing New Class-Based Design
// This file demonstrates how to use the new SCLP classes

#include <iostream>
#include <list>
#include "ast_node.h"
#include "tac_str.h"
#include "tac_generator.h"

using namespace std;

// Example 1: Building AST for expression "x = a + b"
void example_simple_assignment() {
    cout << "\n=== Example 1: Simple Assignment x = a + b ===" << endl;
    
    // Create operands: a and b
    Expression_Ast* a = new Name_Expr_Ast("a");
    Expression_Ast* b = new Name_Expr_Ast("b");
    
    // Create binary expression: a + b
    Expression_Ast* plus_expr = new Binary_Expr_Ast(a, Binary_Expr_Ast::PLUS_OP, b);
    plus_expr->set_data_type(INT_DATA_TYPE);
    
    // Create assignment: x = (a + b)
    Assignment_Stmt* assign = new Assignment_Stmt("x", plus_expr);
    
    // Print AST
    cout << "AST: ";
    assign->print(stdout);
    cout << endl;
    
    // Generate TAC
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    assign->generate_tac(tac_list);
    
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    // Cleanup
    delete assign;
}

// Example 2: Building AST for arithmetic with multiple operations
void example_complex_arithmetic() {
    cout << "\n=== Example 2: Complex Expression x = a * b + c / d ===" << endl;
    
    // Create operands
    Expression_Ast* a = new Name_Expr_Ast("a");
    Expression_Ast* b = new Name_Expr_Ast("b");
    Expression_Ast* c = new Name_Expr_Ast("c");
    Expression_Ast* d = new Name_Expr_Ast("d");
    
    // Create: a * b
    Expression_Ast* mult_expr = new Binary_Expr_Ast(a, Binary_Expr_Ast::MULT_OP, b);
    mult_expr->set_data_type(INT_DATA_TYPE);
    
    // Create: c / d
    Expression_Ast* div_expr = new Binary_Expr_Ast(c, Binary_Expr_Ast::DIV_OP, d);
    div_expr->set_data_type(INT_DATA_TYPE);
    
    // Create: (a*b) + (c/d)
    Expression_Ast* plus_expr = new Binary_Expr_Ast(mult_expr, Binary_Expr_Ast::PLUS_OP, div_expr);
    plus_expr->set_data_type(INT_DATA_TYPE);
    
    // Create assignment
    Assignment_Stmt* assign = new Assignment_Stmt("x", plus_expr);
    
    // Print and generate TAC
    cout << "AST: ";
    assign->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    assign->generate_tac(tac_list);
    
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete assign;
}

// Example 3: Constant expressions
void example_constants() {
    cout << "\n=== Example 3: Constants ===" << endl;
    
    // Integer constant
    Expression_Ast* int_const = new Const_Expr_Ast(42);
    
    // Float constant
    Expression_Ast* float_const = new Const_Expr_Ast(3.14);
    
    // String constant
    Expression_Ast* str_const = new Const_Expr_Ast("hello");
    
    cout << "Integer: ";
    int_const->print(stdout);
    cout << endl;
    
    cout << "Float: ";
    float_const->print(stdout);
    cout << endl;
    
    cout << "String: ";
    str_const->print(stdout);
    cout << endl;
    
    delete int_const;
    delete float_const;
    delete str_const;
}

// Example 4: Unary operations
void example_unary_operations() {
    cout << "\n=== Example 4: Unary Operations ===" << endl;
    
    // -x
    Expression_Ast* x = new Name_Expr_Ast("x");
    Expression_Ast* uminus = new Unary_Expr_Ast(Unary_Expr_Ast::UMINUS_OP, x);
    uminus->set_data_type(INT_DATA_TYPE);
    
    Assignment_Stmt* assign1 = new Assignment_Stmt("y", uminus);
    
    cout << "AST for y = -x: ";
    assign1->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    assign1->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete assign1;
    
    // !condition
    Expression_Ast* cond = new Name_Expr_Ast("cond");
    Expression_Ast* not_op = new Unary_Expr_Ast(Unary_Expr_Ast::NOT_OP, cond);
    not_op->set_data_type(BOOL_DATA_TYPE);
    
    Assignment_Stmt* assign2 = new Assignment_Stmt("result", not_op);
    
    cout << "\nAST for result = !cond: ";
    assign2->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    tac_list.clear();
    assign2->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete assign2;
}

// Example 5: Relational and logical operations
void example_relational_logical() {
    cout << "\n=== Example 5: Relational and Logical Operations ===" << endl;
    
    // a < b
    Expression_Ast* a = new Name_Expr_Ast("a");
    Expression_Ast* b = new Name_Expr_Ast("b");
    Expression_Ast* lt = new Binary_Expr_Ast(a, Binary_Expr_Ast::LT_OP, b);
    lt->set_data_type(BOOL_DATA_TYPE);
    
    // c > d
    Expression_Ast* c = new Name_Expr_Ast("c");
    Expression_Ast* d = new Name_Expr_Ast("d");
    Expression_Ast* gt = new Binary_Expr_Ast(c, Binary_Expr_Ast::GT_OP, d);
    gt->set_data_type(BOOL_DATA_TYPE);
    
    // (a < b) && (c > d)
    Expression_Ast* and_expr = new Binary_Expr_Ast(lt, Binary_Expr_Ast::AND_OP, gt);
    and_expr->set_data_type(BOOL_DATA_TYPE);
    
    Assignment_Stmt* assign = new Assignment_Stmt("result", and_expr);
    
    cout << "AST for result = (a < b) && (c > d): ";
    assign->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    assign->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete assign;
}

// Example 6: Ternary conditional expression
void example_ternary() {
    cout << "\n=== Example 6: Ternary Expression ===" << endl;
    
    // condition: a > b
    Expression_Ast* a = new Name_Expr_Ast("a");
    Expression_Ast* b = new Name_Expr_Ast("b");
    Expression_Ast* cond = new Binary_Expr_Ast(a, Binary_Expr_Ast::GT_OP, b);
    cond->set_data_type(BOOL_DATA_TYPE);
    
    // true_expr: 10
    Expression_Ast* true_expr = new Const_Expr_Ast(10);
    
    // false_expr: 20
    Expression_Ast* false_expr = new Const_Expr_Ast(20);
    
    // x = a > b ? 10 : 20
    Expression_Ast* ternary = new Ternary_Expr_Ast(cond, true_expr, false_expr);
    ternary->set_data_type(INT_DATA_TYPE);
    
    Assignment_Stmt* assign = new Assignment_Stmt("x", ternary);
    
    cout << "AST for x = (a > b) ? 10 : 20: ";
    assign->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    assign->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete assign;
}

// Example 7: Print and Read statements
void example_io_statements() {
    cout << "\n=== Example 7: I/O Statements ===" << endl;
    
    // write x
    Expression_Ast* x = new Name_Expr_Ast("x");
    Print_Stmt* print_stmt = new Print_Stmt(x);
    
    cout << "AST for write x: ";
    print_stmt->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    print_stmt->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete print_stmt;
    
    // read y
    Read_Stmt* read_stmt = new Read_Stmt("y");
    
    cout << "\nAST for read y: ";
    read_stmt->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    tac_list.clear();
    read_stmt->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete read_stmt;
}

// Example 8: If statement
void example_if_statement() {
    cout << "\n=== Example 8: If Statement ===" << endl;
    
    // Condition: x > 0
    Expression_Ast* x = new Name_Expr_Ast("x");
    Expression_Ast* zero = new Const_Expr_Ast(0);
    Expression_Ast* cond = new Binary_Expr_Ast(x, Binary_Expr_Ast::GT_OP, zero);
    cond->set_data_type(BOOL_DATA_TYPE);
    
    // Then statement: write x
    Expression_Ast* write_x = new Name_Expr_Ast("x");
    Statement_Ast* then_stmt = new Print_Stmt(write_x);
    
    // Else statement: write 0
    Expression_Ast* write_zero = new Const_Expr_Ast(0);
    Statement_Ast* else_stmt = new Print_Stmt(write_zero);
    
    // if (x > 0) write x else write 0
    Statement_Ast* if_stmt = new If_Stmt(cond, then_stmt, else_stmt);
    
    cout << "AST for if (x > 0) write x else write 0: ";
    if_stmt->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    if_stmt->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete if_stmt;
}

// Example 9: While statement
void example_while_statement() {
    cout << "\n=== Example 9: While Statement ===" << endl;
    
    // Condition: x > 0
    Expression_Ast* x = new Name_Expr_Ast("x");
    Expression_Ast* zero = new Const_Expr_Ast(0);
    Expression_Ast* cond = new Binary_Expr_Ast(x, Binary_Expr_Ast::GT_OP, zero);
    cond->set_data_type(BOOL_DATA_TYPE);
    
    // Body: write x
    Expression_Ast* write_x = new Name_Expr_Ast("x");
    Statement_Ast* body = new Print_Stmt(write_x);
    
    // while (x > 0) write x
    Statement_Ast* while_stmt = new While_Stmt(cond, body);
    
    cout << "AST for while (x > 0) write x: ";
    while_stmt->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    while_stmt->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete while_stmt;
}

// Example 10: Compound statement
void example_compound_statement() {
    cout << "\n=== Example 10: Compound Statement ===" << endl;
    
    // Create compound block: { read x; write x; }
    Compound_Stmt* compound = new Compound_Stmt();
    
    // read x
    Statement_Ast* read_stmt = new Read_Stmt("x");
    compound->add_statement(read_stmt);
    
    // write x
    Expression_Ast* x = new Name_Expr_Ast("x");
    Statement_Ast* write_stmt = new Print_Stmt(x);
    compound->add_statement(write_stmt);
    
    cout << "AST for { read x; write x; }:" << endl;
    compound->print(stdout);
    cout << endl;
    
    cout << "TAC:" << endl;
    list<TAC_Stmt*> tac_list;
    compound->generate_tac(tac_list);
    for (auto stmt : tac_list) {
        cout << "  " << stmt->to_string() << endl;
    }
    
    delete compound;
}

// Main function demonstrating all examples
int main() {
    cout << "=== SCLP Class-Based Design Examples ===" << endl;
    
    // Initialize TAC generator
    TAC_Generator* gen = TAC_Generator::get_instance();
    
    example_simple_assignment();
    gen->reset_counters();
    
    example_complex_arithmetic();
    gen->reset_counters();
    
    example_constants();
    gen->reset_counters();
    
    example_unary_operations();
    gen->reset_counters();
    
    example_relational_logical();
    gen->reset_counters();
    
    example_ternary();
    gen->reset_counters();
    
    example_io_statements();
    gen->reset_counters();
    
    example_if_statement();
    gen->reset_counters();
    
    example_while_statement();
    gen->reset_counters();
    
    example_compound_statement();
    gen->reset_counters();
    
    cout << "\n=== All Examples Completed ===" << endl;
    
    return 0;
}
