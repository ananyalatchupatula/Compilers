# Parser Integration Guide - Function Calls and Recursion

This guide shows how to update your Bison parser (`parser_new.y`) to use the new function and recursion support classes.

## Overview

The parser needs to handle:
1. Function declarations with return types and parameters
2. Function definitions with bodies
3. Function calls in expressions
4. Return statements
5. Parameter lists

## Grammar Rules

### Function Declaration

Add to your parser rules:

```bison
/* Function declaration: return_type function_name(parameter_list) { body } */
declaration:
    type IDENTIFIER '(' param_list ')' '{' statement_list '}'
    {
        FunctionDef_Stmt *func = new FunctionDef_Stmt($2, $1);
        
        // Add parameters from param_list (stored in global)
        // (See parameter handling below)
        
        Compound_Stmt *body = new Compound_Stmt();
        // Add statements from statement_list
        func->set_body(body);
        
        // Add to function table
        main_stmt_list.push_back(func);
    }
;

/* For functions with no parameters */
declaration:
    type IDENTIFIER '(' ')' '{' statement_list '}'
    {
        FunctionDef_Stmt *func = new FunctionDef_Stmt($2, $1);
        // ... same as above
    }
;
```

### Parameter List

```bison
param_list:
    type IDENTIFIER
    {
        // Create a parameter list
        param_stack.push_back(make_pair($2, $1));
    }
    | param_list ',' type IDENTIFIER
    {
        param_stack.push_back(make_pair($4, $3));
    }
;

/* Helper to store parameter stack */
%code {
    extern vector<pair<string, DataType>> param_stack;
}
```

### Function Call

Add to your expression rules:

```bison
expression:
    /* ... existing expression rules ... */
    | IDENTIFIER '(' arg_list ')'
    {
        FunctionCall_Expr_Ast *call = new FunctionCall_Expr_Ast($1);
        
        // Add arguments from arg_list
        // (See argument handling below)
        
        /* Determine return type from function table */
        // For now, set a default type or query function symbol table
        call->set_data_type(INT_DATA_TYPE);  // Or look up actual return type
        
        $$ = call;
    }
    | IDENTIFIER '(' ')'
    {
        FunctionCall_Expr_Ast *call = new FunctionCall_Expr_Ast($1);
        call->set_data_type(INT_DATA_TYPE);
        $$ = call;
    }
;
```

### Argument List

```bison
arg_list:
    expression
    {
        // Store first argument in a temporary list
        arg_stack.push_back($1);
    }
    | arg_list ',' expression
    {
        arg_stack.push_back($3);
    }
;

/* Helper to store argument stack */
%code {
    extern vector<Expression_Ast*> arg_stack;
}
```

### Return Statement

Add to your statement rules:

```bison
statement:
    /* ... existing statement rules ... */
    | RETURN expression ';'
    {
        $$ = new Return_Stmt($2);
    }
    | RETURN ';'
    {
        $$ = new Return_Stmt();  // Void return
    }
;
```

## Global Variables for Parser

Add these to your parser or separate header:

```cpp
// For collecting parameters during parsing
vector<pair<string, DataType>> param_stack;

// For collecting arguments during parsing
vector<Expression_Ast*> arg_stack;

// Symbol table for functions
map<string, pair<DataType, vector<DataType>>> function_table;
// Maps: function_name -> (return_type, list_of_param_types)
```

## Example: Fibonacci Function

Here's how a recursive Fibonacci function would be parsed:

```c
int fib(int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}
```

### Grammar Derivation

1. **Function Declaration**
   - Type: `int`
   - Identifier: `fib`
   - Parameters: `n` (int)
   - Body: If statement and return

2. **Function Body**
   - If condition: `n <= 1`
   - Then: `return n`
   - Else: `return fib(n-1) + fib(n-2)`

3. **Function Calls**
   - `fib(n-1)` - FunctionCall_Expr_Ast with one argument
   - `fib(n-2)` - FunctionCall_Expr_Ast with one argument

### Generated AST

```
FunctionDef_Stmt("fib", INT_DATA_TYPE)
├── Parameter: n (INT)
└── Body: Compound_Stmt
    ├── If_Stmt
    │   ├── Condition: n <= 1
    │   └── Then: Return_Stmt(n)
    └── Return_Stmt
        └── Binary_Expr_Ast(+)
            ├── FunctionCall_Expr_Ast("fib", fib(n-1))
            └── FunctionCall_Expr_Ast("fib", fib(n-2))
```

