# TAC Integration with AST Guide

## Overview

This guide shows how to integrate TAC generation into your AST `generate_tac()` methods.

## Integration Pattern

Each AST node should have a `generate_tac()` method that:
1. Recursively generates TAC for child nodes
2. Creates intermediate operations as needed
3. Returns a TAC_Opd representing the result
4. Appends statements to the provided list

---

## AST Class Integration Template

### Basic Expression Pattern

```cpp
// In ast_new.h - Add method declaration
class Expression_Ast : public Ast {
    // ... existing code ...
    virtual TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) = 0;
};

// In ast_new.cpp - Implement in each expression class
TAC_Opd* My_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    // Generate TAC for this expression
    // Return TAC_Opd representing the result
}
```

---

## Expression Examples

### 1. Constant Expression (Int/Float)

```cpp
class Const_Expr_Ast : public Expression_Ast {
private:
    string value;
    DataType type;

public:
    Const_Expr_Ast(string val, DataType dtype);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) {
        // Constants don't generate any statements
        // Just return a constant operand
        return new Const_TAC_Opd(value, type);
    }
};
```

### 2. Variable Expression

```cpp
class Name_Expr_Ast : public Expression_Ast {
private:
    string name;
    DataType type;

public:
    Name_Expr_Ast(string var_name, DataType dtype);
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) {
        // Variables don't generate any statements
        // Just return a variable operand
        return new Var_TAC_Opd(name, type);
    }
};
```

### 3. Binary Expression (a + b, a < b, etc.)

```cpp
class Binary_Expr_Ast : public Expression_Ast {
private:
    Expression_Ast *left;
    Expression_Ast *right;
    string operation;  // "+", "-", "<", etc.
    DataType result_type;

public:
    Binary_Expr_Ast(Expression_Ast *l, Expression_Ast *r, string op, DataType rtype);
    
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) {
        // Generate TAC for left operand
        TAC_Opd *left_opd = left->generate_tac(statements);
        
        // Generate TAC for right operand
        TAC_Opd *right_opd = right->generate_tac(statements);
        
        // Create temporary for result
        Temp_TAC_Opd *result = tac_gen.new_temp(result_type);
        
        // Create assignment statement: result = left op right
        Assign_TAC_Stmt *assign = tac_gen.new_assign_stmt(result, left_opd, operation, right_opd);
        statements.push_back(assign);
        
        return result;
    }
};
```

### 4. Unary Expression (! x, - x)

```cpp
class Unary_Expr_Ast : public Expression_Ast {
private:
    Expression_Ast *operand;
    string operation;  // "-", "!"
    DataType result_type;

public:
    Unary_Expr_Ast(Expression_Ast *op, string oper, DataType rtype);
    
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) {
        // Generate TAC for operand
        TAC_Opd *opd = operand->generate_tac(statements);
        
        // Create temporary for result
        Temp_TAC_Opd *result = tac_gen.new_temp(result_type);
        
        // Create assignment statement: result = op operand
        Assign_TAC_Stmt *assign = tac_gen.new_assign_stmt(result, operation, opd);
        statements.push_back(assign);
        
        return result;
    }
};
```

### 5. Ternary Expression (a ? b : c)

```cpp
class Ternary_Expr_Ast : public Expression_Ast {
private:
    Expression_Ast *condition;
    Expression_Ast *true_expr;
    Expression_Ast *false_expr;
    DataType result_type;

public:
    Ternary_Expr_Ast(Expression_Ast *cond, Expression_Ast *true_e, 
                     Expression_Ast *false_e, DataType rtype);
    
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) {
        // Generate TAC for condition
        TAC_Opd *cond_opd = condition->generate_tac(statements);
        
        // Create result temporary (using stemp for ternary)
        Temp_TAC_Opd *result = tac_gen.new_stemp(result_type);
        
        // Create labels
        Label_TAC_Opd *false_label = tac_gen.new_label();
        Label_TAC_Opd *end_label = tac_gen.new_label();
        
        // Negate condition and jump to false branch if needed
        Temp_TAC_Opd *neg_cond = tac_gen.new_temp(BOOL_DATA_TYPE);
        statements.push_back(tac_gen.new_assign_stmt(neg_cond, "!", cond_opd));
        statements.push_back(tac_gen.new_if_goto_stmt(neg_cond, false_label));
        
        // True branch
        TAC_Opd *true_opd = true_expr->generate_tac(statements);
        statements.push_back(tac_gen.new_move_stmt(result, true_opd));
        statements.push_back(tac_gen.new_goto_stmt(end_label));
        
        // False branch
        statements.push_back(tac_gen.new_label_stmt(false_label));
        TAC_Opd *false_opd = false_expr->generate_tac(statements);
        statements.push_back(tac_gen.new_move_stmt(result, false_opd));
        
        // End label
        statements.push_back(tac_gen.new_label_stmt(end_label));
        
        return result;
    }
};
```

