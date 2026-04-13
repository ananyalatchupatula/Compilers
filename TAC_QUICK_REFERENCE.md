# TAC Quick Reference Guide

## Quick Pattern Examples

### 1. Arithmetic Expression: `x = a + b`

```cpp
// Create operands
Var_TAC_Opd *a = new Var_TAC_Opd("a", INT_DATA_TYPE);
Var_TAC_Opd *b = new Var_TAC_Opd("b", INT_DATA_TYPE);
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);

// Generate TAC
Temp_TAC_Opd *temp = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(temp, a, "+", b));
statements.push_back(tac_gen.new_move_stmt(x, temp));

// Output:
// temp0 = a + b
// x = temp0
```

### 2. Complex Expression: `x = a + b * c - d`

```cpp
// temp0 = b * c
Temp_TAC_Opd *t0 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, b, "*", c));

// temp1 = a + temp0
Temp_TAC_Opd *t1 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t1, a, "+", t0));

// temp2 = temp1 - d
Temp_TAC_Opd *t2 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t2, t1, "-", d));

// x = temp2
statements.push_back(tac_gen.new_move_stmt(x, t2));

// Output:
// temp0 = b * c
// temp1 = a + temp0
// temp2 = temp1 - d
// x = temp2
```

### 3. Unary Operation: `x = -a`

```cpp
Temp_TAC_Opd *temp = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(temp, "-", a));
statements.push_back(tac_gen.new_move_stmt(x, temp));

// Output:
// temp0 = - a
// x = temp0
```

### 4. Boolean Operation: `x = a && b`

```cpp
Temp_TAC_Opd *temp = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(temp, a, "&&", b));
statements.push_back(tac_gen.new_move_stmt(x, temp));

// Output:
// temp0 = a && b
// x = temp0
```

### 5. Relational Expression: `x = a < b`

```cpp
Temp_TAC_Opd *temp = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(temp, a, "<", b));
statements.push_back(tac_gen.new_move_stmt(x, temp));

// Output:
// temp0 = a < b
// x = temp0
```

### 6. If-Else Statement

```cpp
// Example: if(a > b) x = 1; else x = 2;

// temp0 = a > b
Temp_TAC_Opd *cond_temp = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond_temp, a, ">", b));

// if(!temp0) goto else_label
Label_TAC_Opd *else_label = tac_gen.new_label();
statements.push_back(tac_gen.new_if_goto_stmt(cond_temp, else_label, true));

// true branch: x = 1
Const_TAC_Opd *one = new Const_TAC_Opd("1", INT_DATA_TYPE);
statements.push_back(tac_gen.new_move_stmt(x, one));

// goto end_label
Label_TAC_Opd *end_label = tac_gen.new_label();
statements.push_back(tac_gen.new_goto_stmt(end_label));

// else_label:
statements.push_back(tac_gen.new_label_stmt(else_label));

// false branch: x = 2
Const_TAC_Opd *two = new Const_TAC_Opd("2", INT_DATA_TYPE);
statements.push_back(tac_gen.new_move_stmt(x, two));

// end_label:
statements.push_back(tac_gen.new_label_stmt(end_label));

// Output:
// temp0 = a > b
// if(!temp0) goto Label0
// x = 1
// goto Label1
// Label0:
// x = 2
// Label1:
```

### 7. While Loop

```cpp
// Example: while(i < 10) i = i + 1;

// loop_label:
Label_TAC_Opd *loop_label = tac_gen.new_label();
statements.push_back(tac_gen.new_label_stmt(loop_label));

// temp0 = i < 10
Temp_TAC_Opd *cond = tac_gen.new_temp(BOOL_DATA_TYPE);
Const_TAC_Opd *ten = new Const_TAC_Opd("10", INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond, i, "<", ten));

// if(!temp0) goto exit_label
Label_TAC_Opd *exit_label = tac_gen.new_label();
statements.push_back(tac_gen.new_if_goto_stmt(cond, exit_label, true));

// body: i = i + 1
Temp_TAC_Opd *t0 = tac_gen.new_temp(INT_DATA_TYPE);
Const_TAC_Opd *one = new Const_TAC_Opd("1", INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, i, "+", one));
statements.push_back(tac_gen.new_move_stmt(i, t0));

// goto loop_label
statements.push_back(tac_gen.new_goto_stmt(loop_label));

// exit_label:
statements.push_back(tac_gen.new_label_stmt(exit_label));

// Output:
// Label0:
// temp0 = i < 10
// if(!temp0) goto Label1
// temp1 = i + 1
// i = temp1
// goto Label0
// Label1:
```

### 8. Do-While Loop