## Parser Action Details

### Collecting Parameters

```cpp
/* After matching a parameter list, add all to function */
{
    for(auto& param : param_stack) {
        func->add_parameter(param.first, param.second);
    }
    param_stack.clear();
}
```

### Collecting Arguments

```cpp
/* After matching an argument list, add all to function call */
{
    for(auto arg : arg_stack) {
        call->add_argument(arg);
    }
    arg_stack.clear();
}
```

### Building Function Body

```cpp
/* After parsing a statement list into function body */
{
    Compound_Stmt *body = new Compound_Stmt();
    // Add statements from statement_list stack
    for(auto stmt : stmt_stack) {
        body->add_stmt(stmt);
    }
    func->set_body(body);
}
```

## Integration with Existing Parser Rules

### Type Rule

Ensure your type rule returns DataType enum:

```bison
type:
    INT { $$ = INT_DATA_TYPE; }
    | FLOAT { $$ = FLOAT_DATA_TYPE; }
    | VOID { $$ = VOID_DATA_TYPE; }
    | BOOL { $$ = BOOL_DATA_TYPE; }
;
```

### Statement List Rule

Make sure statement list collects all statements:

```bison
statement_list:
    statement
    {
        Compound_Stmt *comp = new Compound_Stmt();
        comp->add_stmt($1);
        $$ = comp;
    }
    | statement_list statement
    {
        $1->add_stmt($2);
        $$ = $1;
    }
;
```

## Symbol Table Management

For proper type checking of function calls, maintain a symbol table:

```cpp
class SymbolTable {
private:
    map<string, pair<DataType, vector<DataType>>> functions;
    
public:
    void register_function(string name, DataType ret_type, 
                          vector<DataType> param_types) {
        functions[name] = make_pair(ret_type, param_types);
    }
    
    DataType get_function_return_type(string name) {
        return functions[name].first;
    }
    
    vector<DataType> get_function_param_types(string name) {
        return functions[name].second;
    }
};
```

## Lexer Tokens

Ensure your lexer recognizes these tokens:

```c
// Keywords
RETURN       { return RETURN; }
VOID         { return VOID; }

// Keep existing tokens for:
// - Type keywords (int, float, etc.)
// - Identifiers
// - Numbers
// - Operators and punctuation
```

## Testing the Integration

After updating the parser, test with:

```c
// test_function.c
int add(int a, int b) {
    return a + b;
}

int main() {
    int x;
    int y;
    int z;
    read x;
    read y;
    z = add(x, y);
    write z;
}
```

Generate AST to verify:
```bash
./compiler test_function.c --show-ast
```

Should output:
```
PROCEDURE: add
  Return Type: int
  Formal Parameters:
    a<int>
    b<int>
**BEGIN: Abstract Syntax Tree
  Return (
    Arith: Plus<int>
    L_Opd (Name : a<int>)
    R_Opd (Name : b<int>))
**END: Abstract Syntax Tree

PROCEDURE: main
  Return Type: void
  Formal Parameters:
**BEGIN: Abstract Syntax Tree
  ...
```

## Common Pitfalls

1. **Forgetting to clear stacks**: Always clear param_stack and arg_stack after using them
2. **Type checking**: Verify argument types match parameter types
3. **Return type inference**: Some calls may have unknown return types - handle gracefully
4. **Recursive calls**: Ensure function names are available in symbol table before body parsing
5. **Memory management**: Properly delete AST nodes in error cases

## Debugging

Add debug output to parser:

```cpp
// In parser action for function call
fprintf(stderr, "Parsing function call: %s with %d arguments\n", 
        function_name, arg_count);

// In parser action for function definition
fprintf(stderr, "Defining function: %s with %d parameters\n",
        function_name, param_count);
```

This helps identify parsing issues early.

---

## Summary

The parser integration for function support requires:

✅ New grammar rules for function declarations and calls  
✅ Integration of new AST classes (FunctionDef_Stmt, FunctionCall_Expr_Ast, Return_Stmt)  
✅ Symbol table management for function metadata  
✅ Proper parameter and argument handling  
✅ Type checking for function calls  
✅ Clear cleanup of temporary parsing structures

With these changes, your compiler will fully support function declarations, calls, parameters, return values, and recursion at the L4 level!