---

## Statement Examples

### 6. Assignment Statement (x = expr;)

```cpp
class Assignment_Stmt_Ast : public Statement_Ast {
private:
    string variable;
    Expression_Ast *rhs;
    DataType var_type;

public:
    Assignment_Stmt_Ast(string var, Expression_Ast *r, DataType dtype);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        // Generate TAC for RHS expression
        TAC_Opd *rhs_opd = rhs->generate_tac(statements);
        
        // Create variable operand
        Var_TAC_Opd *var_opd = new Var_TAC_Opd(variable, var_type);
        
        // Create assignment: var = rhs_result
        statements.push_back(tac_gen.new_move_stmt(var_opd, rhs_opd));
    }
};
```

### 7. If Statement

```cpp
class If_Stmt_Ast : public Statement_Ast {
private:
    Expression_Ast *condition;
    Statement_Ast *true_stmt;
    Statement_Ast *false_stmt;  // NULL if no else

public:
    If_Stmt_Ast(Expression_Ast *cond, Statement_Ast *true_s, Statement_Ast *false_s = NULL);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        // Generate condition TAC
        TAC_Opd *cond_opd = condition->generate_tac(statements);
        
        // Create labels
        Label_TAC_Opd *else_label = tac_gen.new_label();
        Label_TAC_Opd *end_label = tac_gen.new_label();
        
        // if(!condition) goto else_label
        Temp_TAC_Opd *neg = tac_gen.new_temp(BOOL_DATA_TYPE);
        statements.push_back(tac_gen.new_assign_stmt(neg, "!", cond_opd));
        statements.push_back(tac_gen.new_if_goto_stmt(neg, else_label));
        
        // True branch
        true_stmt->generate_tac(statements);
        statements.push_back(tac_gen.new_goto_stmt(end_label));
        
        // Else branch (if exists)
        statements.push_back(tac_gen.new_label_stmt(else_label));
        if (false_stmt) {
            false_stmt->generate_tac(statements);
        }
        
        // End label
        statements.push_back(tac_gen.new_label_stmt(end_label));
    }
};
```

### 8. While Loop

```cpp
class While_Stmt_Ast : public Statement_Ast {
private:
    Expression_Ast *condition;
    Statement_Ast *body;

public:
    While_Stmt_Ast(Expression_Ast *cond, Statement_Ast *b);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        // Create labels
        Label_TAC_Opd *loop_label = tac_gen.new_label();
        Label_TAC_Opd *exit_label = tac_gen.new_label();
        
        // Loop label
        statements.push_back(tac_gen.new_label_stmt(loop_label));
        
        // Generate condition TAC
        TAC_Opd *cond_opd = condition->generate_tac(statements);
        
        // if(!condition) goto exit_label
        Temp_TAC_Opd *neg = tac_gen.new_temp(BOOL_DATA_TYPE);
        statements.push_back(tac_gen.new_assign_stmt(neg, "!", cond_opd));
        statements.push_back(tac_gen.new_if_goto_stmt(neg, exit_label));
        
        // Body
        body->generate_tac(statements);
        
        // goto loop_label
        statements.push_back(tac_gen.new_goto_stmt(loop_label));
        
        // Exit label
        statements.push_back(tac_gen.new_label_stmt(exit_label));
    }
};
```

### 9. Do-While Loop

```cpp
class Do_While_Stmt_Ast : public Statement_Ast {
private:
    Statement_Ast *body;
    Expression_Ast *condition;

public:
    Do_While_Stmt_Ast(Statement_Ast *b, Expression_Ast *cond);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        // Create labels
        Label_TAC_Opd *loop_label = tac_gen.new_label();
        
        // Loop label
        statements.push_back(tac_gen.new_label_stmt(loop_label));
        
        // Body
        body->generate_tac(statements);
        
        // Generate condition TAC
        TAC_Opd *cond_opd = condition->generate_tac(statements);
        
        // if(condition) goto loop_label
        statements.push_back(tac_gen.new_if_goto_stmt(cond_opd, loop_label));
    }
};
```

### 10. Print Statement

```cpp
class Print_Stmt_Ast : public Statement_Ast {
private:
    Expression_Ast *expr;

public:
    Print_Stmt_Ast(Expression_Ast *e);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        // Generate TAC for expression
        TAC_Opd *expr_opd = expr->generate_tac(statements);
        
        // Create print statement
        statements.push_back(tac_gen.new_print_stmt(expr_opd));
    }
};
```

### 11. Read Statement

```cpp
class Read_Stmt_Ast : public Statement_Ast {
private:
    string variable;
    DataType var_type;

public:
    Read_Stmt_Ast(string var, DataType dtype);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        // Create variable operand
        Var_TAC_Opd *var_opd = new Var_TAC_Opd(variable, var_type);
        
        // Create read statement
        statements.push_back(tac_gen.new_read_stmt(var_opd));
    }
};
```

