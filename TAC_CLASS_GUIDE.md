# TAC (Three-Address Code) Implementation Guide

## Overview

This document describes the TAC (Three-Address Code) class hierarchy implemented for the SCLP compiler based on IIT Bombay's specification.

**Reference:** https://www.cse.iitb.ac.in/~uday/sclp-web/

## Class Hierarchy

### TAC_Opd (Operand Classes)

All operands inherit from the abstract base class `TAC_Opd`.

```
TAC_Opd (Base)
├── Const_TAC_Opd      (Constants: 5, 3.14, "hello", true)
├── Var_TAC_Opd        (Variables: x, y, arr)
├── Temp_TAC_Opd       (Temporaries: temp0, temp1, stemp0)
└── Label_TAC_Opd      (Labels: Label0, Label1)
```

#### Const_TAC_Opd - Constants

Represents constant values in the program.

```cpp
// Create constant operands
Const_TAC_Opd *c1 = new Const_TAC_Opd("5", INT_DATA_TYPE);
Const_TAC_Opd *c2 = new Const_TAC_Opd("3.14", FLOAT_DATA_TYPE);
Const_TAC_Opd *c3 = new Const_TAC_Opd("\"hello\"", STRING_DATA_TYPE);

// Printing
c1->print();            // Output: 5
c1->get_name();         // Returns: "5"
c1->get_value();        // Returns: "5"
c1->get_data_type();    // Returns: INT_DATA_TYPE
```

#### Var_TAC_Opd - Variables

Represents variable operands.

```cpp
// Create variable operands
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Var_TAC_Opd *arr = new Var_TAC_Opd("arr", INT_DATA_TYPE);

// Printing
x->print();             // Output: x
x->get_name();          // Returns: "x"
x->get_data_type();     // Returns: INT_DATA_TYPE
```

#### Temp_TAC_Opd - Temporaries

Represents temporary variables generated during TAC generation.

```cpp
// Create temporary operands (using TACGenerator)
Temp_TAC_Opd *t1 = tac_gen.new_temp(INT_DATA_TYPE);      // temp0, temp1, ...
Temp_TAC_Opd *s1 = tac_gen.new_stemp(STRING_DATA_TYPE);  // stemp0, stemp1, ...

// Direct creation
Temp_TAC_Opd *t = new Temp_TAC_Opd(0, INT_DATA_TYPE, "temp");

// Methods
t1->get_id();           // Returns: 0 (for temp0)
t1->get_name();         // Returns: "temp0"
t1->print();            // Output: temp0
t1->get_data_type();    // Returns: INT_DATA_TYPE
```

#### Label_TAC_Opd - Labels

Represents labels used in control flow statements.

```cpp
// Create label operands (using TACGenerator)
Label_TAC_Opd *l1 = tac_gen.new_label();  // Label0, Label1, ...
Label_TAC_Opd *l2 = tac_gen.new_label();  // Label2, ...

// Direct creation
Label_TAC_Opd *lbl = new Label_TAC_Opd(0);

// Methods
l1->get_id();           // Returns: 0 (for Label0)
l1->get_name();         // Returns: "Label0"
l1->print();            // Output: Label0
```

### TAC_Stmt (Statement Classes)

All statements inherit from the abstract base class `TAC_Stmt`.

```
TAC_Stmt (Base)
├── Assign_TAC_Stmt        (temp = a + b, temp = a, temp = -x)
├── Move_TAC_Stmt          (x = temp, x = 10)
├── Label_TAC_Stmt         (Label0:)
├── Goto_TAC_Stmt          (goto Label0)
├── If_Goto_TAC_Stmt       (if(temp) goto Label0)
├── FCall_TAC_Stmt         (call foo(x, y))
├── FCall_Return_TAC_Stmt  (temp = call foo(x, y))
├── Return_TAC_Stmt        (return x, return)
├── Param_TAC_Stmt         (param x, param temp)
├── Print_TAC_Stmt         (print x)
└── Read_TAC_Stmt          (read x)
```

#### Assign_TAC_Stmt - Assignments

Represents binary or unary operations with assignment.

```cpp
// Binary operation: temp0 = x + y
Const_TAC_Opd *x = new Const_TAC_Opd("x", INT_DATA_TYPE);
Const_TAC_Opd *y = new Const_TAC_Opd("y", INT_DATA_TYPE);
Temp_TAC_Opd *t = tac_gen.new_temp(INT_DATA_TYPE);
Assign_TAC_Stmt *s1 = tac_gen.new_assign_stmt(t, x, "+", y);

// Unary operation: temp1 = -x
Assign_TAC_Stmt *s2 = tac_gen.new_assign_stmt(t, "-", x);

// Relational: temp2 = a < b
Assign_TAC_Stmt *s3 = tac_gen.new_assign_stmt(t, a, "<", b);

// Logical: temp3 = !condition
Assign_TAC_Stmt *s4 = tac_gen.new_assign_stmt(t, "!", cond);

// Methods
s1->get_result();       // Returns: temp0
s1->get_opd1();         // Returns: x
s1->get_opd2();         // Returns: y
s1->get_operation();    // Returns: "+"
s1->print();            // Output: temp0 = x + y
```

