# SCLP Class-Based Refactoring - Complete Usage Guide

## New Class Hierarchy Overview

Your refactored code now follows the SCLP website's object-oriented design with proper class hierarchies.

### File Structure

```
ast_node.h          - Base AST classes (Ast, Expression_Ast, Statement_Ast)
ast_node.cpp        - Implementation of AST classes

tac_str.h           - TAC statement and operand classes
tac_str.cpp         - Implementation of TAC classes

tac_generator.h     - TAC generator (singleton pattern)
tac_generator.cpp   - Implementation of TAC generator
```

## AST Class Hierarchy

### Expression Classes

#### 1. Binary Expressions (arithmetic, relational, logical)
```cpp
// Arithmetic: a + b, a - b, a * b, a / b
Binary_Expr_Ast* expr = new Binary_Expr_Ast(lhs, Binary_Expr_Ast::PLUS_OP, rhs);

// Relational: a < b, a == b, etc.
Binary_Expr_Ast* expr = new Binary_Expr_Ast(lhs, Binary_Expr_Ast::LT_OP, rhs);

// Logical: a && b, a || b
Binary_Expr_Ast* expr = new Binary_Expr_Ast(lhs, Binary_Expr_Ast::AND_OP, rhs);
```

#### 2. Unary Expressions
```cpp
// Unary minus: -a
Unary_Expr_Ast* expr = new Unary_Expr_Ast(Unary_Expr_Ast::UMINUS_OP, operand);

// Logical NOT: !a
Unary_Expr_Ast* expr = new Unary_Expr_Ast(Unary_Expr_Ast::NOT_OP, operand);
```

#### 3. Ternary Expressions
```cpp
// Conditional: a ? b : c
Ternary_Expr_Ast* expr = new Ternary_Expr_Ast(condition, true_expr, false_expr);
```

#### 4. Constant Expressions
```cpp
// Integer constant
Const_Expr_Ast* num = new Const_Expr_Ast(42);

// Float constant
Const_Expr_Ast* num = new Const_Expr_Ast(3.14);

// String constant
Const_Expr_Ast* str = new Const_Expr_Ast("hello");
```

#### 5. Variable/Name Expressions
```cpp
// Variable reference
Name_Expr_Ast* var = new Name_Expr_Ast("x");
```

### Statement Classes

#### 1. Assignment Statement
```cpp
// x = expression
Assignment_Stmt* stmt = new Assignment_Stmt("x", expr);
```

#### 2. Print Statement
```cpp
// write expression
Print_Stmt* stmt = new Print_Stmt(expr);
```

#### 3. Read Statement
```cpp
// read x
Read_Stmt* stmt = new Read_Stmt("x");
```

#### 4. If Statement
```cpp
// if (condition) then_stmt
If_Stmt* stmt = new If_Stmt(condition, then_stmt);

// if (condition) then_stmt else else_stmt
If_Stmt* stmt = new If_Stmt(condition, then_stmt, else_stmt);
```

#### 5. While Statement
```cpp
// while (condition) body
While_Stmt* stmt = new While_Stmt(condition, body);
```

#### 6. Do-While Statement
```cpp
// do body while (condition)
Do_While_Stmt* stmt = new Do_While_Stmt(body, condition);
```

#### 7. Compound Statement
```cpp
// { stmt1; stmt2; ... }
Compound_Stmt* compound = new Compound_Stmt();
compound->add_statement(stmt1);
compound->add_statement(stmt2);
```

## TAC Class Hierarchy

### TAC Operands

#### 1. Constant Operand
```cpp
// Integer constant
TAC_Opd* opd = new Const_TAC_Opd(42);

// Float constant
TAC_Opd* opd = new Const_TAC_Opd(3.14);

// String constant
TAC_Opd* opd = new Const_TAC_Opd("hello");
```

#### 2. Variable Operand
```cpp
// Variable x
TAC_Opd* opd = new Var_TAC_Opd("x");
```

#### 3. Temporary Variable Operand
```cpp
// Created by TAC generator
TAC_Generator* gen = TAC_Generator::get_instance();
TAC_Opd* temp = gen->create_new_temp();        // integer temp
TAC_Opd* temp = gen->create_new_temp(true);    // float temp
```

#### 4. Label Operand
```cpp
// Created by TAC generator
TAC_Generator* gen = TAC_Generator::get_instance();
Label_TAC_Opd* label = gen->create_new_label();
```

### TAC Statements

#### 1. Assignment Statement
```cpp
// x = temp0
TAC_Stmt* stmt = new Assign_TAC_Stmt(lhs, rhs);
```