### 12. Return Statement

```cpp
class Return_Stmt_Ast : public Statement_Ast {
private:
    Expression_Ast *expr;  // NULL for void return

public:
    Return_Stmt_Ast(Expression_Ast *e = NULL);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        TAC_Opd *return_opd = NULL;
        
        if (expr) {
            // Generate TAC for return expression
            return_opd = expr->generate_tac(statements);
        }
        
        // Create return statement
        statements.push_back(tac_gen.new_return_stmt(return_opd));
    }
};
```

### 13. Function Call Expression

```cpp
class FCall_Expr_Ast : public Expression_Ast {
private:
    string func_name;
    list<Expression_Ast*> arguments;
    DataType return_type;

public:
    FCall_Expr_Ast(string fname, list<Expression_Ast*> args, DataType rtype);
    
    TAC_Opd* generate_tac(list<TAC_Stmt*>& statements) {
        // Create temporary for result
        Temp_TAC_Opd *result = tac_gen.new_temp(return_type);
        
        // Create function call statement
        FCall_Return_TAC_Stmt *call = new FCall_Return_TAC_Stmt(result, func_name);
        
        // Generate TAC for each argument
        for (auto arg : arguments) {
            TAC_Opd *arg_opd = arg->generate_tac(statements);
            call->add_argument(arg_opd);
        }
        
        statements.push_back(call);
        
        return result;
    }
};
```

### 14. Compound Statement

```cpp
class Compound_Stmt_Ast : public Statement_Ast {
private:
    list<Statement_Ast*> statements_list;

public:
    Compound_Stmt_Ast(list<Statement_Ast*> stmts);
    
    void generate_tac(list<TAC_Stmt*>& statements) {
        // Generate TAC for each statement in sequence
        for (auto stmt : statements_list) {
            stmt->generate_tac(statements);
        }
    }
};
```

---

## Procedure/Function Integration

### Program-Level TAC Generation

```cpp
class Procedure_Ast {
private:
    string name;
    list<string> params;
    list<Statement_Ast*> statements;
    DataType return_type;

public:
    Procedure_Ast(string fname, list<string> parms, 
                  list<Statement_Ast*> stmts, DataType rtype);
    
    list<TAC_Stmt*> generate_tac() {
        list<TAC_Stmt*> tac_statements;
        
        // Reset TAC generator for this procedure
        tac_gen.reset();
        
        // Generate TAC for function body
        for (auto stmt : statements) {
            stmt->generate_tac(tac_statements);
        }
        
        return tac_statements;
    }
};
```

---

## TAC File Output

```cpp
void write_tac_to_file(Procedure_Ast *proc, const char *filename) {
    // Open TAC output file
    FILE *tac_file_ptr = fopen(filename, "w");
    if (!tac_file_ptr) {
        perror("Cannot open TAC file");
        return;
    }
    
    // Set global tac_file
    tac_file = tac_file_ptr;
    
    // Generate TAC
    list<TAC_Stmt*> tac_stmts = proc->generate_tac();
    
    // Write TAC statements
    for (auto stmt : tac_stmts) {
        stmt->print();
    }
    
    // Close file
    fclose(tac_file_ptr);
    tac_file = NULL;
}
```

---

## Integration Checklist

- [ ] Add forward declaration of TAC_Opd in ast_new.h
- [ ] Add #include "tac_new.h" to ast_new.cpp
- [ ] Add generate_tac() declaration to all AST classes
- [ ] Implement generate_tac() in all Expression_Ast subclasses
- [ ] Implement generate_tac() in all Statement_Ast subclasses
- [ ] Add procedure-level TAC generation
- [ ] Test TAC generation with sample input
- [ ] Verify TAC output to file
- [ ] Add error handling for type mismatches

---

## Testing TAC Generation

```cpp
int main() {
    // Parse program
    ASTNode *ast_root = parse(input_file);
    
    // Convert to new AST classes (if needed)
    Procedure_Ast *proc = convert_to_new_ast(ast_root);
    
    // Generate TAC
    list<TAC_Stmt*> tac = proc->generate_tac();
    
    // Write to file
    tac_file = fopen("output.tac", "w");
    for (auto stmt : tac) {
        stmt->print();
    }
    fclose(tac_file);
    
    return 0;
}
```

---

## Important Notes

1. **Type Consistency**: Ensure operand types match expected types
2. **Temporary Management**: Use tac_gen for unique temporaries
3. **Label Management**: Use tac_gen for unique labels
4. **Memory**: Consider cleanup of allocated TAC objects
5. **Visitor Pattern**: Can implement visitor pattern for TAC generation
6. **Code Reuse**: Extract common patterns into helper functions

---

*This guide provides patterns for integrating TAC generation into AST classes.*