#### Move_TAC_Stmt - Simple Assignments

Represents simple value assignments without operations.

```cpp
// Simple assignment: x = temp0
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Temp_TAC_Opd *t = new Temp_TAC_Opd(0, INT_DATA_TYPE, "temp");
Move_TAC_Stmt *stmt = tac_gen.new_move_stmt(x, t);

// Methods
stmt->get_result();     // Returns: x
stmt->get_opd();        // Returns: temp0
stmt->print();          // Output: x = temp0
```

#### Label_TAC_Stmt - Label Definition

Represents a label point in the code.

```cpp
// Label definition
Label_TAC_Opd *lbl = tac_gen.new_label();
Label_TAC_Stmt *stmt = tac_gen.new_label_stmt(lbl);

// Methods
stmt->get_label();      // Returns: Label0
stmt->print();          // Output: Label0:
```

#### Goto_TAC_Stmt - Unconditional Jump

Represents unconditional branch statements.

```cpp
// Unconditional goto
Label_TAC_Opd *target = tac_gen.new_label();
Goto_TAC_Stmt *stmt = tac_gen.new_goto_stmt(target);

// Methods
stmt->get_label();      // Returns: target label
stmt->print();          // Output: goto Label0
```

#### If_Goto_TAC_Stmt - Conditional Jump

Represents conditional branch statements.

```cpp
// Conditional jump: if(temp0) goto Label1
Temp_TAC_Opd *cond = new Temp_TAC_Opd(0, BOOL_DATA_TYPE, "temp");
Label_TAC_Opd *target = tac_gen.new_label();
If_Goto_TAC_Stmt *stmt = tac_gen.new_if_goto_stmt(cond, target);

// Negated conditional: if(!temp0) goto Label1
If_Goto_TAC_Stmt *stmt2 = tac_gen.new_if_goto_stmt(cond, target, true);

// Methods
stmt->get_condition();  // Returns: temp0
stmt->get_label();      // Returns: target label
stmt->is_negated();     // Returns: false
stmt->print();          // Output: if(temp0) goto Label0
stmt2->print();         // Output: if(!temp0) goto Label1
```

#### FCall_TAC_Stmt - Function Calls (No Return)

Represents function calls where return value is not used.

```cpp
// Function call without return value
FCall_TAC_Stmt *stmt = new FCall_TAC_Stmt("foo");
stmt->add_argument(new Var_TAC_Opd("x", INT_DATA_TYPE));
stmt->add_argument(new Var_TAC_Opd("y", INT_DATA_TYPE));

// Methods
stmt->get_func_name();      // Returns: "foo"
stmt->get_arguments();      // Returns: list of arguments
stmt->print();              // Output: call foo(x, y)
```

#### FCall_Return_TAC_Stmt - Function Calls (With Return)

Represents function calls with return value assignment.

```cpp
// temp0 = call bar(x)
Temp_TAC_Opd *result = tac_gen.new_temp(INT_DATA_TYPE);
FCall_Return_TAC_Stmt *stmt = new FCall_Return_TAC_Stmt(result, "bar");
stmt->add_argument(new Var_TAC_Opd("x", INT_DATA_TYPE));

// Methods
stmt->get_result();         // Returns: temp0
stmt->get_func_name();      // Returns: "bar"
stmt->get_arguments();      // Returns: list of arguments
stmt->print();              // Output: temp0 = call bar(x)
```

#### Return_TAC_Stmt - Return Statement

Represents function return statements.

```cpp
// return x
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Return_TAC_Stmt *stmt1 = tac_gen.new_return_stmt(x);

// return (void function)
Return_TAC_Stmt *stmt2 = tac_gen.new_return_stmt(NULL);

// Methods
stmt1->get_value();     // Returns: x
stmt1->print();         // Output: return x
stmt2->print();         // Output: return
```

#### Param_TAC_Stmt - Parameter Statement

Represents parameter passing to functions.

```cpp
// param x
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Param_TAC_Stmt *stmt = tac_gen.new_read_stmt(x, 1);

// Methods
stmt->get_value();          // Returns: x
stmt->get_param_count();    // Returns: 1
stmt->print();              // Output: param x
```

#### Print_TAC_Stmt - Print Statement

Represents print/output statements.

```cpp
// print x
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Print_TAC_Stmt *stmt = tac_gen.new_print_stmt(x);

// Methods
stmt->get_value();          // Returns: x
stmt->print();              // Output: print x
```

#### Read_TAC_Stmt - Read Statement

Represents read/input statements.

```cpp
// read x
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Read_TAC_Stmt *stmt = tac_gen.new_read_stmt(x);

// Methods
stmt->get_variable();       // Returns: x
stmt->print();              // Output: read x
```

