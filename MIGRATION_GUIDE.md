# Complete Migration Guide: Old AST → New Class-Based AST

## Overview

This guide shows the complete transformation from your procedural AST (using generic `ASTNode` struct) to a proper object-oriented class hierarchy following the SCLP website design patterns.

---

## Part 1: Changes to Core Files

### 1.1 Header Files

#### Before: `ast.h`
```cpp
struct ASTNode {
    char label[100];      // Generic string label
    DataType type;
    ASTNode *left;
    ASTNode *right;
    ASTNode *third;
    int temp_id;          // Temp allocation tracking
    int stemp_id;
    int not_temp_id;
};

ASTNode* make_node(const char *label, DataType type,
                   ASTNode *l, ASTNode *r, ASTNode *t);
```

#### After: `ast_new.h`
```cpp
class Ast {  // Base class with virtual methods
public:
    DataType node_data_type;
    int temp_id, stemp_id, not_temp_id;
    
    virtual void print(int indent = 0) = 0;
    virtual TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) = 0;
};

class Expression_Ast : public Ast {  // For expressions
public:
    // Pure virtual - subclassed by specific expressions
};

class Statement_Ast : public Ast {   // For statements
public:
    // Pure virtual - subclassed by specific statements
};

// Specific expression classes:
class Binary_Expr_Ast : public Expression_Ast {
public:
    enum BinaryOp { PLUS_OP, MINUS_OP, MULT_OP, ... };
    Binary_Expr_Ast(Expression_Ast* l, BinaryOp op, Expression_Ast* r);
    void print(int indent = 0) override;
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) override;
};

// Specific statement classes:
class Assignment_Stmt : public Statement_Ast {
public:
    Assignment_Stmt(string var_name, Expression_Ast* rhs);
    void print(int indent = 0) override;
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) override;
};
```

**Key Differences:**
- ❌ No generic `ASTNode` struct with string labels
- ✅ Separate classes for different node types
- ✅ Virtual methods for polymorphic behavior
- ✅ Type-safe constructors instead of `make_node()`

---

## Part 2: Parser Changes

### 2.1 Expression Parsing

#### Before: Using `make_node()`
```bison
expr PLUS expr
{
    // Create wrapper nodes and chains them manually
    ASTNode* closeL = make_node(")", TYPE_VOID, NULL, NULL, NULL);
    ASTNode* tempL = $1;
    while(tempL->right) tempL = tempL->right;
    tempL->right = closeL;
    
    ASTNode* L = make_node("L_Opd (", $1->type, $1, NULL, NULL);
    
    // ... similar for right operand ...
    
    char buf[128];
    snprintf(buf, sizeof(buf), "Arith: Plus<%s>", type_to_string(resultType));
    $$ = make_node(buf, resultType, L, R, NULL);
}
```

#### After: Using Class Constructors
```bison
expr PLUS expr
{
    if(!isNumeric(datatype_to_int($1->get_data_type())) || 
       !isNumeric(datatype_to_int($3->get_data_type()))) {
        cout << "Semantic error: operands must be numeric" << endl;
        exit(1);
    }

    int result_type = numericResult(
        datatype_to_int($1->get_data_type()),
        datatype_to_int($3->get_data_type())
    );

    $$ = new Binary_Expr_Ast($1, Binary_Expr_Ast::PLUS_OP, $3);
    $$->set_data_type(int_to_datatype(result_type));
}
```

**Improvements:**
- ✅ No manual node wrapping and chaining
- ✅ Cleaner constructor-based creation
- ✅ Type checking integrated naturally
- ✅ Result type calculation simplified

### 2.2 Statement Parsing

#### Before: Using `make_node()`
```bison
assign_stmt
    : NAME ASSIGN_OP expr
{
    // Create multiple wrapper nodes
    ASTNode* lhsNode = make_node(...);
    ASTNode* closeLHS = make_node(")", ...);
    ASTNode* lhsWrapper = make_node("L_Opd (", ...);
    
    ASTNode* closeRHS = make_node(")", ...);
    ASTNode* rhsWrapper = make_node("RHS", ...);
    
    ASTNode* asgnNode = make_node("Asgn:", ...);
    // Complex chaining...
    $$ = asgnNode;
}
```