```cpp
// Example: do { i = i + 1; } while(i < 10);

// loop_label:
Label_TAC_Opd *loop_label = tac_gen.new_label();
statements.push_back(tac_gen.new_label_stmt(loop_label));

// body: i = i + 1
Temp_TAC_Opd *t0 = tac_gen.new_temp(INT_DATA_TYPE);
Const_TAC_Opd *one = new Const_TAC_Opd("1", INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, i, "+", one));
statements.push_back(tac_gen.new_move_stmt(i, t0));

// temp1 = i < 10
Temp_TAC_Opd *cond = tac_gen.new_temp(BOOL_DATA_TYPE);
Const_TAC_Opd *ten = new Const_TAC_Opd("10", INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond, i, "<", ten));

// if(temp1) goto loop_label
statements.push_back(tac_gen.new_if_goto_stmt(cond, loop_label, false));

// Output:
// Label0:
// temp0 = i + 1
// i = temp0
// temp1 = i < 10
// if(temp1) goto Label0
```

### 9. Ternary Operator: `x = a > b ? c : d`

```cpp
// temp0 = a > b
Temp_TAC_Opd *cond = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond, a, ">", b));

// temp1 = !temp0
Temp_TAC_Opd *neg_cond = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(neg_cond, "!", cond));

// if(!temp0) goto false_label
Label_TAC_Opd *false_label = tac_gen.new_label();
statements.push_back(tac_gen.new_if_goto_stmt(neg_cond, false_label));

// true branch: stemp0 = c
Temp_TAC_Opd *result = tac_gen.new_stemp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_move_stmt(result, c));

// goto end_label
Label_TAC_Opd *end_label = tac_gen.new_label();
statements.push_back(tac_gen.new_goto_stmt(end_label));

// false_label:
statements.push_back(tac_gen.new_label_stmt(false_label));

// false branch: stemp0 = d
statements.push_back(tac_gen.new_move_stmt(result, d));

// end_label:
statements.push_back(tac_gen.new_label_stmt(end_label));

// x = stemp0
statements.push_back(tac_gen.new_move_stmt(x, result));

// Output:
// temp0 = a > b
// temp1 = ! temp0
// if(!temp1) goto Label0
// stemp0 = c
// goto Label1
// Label0:
// stemp0 = d
// Label1:
// x = stemp0
```

### 10. Function Call

```cpp
// print(x, y)
Print_TAC_Stmt *print_stmt = tac_gen.new_print_stmt(x);
statements.push_back(print_stmt);

// read y
Var_TAC_Opd *y = new Var_TAC_Opd("y", INT_DATA_TYPE);
Read_TAC_Stmt *read_stmt = tac_gen.new_read_stmt(y);
statements.push_back(read_stmt);

// Output:
// print x
// read y
```

### 11. Function Call with Return Value

```cpp
// temp0 = call foo(a, b)
Temp_TAC_Opd *result = tac_gen.new_temp(INT_DATA_TYPE);
FCall_Return_TAC_Stmt *call = new FCall_Return_TAC_Stmt(result, "foo");
call->add_argument(a);
call->add_argument(b);
statements.push_back(call);

// x = temp0
statements.push_back(tac_gen.new_move_stmt(x, result));

// Output:
// temp0 = call foo(a, b)
// x = temp0
```

### 12. Function Definition with Return

```cpp
// Within function foo:
// return x + y

Temp_TAC_Opd *sum = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(sum, x, "+", y));
statements.push_back(tac_gen.new_return_stmt(sum));

// Output:
// temp0 = x + y
// return temp0
```

## Operator Codes Used in TAC

| Operator | Code | Type |
|----------|------|------|
| Addition | "+" | Arithmetic |
| Subtraction | "-" | Arithmetic |
| Multiplication | "*" | Arithmetic |
| Division | "/" | Arithmetic |
| Unary Minus | "-" | Unary |
| Less Than | "<" | Relational |
| Less Equal | "<=" | Relational |
| Greater Than | ">" | Relational |
| Greater Equal | ">=" | Relational |
| Equality | "==" | Relational |
| Not Equal | "!=" | Relational |
| Logical AND | "&&" | Logical |
| Logical OR | "\|\|" | Logical |
| Logical NOT | "!" | Unary |

## Important Tips

1. **Precedence**: Generate TAC respecting operator precedence (e.g., multiply before add)
2. **Temporaries**: Always use new temporaries for intermediate results
3. **String Temps**: Use `new_stemp()` for results from ternary operators  
4. **Labels**: Use unique labels from `new_label()` for control flow
5. **Data Types**: Track data types through TAC generation
6. **Memory**: Clean up allocated objects when done (especially in lists)

## Integration with AST

In AST `generate_tac()` methods:

```cpp
// Pattern for binary expressions
TAC_Opd* Binary_Expr_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    TAC_Opd *left = left_child->generate_tac(statements);
    TAC_Opd *right = right_child->generate_tac(statements);
    Temp_TAC_Opd *result = tac_gen.new_temp(get_data_type());
    statements.push_back(tac_gen.new_assign_stmt(result, left, get_operator(), right));
    return result;
}

// Pattern for statements
void Statement_Ast::generate_tac(list<TAC_Stmt*>& statements) {
    // Generate TAC specific to statement type
}
```

---
*Quick reference for SCLP TAC class hierarchy*
