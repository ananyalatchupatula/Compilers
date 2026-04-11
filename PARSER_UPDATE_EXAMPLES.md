# Parser Update Examples

## How to Update parser.y to Use New Classes

### Example 1: Binary Expression in Parser

**OLD CODE (before):**
```cpp
%token PLUS MINUS MULT DIV

expression:
    expression PLUS expression {
        // Create old-style ASTNode
        $$ = make_node("Plus", TYPE_INT, $1, $3, NULL);
    }
    | expression MINUS expression {
        $$ = make_node("Minus", TYPE_INT, $1, $3, NULL);
    }
```

**NEW CODE (after):**
```cpp
expression:
    expression PLUS expression {
        // Create new-style Binary_Expr_Ast
        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::PLUS_OP, $3);
        $$->set_data_type(INT_DATA_TYPE);  // or determine from operands
    }
    | expression MINUS expression {
        $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::MINUS_OP, $3);
        $$->set_data_type(INT_DATA_TYPE);
    }
```

### Example 2: Variable/Constant Expressions

**OLD CODE:**
```cpp
expression:
    INTEGER_NUMBER {
        ASTNode* node = make_node("Num", TYPE_INT, NULL, NULL, NULL);
        $$ = node;
    }
    | NAME {
        ASTNode* node = make_node("Name", TYPE_INT, NULL, NULL, NULL);
        $$ = node;
    }
```

**NEW CODE:**
```cpp
expression:
    INTEGER_NUMBER {
        $$ = new Const_Expr_Ast($1);  // Automatically sets INT_DATA_TYPE
    }
    | DOUBLE_NUMBER {
        $$ = new Const_Expr_Ast($1);  // Automatically sets FLOAT_DATA_TYPE
    }
    | NAME {
        $$ = new Name_Expr_Ast($1);   // Variable reference
    }
    | STRING_CONSTANT {
        $$ = new Const_Expr_Ast($1);  // String constant
    }
```

### Example 3: Assignment Statement

**OLD CODE:**
```cpp
statement:
    NAME ASSIGN expression SEMICOLON {
        // Create assignment node
        $$ = make_node("Asgn:", TYPE_VOID, $3, NULL, NULL);
    }
```

**NEW CODE:**
```cpp
statement:
    NAME ASSIGN expression SEMICOLON {
        // Create assignment statement
        $$ = new Assignment_Stmt($1, $3);
    }
```

### Example 4: If Statement

**OLD CODE:**
```cpp
if_statement:
    IF '(' expression ')' statement {
        $$ = make_node("If:", TYPE_VOID, $3, $5, NULL);
    }
    | IF '(' expression ')' statement ELSE statement {
        ASTNode* if_node = make_node("If:", TYPE_VOID, $3, $5, NULL);
        if_node->third = $7;  // Add else part
        $$ = if_node;
    }
```

**NEW CODE:**
```cpp
if_statement:
    IF '(' expression ')' statement {
        // Create if statement without else
        $$ = new If_Stmt($3, $5, NULL);
    }
    | IF '(' expression ')' statement ELSE statement {
        // Create if statement with else
        $$ = new If_Stmt($3, $5, $7);
    }
```

### Example 5: While Loop

**OLD CODE:**
```cpp
while_statement:
    WHILE '(' expression ')' statement {
        $$ = make_node("While:", TYPE_VOID, $3, $5, NULL);
    }
```

**NEW CODE:**
```cpp
while_statement:
    WHILE '(' expression ')' statement {
        $$ = new While_Stmt($3, $5);
    }
```

### Example 6: Unary Operations

**OLD CODE:**
```cpp
expression:
    MINUS expression %prec UMINUS {
        $$ = make_node("Uminus:", TYPE_INT, $2, NULL, NULL);
    }
    | NOT expression {
        $$ = make_node("NOT:", TYPE_BOOL, $2, NULL, NULL);
    }
```

**NEW CODE:**
```cpp
expression:
    MINUS expression %prec UMINUS {
        $$ = new Unary_Expr_Ast(Unary_Expr_Ast::UMINUS_OP, $2);
    }
    | NOT expression {
        $$ = new Unary_Expr_Ast(Unary_Expr_Ast::NOT_OP, $2);
    }
```

