# Quick Start Guide - Using the New Classes

## Getting Started (5 minutes)

### 1. Understanding the Structure

Your code now has:
- **AST Classes** (in `ast_node.h`/`.cpp`) - Represent your program structure
- **TAC Classes** (in `tac_str.h`/`.cpp`) - Represent three-address code
- **TAC Generator** (in `tac_generator.h`/`.cpp`) - Creates temp variables and labels

### 2. Basic Expression Building

```cpp
#include "ast_node.h"
#include "tac_generator.h"

// Create: x = 5
Expression_Ast* five = new Const_Expr_Ast(5);
Statement_Ast* assign = new Assignment_Stmt("x", five);

// Print the AST
assign->print(stdout);
```

### 3. Binary Operations

```cpp
// Create: a + b
Expression_Ast* a = new Name_Expr_Ast("a");
Expression_Ast* b = new Name_Expr_Ast("b");
Expression_Ast* plus = new Binary_Expr_Ast(a, Binary_Expr_Ast::PLUS_OP, b);
plus->set_data_type(INT_DATA_TYPE);

// Now use in assignment: x = a + b
Statement_Ast* assign = new Assignment_Stmt("x", plus);
```

### 4. Generating TAC

```cpp
#include <list>

list<TAC_Stmt*> tac_list;
assign->generate_tac(tac_list);

// Print all TAC statements
for (auto stmt : tac_list) {
    cout << stmt->to_string() << endl;
}
```

## Common Operations

### Expressions

| Operation | Code |
|-----------|------|
| Integer constant | `new Const_Expr_Ast(42)` |
| Float constant | `new Const_Expr_Ast(3.14)` |
| String constant | `new Const_Expr_Ast("text")` |
| Variable x | `new Name_Expr_Ast("x")` |
| -x (negate) | `new Unary_Expr_Ast(UMINUS_OP, x)` |
| !x (not) | `new Unary_Expr_Ast(NOT_OP, x)` |
| a + b | `new Binary_Expr_Ast(a, PLUS_OP, b)` |
| a - b | `new Binary_Expr_Ast(a, MINUS_OP, b)` |
| a * b | `new Binary_Expr_Ast(a, MULT_OP, b)` |
| a / b | `new Binary_Expr_Ast(a, DIV_OP, b)` |
| a < b | `new Binary_Expr_Ast(a, LT_OP, b)` |
| a > b | `new Binary_Expr_Ast(a, GT_OP, b)` |
| a == b | `new Binary_Expr_Ast(a, EQ_OP, b)` |
| a != b | `new Binary_Expr_Ast(a, NE_OP, b)` |
| a && b | `new Binary_Expr_Ast(a, AND_OP, b)` |
| a \|\| b | `new Binary_Expr_Ast(a, OR_OP, b)` |
| a ? b : c | `new Ternary_Expr_Ast(a, b, c)` |

### Statements

| Statement | Code |
|-----------|------|
| x = expr | `new Assignment_Stmt("x", expr)` |
| write expr | `new Print_Stmt(expr)` |
| read x | `new Read_Stmt("x")` |
| if (c) s1; else s2; | `new If_Stmt(c, s1, s2)` |
| while (c) s | `new While_Stmt(c, s)` |
| do s while (c) | `new Do_While_Stmt(s, c)` |
| { s1; s2; ... } | `Compound_Stmt* c = new Compound_Stmt(); c->add_statement(s1); c->add_statement(s2);` |

## Real Example: If Statement

```cpp
#include "ast_node.h"
#include <list>

int main() {
    // Build: if (x > 0) write "positive" else write "negative"
    
    // Condition: x > 0
    Expression_Ast* x = new Name_Expr_Ast("x");
    Expression_Ast* zero = new Const_Expr_Ast(0);
    Expression_Ast* cond = new Binary_Expr_Ast(x, Binary_Expr_Ast::GT_OP, zero);
    cond->set_data_type(BOOL_DATA_TYPE);
    
    // Then: write "positive"
    Expression_Ast* pos_str = new Const_Expr_Ast("positive");
    Statement_Ast* then_stmt = new Print_Stmt(pos_str);
    
    // Else: write "negative"
    Expression_Ast* neg_str = new Const_Expr_Ast("negative");
    Statement_Ast* else_stmt = new Print_Stmt(neg_str);
    
    // Create if statement
    Statement_Ast* if_stmt = new If_Stmt(cond, then_stmt, else_stmt);
    
    // Generate TAC
    list<TAC_Stmt*> tac_list;
    if_stmt->generate_tac(tac_list);
    
    // Print TAC
    for (auto stmt : tac_list) {
        printf("%s\n", stmt->to_string().c_str());
    }
    
    // Clean up
    delete if_stmt;
    
    return 0;
}
```