## TACGenerator Class

The `TACGenerator` class manages the creation of TAC operands and statements.

```cpp
// Global instance
extern TACGenerator tac_gen;

// Creating operands
Temp_TAC_Opd* new_temp(DataType dtype);      // Creates temp0, temp1, ...
Temp_TAC_Opd* new_stemp(DataType dtype);     // Creates stemp0, stemp1, ... (for ternary)
Label_TAC_Opd* new_label();                  // Creates Label0, Label1, ...

// Creating statements
Assign_TAC_Stmt* new_assign_stmt(TAC_Opd *res, TAC_Opd *op1, string op, TAC_Opd *op2);
Assign_TAC_Stmt* new_assign_stmt(TAC_Opd *res, string op, TAC_Opd *op1);  // Unary
Move_TAC_Stmt* new_move_stmt(TAC_Opd *res, TAC_Opd *op);
Label_TAC_Stmt* new_label_stmt(Label_TAC_Opd *lbl);
Goto_TAC_Stmt* new_goto_stmt(Label_TAC_Opd *lbl);
If_Goto_TAC_Stmt* new_if_goto_stmt(TAC_Opd *cond, Label_TAC_Opd *lbl, bool neg = false);
Return_TAC_Stmt* new_return_stmt(TAC_Opd *val = NULL);
Print_TAC_Stmt* new_print_stmt(TAC_Opd *val);
Read_TAC_Stmt* new_read_stmt(Var_TAC_Opd *var);

// Reset counters (for restarting TAC generation)
void reset();
```

## Complete Example

Here's a complete example of generating TAC for an expression: `x = a + b * c`

```cpp
#include "tac_new.h"

int main() {
    // Initialize TAC file for output
    tac_file = stdout;
    
    // Create operands
    Var_TAC_Opd *a = new Var_TAC_Opd("a", INT_DATA_TYPE);
    Var_TAC_Opd *b = new Var_TAC_Opd("b", INT_DATA_TYPE);
    Var_TAC_Opd *c = new Var_TAC_Opd("c", INT_DATA_TYPE);
    Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
    
    // Create TAC statements
    list<TAC_Stmt*> statements;
    
    // temp0 = b * c
    Temp_TAC_Opd *temp0 = tac_gen.new_temp(INT_DATA_TYPE);
    statements.push_back(tac_gen.new_assign_stmt(temp0, b, "*", c));
    
    // temp1 = a + temp0
    Temp_TAC_Opd *temp1 = tac_gen.new_temp(INT_DATA_TYPE);
    statements.push_back(tac_gen.new_assign_stmt(temp1, a, "+", temp0));
    
    // x = temp1
    statements.push_back(tac_gen.new_move_stmt(x, temp1));
    
    // Print TAC
    for (auto stmt : statements) {
        stmt->print();
    }
    
    // Output:
    // temp0 = b * c
    // temp1 = a + temp0
    // x = temp1
    
    return 0;
}
```

## Usage with AST

When integrating TAC generation with AST (in the `generate_tac()` method):

```cpp
// In Expression_Ast::generate_tac()
TAC_Opd* Binary_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    // Generate TAC for left operand
    TAC_Opd *left_opd = left->generate_tac(statements);
    
    // Generate TAC for right operand
    TAC_Opd *right_opd = right->generate_tac(statements);
    
    // Create result temporary
    Temp_TAC_Opd *result = tac_gen.new_temp(get_data_type());
    
    // Create assignment statement
    TAC_Stmt *assign = tac_gen.new_assign_stmt(result, left_opd, "+", right_opd);
    statements.push_back(assign);
    
    return result;
}
```

## Data Types

Supported data types for TAC operands:

```cpp
typedef enum {
    INT_DATA_TYPE,      // int
    FLOAT_DATA_TYPE,    // float
    BOOL_DATA_TYPE,     // bool
    STRING_DATA_TYPE,   // string
    CHAR_DATA_TYPE,     // char
    VOID_DATA_TYPE,     // void
    ERROR_DATA_TYPE,    // error
} DataType;
```

## Notes

1. **Memory Management**: Objects created with `new` should be deleted when no longer needed
2. **TACGenerator**: Use the global `tac_gen` instance for creating temporaries and labels
3. **Data Types**: Always specify the correct data type when creating operands
4. **TAC Lists**: TAC statements are typically stored in a `list<TAC_Stmt*>` for each expression or statement
5. **Printing**: Use `print()` method for displaying TAC or `stmt->print()` for individual statements

## Compilation

To compile code using these classes:

```bash
g++ -c tac_new.cpp
g++ -c your_program.cpp
g++ -o your_program your_program.o tac_new.o ast_new.o
```

## Files

- **tac_new.h** - TAC class definitions
- **tac_new.cpp** - TAC class implementations

---
*Based on SCLP (Sclp: A Language Processor for a Small C-like Language)*
*Reference: https://www.cse.iitb.ac.in/~uday/sclp-web/*