### Example 7: Ternary Operator

**OLD CODE:**
```cpp
expression:
    expression QUESTION expression COLON expression {
        ASTNode* ternary = make_node("Ternary:", TYPE_INT, $1, $3, NULL);
        ternary->third = $5;
        $$ = ternary;
    }
```

**NEW CODE:**
```cpp
expression:
    expression QUESTION expression COLON expression {
        $$ = new Ternary_Expr_Ast($1, $3, $5);
    }
```

### Example 8: Print Statement

**OLD CODE:**
```cpp
statement:
    WRITE expression SEMICOLON {
        $$ = make_node("Write:", TYPE_VOID, $2, NULL, NULL);
    }
```

**NEW CODE:**
```cpp
statement:
    WRITE expression SEMICOLON {
        $$ = new Print_Stmt($2);
    }
```

### Example 9: Read Statement

**OLD CODE:**
```cpp
statement:
    READ NAME SEMICOLON {
        ASTNode* node = make_node("Read:", TYPE_VOID, NULL, NULL, NULL);
        // Had to store variable name separately somehow
        $$ = node;
    }
```

**NEW CODE:**
```cpp
statement:
    READ NAME SEMICOLON {
        $$ = new Read_Stmt($2);
    }
```

### Example 10: Compound Statement

**OLD CODE:**
```cpp
compound_statement:
    '{' statement_list '}' {
        // $2 would be a chain of statements linked via ->right
        $$ = $2;
    }

statement_list:
    /* empty */ { $$ = NULL; }
    | statement_list statement {
        if ($1 == NULL) {
            $$ = $2;
        } else {
            ASTNode* curr = $1;
            while (curr->right) curr = curr->right;
            curr->right = $2;
            $$ = $1;
        }
    }
```

**NEW CODE:**
```cpp
compound_statement:
    '{' statement_list '}' {
        $$ = $2;
    }

statement_list:
    /* empty */ { 
        $$ = new Compound_Stmt();
    }
    | statement_list statement {
        Compound_Stmt* compound = (Compound_Stmt*)$1;
        compound->add_statement($2);
        $$ = compound;
    }
```

## Type Mapping

Map your old TYPE_* constants to new Data_Type enum:

```cpp
// OLD CODE
#define TYPE_INT    1
#define TYPE_FLOAT  2
#define TYPE_STRING 3
#define TYPE_BOOL   4

// NEW CODE
enum Data_Type {
    INT_DATA_TYPE = 0,      // Instead of TYPE_INT
    FLOAT_DATA_TYPE = 1,    // Instead of TYPE_FLOAT
    BOOL_DATA_TYPE = 2,     // Instead of TYPE_BOOL
    STRING_DATA_TYPE = 3,   // Instead of TYPE_STRING
    VOID_DATA_TYPE = 4,
};
```

Update type checking in parser:
```cpp
// OLD
if (lookup_type($1) == TYPE_INT) { ... }

// NEW
if (lookup_type($1) == INT_DATA_TYPE) { ... }
```

## Printing and Debugging

**OLD CODE:**
```cpp
print_ast(root, 0);  // Function taking ASTNode*
```

**NEW CODE:**
```cpp
root->print(stdout);  // Virtual method on Ast*
```

Or for TAC:
```cpp
for (auto stmt : tac_list) {
    stmt->print(stdout);  // Print each TAC statement
}
```

## Key Changes Summary

1. **Replace make_node() calls** with appropriate class constructors
2. **Replace old TYPE_* constants** with Data_Type enum
3. **Use set_data_type()** to set types after construction if needed
4. **Delete left/right/third pointer manipulation** - use proper constructors
5. **TAC generation** now happens via virtual generate_tac() method
6. **No more label strings** - use actual class hierarchy

## Compilation with New Classes

Add to your Makefile or compiler flags:
```cpp
#include "ast_node.h"
#include "tac_str.h"
#include "tac_generator.h"
```

Don't forget to link the new .o files:
```makefile
OBJS = lex.yy.o parser.tab.o ast_node.o tac_str.o tac_generator.o main.o
```