#### After: Using Class Constructor
```bison
assign_stmt
    : NAME ASSIGN_OP expr
{
    int lhs_type = lookup($1);
    int rhs_type = datatype_to_int($3->get_data_type());

    if(lhs_type == TYPE_ERROR || rhs_type == TYPE_ERROR) {
        cout << "Semantic error: type error in assignment" << endl;
        exit(1);
    }

    if(lhs_type == TYPE_FLOAT && rhs_type == TYPE_INT) {
        /* Implicit conversion allowed */
    } else if(lhs_type != rhs_type) {
        cout << "Semantic error: type mismatch in assignment" << endl;
        exit(1);
    }

    $$ = new Assignment_Stmt($1, $3);
    $$->set_data_type(int_to_datatype(lhs_type));
}
```

**Improvements:**
- ✅ Single clean constructor call
- ✅ No wrapper nodes needed
- ✅ Type checking explicit and obvious
- ✅ Much shorter and readable

### 2.3 Read/Write Statements

#### Before:
```bison
read_stmt
    : READ NAME
    ;

write_stmt
    : WRITE expr
{
    if($2->type == TYPE_BOOL){
        // error handling...
    }
    $$ = make_node(...);
}
```

#### After:
```bison
read_stmt
    : READ NAME
{
    int var_type = lookup($2);
    $$ = new Read_Stmt($2);
    $$->set_data_type(int_to_datatype(var_type));
}
;

write_stmt
    : WRITE expr
{
    if($2->get_data_type() == BOOL_DATA_TYPE){
        cout << "Semantic error: cannot print bool" << endl;
        exit(1);
    }

    $$ = new Print_Stmt($2);
}
```

**Improvements:**
- ✅ Direct class instantiation
- ✅ Better naming (Print_Stmt vs generic make_node)
- ✅ Type getters are explicit

### 2.4 Compound Statements

#### Before:
```bison
stmt_list
    : stmt_list stmt
{
    if($1==NULL) $$=$2;
    else {
        ASTNode* t=$1;
        while(t->right) t=t->right;
        t->right=$2;
        $$=$1;
    }
}
```

#### After:
```bison
stmt_list
    : stmt_list stmt
{
    if($1 == NULL) {
        $$ = new Compound_Stmt();
        if($2) $$->add_stmt($2);
    } else {
        $$->add_stmt($2);
    }
}
    | /* empty */
{
    $$ = new Compound_Stmt();
}
```

**Improvements:**
- ✅ Proper compound statement class
- ✅ Add statement method instead of pointer chaining
- ✅ Cleaner semantics

---

## Part 3: TAC Integration

### 3.1 Old TAC Generation (Procedural)

In your old code:
```cpp
string gen_expr(ASTNode *node){
    // Manual traversal
    string left = gen_expr(node->left);
    string right = gen_expr(node->right);
    
    // Manual temp allocation
    string temp = get_temp_name(node);
    if (temp.empty()) {
        temp = tac_gen.new_temp();
    }
    
    // Manual code generation
    fprintf(tac_file, "%s = %s + %s\n", temp.c_str(), left.c_str(), right.c_str());
    return temp;
}
```

### 3.2 New TAC Generation (Object-Oriented)

```cpp
class Binary_Expr_Ast : public Expression_Ast {
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) {
        // Recursive TAC generation
        TAC_Opd* left_tac = left->generate_tac(statements);
        TAC_Opd* right_tac = right->generate_tac(statements);
        
        // Allocate temp
        if(temp_id == -1) {
            temp_id = TAC_Generator::get_instance().create_new_temp();
        }
        Temp_TAC_Opd* result = new Temp_TAC_Opd(temp_id, node_data_type);
        
        // Add TAC statement to list
        statements.push_back(new Compute_TAC_Stmt(result, left_tac, right_tac, op_str));
        
        return result;
    }
};
```