Output:
```
IF (x) GOTO Label0
PRINT "negative"
GOTO Label1
Label0:
PRINT "positive"
Label1:
```

## Type Management

Always set the data type for expressions:

```cpp
Expression_Ast* expr = new Binary_Expr_Ast(a, Binary_Expr_Ast::PLUS_OP, b);
expr->set_data_type(INT_DATA_TYPE);  // Important!

// Or for relational operations (returns bool)
Expression_Ast* rel = new Binary_Expr_Ast(a, Binary_Expr_Ast::LT_OP, b);
rel->set_data_type(BOOL_DATA_TYPE);
```

Data types available:
```cpp
INT_DATA_TYPE
FLOAT_DATA_TYPE
BOOL_DATA_TYPE
STRING_DATA_TYPE
VOID_DATA_TYPE
```

## Debugging Tips

### Print AST
```cpp
stmt->print(stdout);  // Shows the AST structure
```

### Print TAC
```cpp
for (auto tac_stmt : tac_list) {
    tac_stmt->print(stdout);  // Prints each TAC statement
}
```

### Get String Representation
```cpp
string tac_str = tac_stmt->to_string();  // Get as string
cout << tac_str << endl;
```

### Check Data Types
```cpp
Data_Type t = expr->get_data_type();
if (t == INT_DATA_TYPE) { /* ... */ }
```

## Memory Cleanup

```cpp
// When you're done with an AST node, delete it
delete ast_root;  // This recursively deletes all children

// For TAC statements
for (auto stmt : tac_list) {
    delete stmt;  // Deletes the statement and its operands
}
tac_list.clear();
```

## Integration with Parser

In your parser.y, replace:
```cpp
// OLD
$$ = make_node("Plus", TYPE_INT, $1, $3, NULL);

// NEW
$$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::PLUS_OP, $3);
$$->set_data_type(INT_DATA_TYPE);
```

See **PARSER_UPDATE_EXAMPLES.md** for complete parser conversion guide.

## Compilation

Add these includes to your files:
```cpp
#include "ast_node.h"
#include "tac_str.h"
#include "tac_generator.h"
#include <list>
```

Update Makefile:
```makefile
OBJS = lex.yy.o parser.tab.o ast_node.o tac_str.o tac_generator.o main.o
```

## Testing the Classes

Run the example program:
```bash
g++ -o test_classes EXAMPLE_PROGRAM.cpp ast_node.cpp tac_str.cpp tac_generator.cpp
./test_classes
```

This shows all features in action!

## Quick Troubleshooting

| Issue | Solution |
|-------|----------|
| Undefined reference to `Const_Expr_Ast` | Did you #include "ast_node.h"? |
| Memory errors | Check all `delete` calls; use valgrind |
| TAC missing statements | Check that generate_tac() was called |
| Wrong operator | Check Binary_Expr_Ast::PLUS_OP vs other ops |
| Type errors | Did you set_data_type() on result? |

## Next: Update Your Parser

1. Read **PARSER_UPDATE_EXAMPLES.md** for detailed examples
2. Replace each `make_node()` call with appropriate class constructor
3. Update type constants (TYPE_INT → INT_DATA_TYPE, etc.)
4. Test incrementally - one rule at a time

## More Information

- **IMPLEMENTATION_SUMMARY.md** - Complete overview
- **CLASS_USAGE_GUIDE.md** - Detailed API documentation  
- **PARSER_UPDATE_EXAMPLES.md** - Before/after parser examples
- **EXAMPLE_PROGRAM.cpp** - Runnable code examples
- **REFACTORING_GUIDE.md** - Design philosophy

Happy coding! 🚀