#### 2. Compute Statement
```cpp
// temp0 = a + b
TAC_Stmt* stmt = new Compute_TAC_Stmt(result, opd1, Compute_TAC_Stmt::PLUS, opd2);

// temp1 = -temp0 (unary)
TAC_Stmt* stmt = new Compute_TAC_Stmt(result, opd1, Compute_TAC_Stmt::UMINUS);
```

#### 3. Label Statement
```cpp
// Label0:
TAC_Stmt* stmt = new Label_TAC_Stmt(label);
```

#### 4. Goto Statement
```cpp
// GOTO Label1
TAC_Stmt* stmt = new Goto_TAC_Stmt(label);
```

#### 5. Conditional Goto Statement
```cpp
// IF (cond) GOTO Label1
TAC_Stmt* stmt = new Cond_Goto_TAC_Stmt(cond, label);
```

#### 6. Print Statement
```cpp
// PRINT x
TAC_Stmt* stmt = new Print_TAC_Stmt(opd);
```

#### 7. Read Statement
```cpp
// READ x
TAC_Stmt* stmt = new Read_TAC_Stmt(var);
```

## TAC Generation Process

### Example: Generating TAC for an Expression

```cpp
// For expression: x = a + b * c

// Create AST
Expression_Ast* c_expr = new Name_Expr_Ast("c");
Expression_Ast* b_expr = new Name_Expr_Ast("b");
Expression_Ast* mult_expr = new Binary_Expr_Ast(b_expr, Binary_Expr_Ast::MULT_OP, c_expr);

Expression_Ast* a_expr = new Name_Expr_Ast("a");
Expression_Ast* plus_expr = new Binary_Expr_Ast(a_expr, Binary_Expr_Ast::PLUS_OP, mult_expr);

Assignment_Stmt* assign = new Assignment_Stmt("x", plus_expr);

// Generate TAC
list<TAC_Stmt*> tac_list;
assign->generate_tac(tac_list);

// Print TAC
for (auto stmt : tac_list) {
    stmt->print(stdout);
}
```

This generates:
```
temp0 = b * c
temp1 = a + temp0
x = temp1
```

## Key Design Patterns

### 1. Virtual Methods
All AST and TAC classes use virtual methods:
- `print()` - for debugging/output
- `generate_tac()` - for TAC generation (Expression/Statement_Ast only)
- `to_string()` - for TAC representation

### 2. Polymorphism
Parent class pointers can point to child class objects:
```cpp
Expression_Ast* expr;  // Can point to Binary_Expr_Ast, Unary_Expr_Ast, etc.
Statement_Ast* stmt;   // Can point to Assignment_Stmt, If_Stmt, etc.
TAC_Stmt* tac_stmt;    // Can point to Compute_TAC_Stmt, Label_TAC_Stmt, etc.
TAC_Opd* opd;          // Can point to Const_TAC_Opd, Var_TAC_Opd, etc.
```

### 3. Singleton Pattern
The TAC_Generator uses singleton pattern for global access:
```cpp
TAC_Generator* gen = TAC_Generator::get_instance();
TAC_Opd* temp = gen->create_new_temp();
Label_TAC_Opd* label = gen->create_new_label();
```

## Migration from Old Code to New Code

### Old Code (procedural approach):
```cpp
ASTNode* node = make_node("Plus", TYPE_INT, left, right, NULL);
void generate_TAC(ASTNode *root) { ... }
```

### New Code (OOP approach):
```cpp
Expression_Ast* expr = new Binary_Expr_Ast(left, Binary_Expr_Ast::PLUS_OP, right);
list<TAC_Stmt*> tac_list;
expr->generate_tac(tac_list);
```

## Memory Management

All classes use proper `delete` in destructors:
```cpp
Binary_Expr_Ast::~Binary_Expr_Ast() {
    if (lhs) delete lhs;
    if (rhs) delete rhs;
}
```

When you're done with AST/TAC, clean up:
```cpp
delete ast_root;  // Recursively deletes all children
```

## Compilation

Add the new files to your Makefile:
```makefile
SRCS = scanner.l parser.y ast_node.cpp tac_str.cpp tac_generator.cpp main.cpp
OBJS = lex.yy.o parser.tab.o ast_node.o tac_str.o tac_generator.o main.o
```

## Next Steps

1. Update `parser.y` to create new AST classes instead of ASTNode structs
2. Update TAC generation in `parser.y` to use the new TAC classes
3. Remove old `ast.h`, `ast.cpp`, `tac.h`, `tac.cpp` files after migration complete
4. Test with existing test cases to ensure compatibility