**Benefits:**
- ✅ TAC generation integrated into each class
- ✅ Polymorphic behavior via virtual methods
- ✅ No global state (tac_gen is singleton)
- ✅ Cleaner separation of concerns

---

## Part 4: Type System Changes

### Before: Procedural with #define
```cpp
#define TYPE_INT    1
#define TYPE_FLOAT  2
#define TYPE_STRING 3

int get_type(ASTNode* node) {
    // Parse string label
    if(strstr(node->label, "<int>")) return TYPE_INT;
    if(strstr(node->label, "<float>")) return TYPE_FLOAT;
    // ...
}
```

### After: Enum-based with Helper Functions
```cpp
typedef enum {
    INT_DATA_TYPE,
    FLOAT_DATA_TYPE,
    BOOL_DATA_TYPE,
    STRING_DATA_TYPE,
    CHAR_DATA_TYPE,
    VOID_DATA_TYPE,
    ERROR_DATA_TYPE,
} DataType;

// Helper functions for conversion
DataType int_to_datatype(int t) {
    switch(t) {
        case TYPE_INT:    return INT_DATA_TYPE;
        case TYPE_FLOAT:  return FLOAT_DATA_TYPE;
        // ...
    }
}

int datatype_to_int(DataType t) {
    switch(t) {
        case INT_DATA_TYPE:    return TYPE_INT;
        case FLOAT_DATA_TYPE:  return TYPE_FLOAT;
        // ...
    }
}
```

**Benefits:**
- ✅ Type-safe enum instead of string parsing
- ✅ Compile-time type checking
- ✅ Clear type hierarchy
- ✅ Easier conversions between formats

---

## Part 5: Memory Management

### Before: Manual with malloc/new inconsistency
```cpp
ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));  // malloc
// vs
ASTNode* node = new ASTNode();  // new

// No systematic cleanup
```

### After: Consistent with Destructors
```cpp
class Binary_Expr_Ast : public Expression_Ast {
private:
    Expression_Ast* left;
    Expression_Ast* right;
    
public:
    ~Binary_Expr_Ast() {
        delete left;    // Proper cleanup
        delete right;
    }
};

class Compound_Stmt : public Statement_Ast {
private:
    list<Statement_Ast*> statements;
    
public:
    ~Compound_Stmt() {
        for(auto stmt : statements) {
            delete stmt;  // Clean all statements
        }
        statements.clear();
    }
};
```

**Benefits:**
- ✅ Virtual destructors for proper cleanup
- ✅ Consistent use of `new`/`delete`
- ✅ Automatic cleanup via destructors
- ✅ Prevents memory leaks

---

## Part 6: Printing/Debugging

### Before: Generic Label-based Printing
```cpp
void print_ast(ASTNode *n, int indent) {
    // Parse label to determine type
    if(strstr(n->label, "Plus")) {
        printf("Binary Plus\n");
    } else if(strstr(n->label, "Minus")) {
        printf("Binary Minus\n");
    }
    // ... dozens of string comparisons
}
```

### After: Polymorphic Print Methods
```cpp
class Binary_Expr_Ast : public Expression_Ast {
    void print(int indent = 0) {
        for(int i = 0; i < indent; i++) printf("  ");
        
        const char* op_str = "";
        switch(op) {
            case PLUS_OP:  op_str = "+"; break;
            case MINUS_OP: op_str = "-"; break;
            // ...
        }
        
        printf("BinOp<%s>: %s\n", 
               data_type_to_string(node_data_type), op_str);
        left->print(indent + 1);
        right->print(indent + 1);
    }
};
```

**Benefits:**
- ✅ Each class knows how to print itself
- ✅ No string parsing needed
- ✅ Type-safe via enums
- ✅ Easy to extend with new node types

---

## Part 7: Complete Example

### Old Code Flow
```
parser.y (make_node calls)
    ↓
ast.cpp (generic string-based printing)
    ↓
tac.cpp (procedural gen_expr, gen_assign, etc.)
    ↓
Output: TAC statements
```

### New Code Flow
```
parser_new.y (class constructors)
    ↓
ast_new.h/cpp (virtual methods dispatch)
    ↓
Expression_Ast::generate_tac() → creates TAC_Stmt objects
    ↓
list<TAC_Stmt*> (proper C++ containers)
    ↓
TAC_Stmt::print() (polymorphic output)
    ↓
Output: TAC statements
```

---

## Part 8: Quick Reference for Updates

### Replace in Parser

| Old | New |
|-----|-----|
| `ASTNode* node = make_node(...)` | `Ast* node = new SpecificClass(...)` |
| `node->label` (string parsing) | Class-specific properties |
| `node->type` (generic type) | `node->get_data_type()` |
| `print_ast(node)` | `node->print()` |
| `generate_TAC(node)` | `node->generate_tac(statements_list)` |
| Global `tac_gen` | `TAC_Generator::get_instance()` |

### Class Creation Examples

```cpp
// Constants
new Const_Expr_Ast("42", INT_DATA_TYPE)
new Const_Expr_Ast("3.14", FLOAT_DATA_TYPE)

// Variables
new Name_Expr_Ast("x")

// Binary operations
new Binary_Expr_Ast(left, Binary_Expr_Ast::PLUS_OP, right)
new Binary_Expr_Ast(left, Binary_Expr_Ast::LT_OP, right)

// Unary operations
new Unary_Expr_Ast(Unary_Expr_Ast::NOT_OP, expr)
new Unary_Expr_Ast(Unary_Expr_Ast::UMINUS_OP, expr)

// Ternary
new Ternary_Expr_Ast(cond, true_expr, false_expr)

// Statements
new Assignment_Stmt("x", rhs_expr)
new Read_Stmt("x")
new Print_Stmt(expr)
new If_Stmt(cond, then_stmt, else_stmt)
new While_Stmt(cond, body)
new Compound_Stmt()
```

---

## Part 9: Migration Checklist

- [ ] Replace `#include "ast.h"` with `#include "ast_new.h"`
- [ ] Update all `make_node()` calls to class constructors
- [ ] Replace generic `ASTNode*` with specific `Expression_Ast*` or `Statement_Ast*`
- [ ] Update type checking to use `get_data_type()` method
- [ ] Replace string label parsing with enum-based types
- [ ] Update `stmt_list` to use `Compound_Stmt`
- [ ] Ensure all destructors are called properly
- [ ] Test with existing test cases
- [ ] Update Makefile to compile `ast_new.cpp` instead of `ast.cpp`
- [ ] Verify TAC output matches old implementation

---

## Part 10: Compilation

### Old Makefile
```makefile
compiler: main.cpp parser.tab.o lex.yy.o ast.o tac.o
	g++ -std=c++11 -o compiler main.cpp parser.tab.o lex.yy.o ast.o tac.o
```

### New Makefile
```makefile
compiler: main.cpp parser.tab.o lex.yy.o ast_new.o tac_str.o tac_generator.o
	g++ -std=c++11 -o compiler main.cpp parser.tab.o lex.yy.o ast_new.o tac_str.o tac_generator.o

ast_new.o: ast_new.cpp ast_new.h
	g++ -std=c++11 -c ast_new.cpp

tac_str.o: tac_str.cpp tac_str.h
	g++ -std=c++11 -c tac_str.cpp

tac_generator.o: tac_generator.cpp tac_generator.h
	g++ -std=c++11 -c tac_generator.cpp
```

---

## Summary

The new class-based AST provides:

✅ **Type Safety** - Enums instead of string parsing  
✅ **Extensibility** - Easy to add new node types  
✅ **Maintainability** - Clear class hierarchy  
✅ **Performance** - Virtual dispatch vs string comparison  
✅ **Memory Safety** - Proper destructors  
✅ **Code Clarity** - No generic node wrapping  

This matches the SCLP website's professional compiler architecture pattern.
