# TAC Examples - Test Programs and Expected Output

## Test Program 1: Simple Arithmetic

### Source Code
```c
void main() {
    int x, y, z;
    x = 5;
    y = 10;
    z = x + y;
    print z;
}
```

### Expected TAC Output
```
x = 5
y = 10
temp0 = x + y
z = temp0
print z
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Var_TAC_Opd *y = new Var_TAC_Opd("y", INT_DATA_TYPE);
Var_TAC_Opd *z = new Var_TAC_Opd("z", INT_DATA_TYPE);

Const_TAC_Opd *c5 = new Const_TAC_Opd("5", INT_DATA_TYPE);
Const_TAC_Opd *c10 = new Const_TAC_Opd("10", INT_DATA_TYPE);

statements.push_back(tac_gen.new_move_stmt(x, c5));
statements.push_back(tac_gen.new_move_stmt(y, c10));

Temp_TAC_Opd *temp0 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(temp0, x, "+", y));
statements.push_back(tac_gen.new_move_stmt(z, temp0));

statements.push_back(tac_gen.new_print_stmt(z));
```

---

## Test Program 2: Complex Expression

### Source Code
```c
void main() {
    int a, b, c, result;
    a = 10;
    b = 20;
    c = 5;
    result = a + b * c - a / c;
    print result;
}
```

### Expected TAC Output
```
a = 10
b = 20
c = 5
temp0 = b * c
temp1 = a + temp0
temp2 = a / c
temp3 = temp1 - temp2
result = temp3
print result
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *a = new Var_TAC_Opd("a", INT_DATA_TYPE);
Var_TAC_Opd *b = new Var_TAC_Opd("b", INT_DATA_TYPE);
Var_TAC_Opd *c = new Var_TAC_Opd("c", INT_DATA_TYPE);
Var_TAC_Opd *result = new Var_TAC_Opd("result", INT_DATA_TYPE);

// Assignments
statements.push_back(tac_gen.new_move_stmt(a, new Const_TAC_Opd("10", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(b, new Const_TAC_Opd("20", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(c, new Const_TAC_Opd("5", INT_DATA_TYPE)));

// temp0 = b * c
Temp_TAC_Opd *t0 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, b, "*", c));

// temp1 = a + temp0
Temp_TAC_Opd *t1 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t1, a, "+", t0));

// temp2 = a / c
Temp_TAC_Opd *t2 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t2, a, "/", c));

// temp3 = temp1 - temp2
Temp_TAC_Opd *t3 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t3, t1, "-", t2));

// result = temp3
statements.push_back(tac_gen.new_move_stmt(result, t3));

// print result
statements.push_back(tac_gen.new_print_stmt(result));
```

---

## Test Program 3: If-Else Statement

### Source Code
```c
void main() {
    int x, y;
    x = 10;
    y = 20;
    if (x < y) {
        print x;
    } else {
        print y;
    }
}
```

### Expected TAC Output
```
x = 10
y = 20
temp0 = x < y
if(!temp0) goto Label0
print x
goto Label1
Label0:
print y
Label1:
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Var_TAC_Opd *y = new Var_TAC_Opd("y", INT_DATA_TYPE);

statements.push_back(tac_gen.new_move_stmt(x, new Const_TAC_Opd("10", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(y, new Const_TAC_Opd("20", INT_DATA_TYPE)));

// temp0 = x < y
Temp_TAC_Opd *cond = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond, x, "<", y));

// Labels
Label_TAC_Opd *false_label = tac_gen.new_label();
Label_TAC_Opd *end_label = tac_gen.new_label();

// if(!temp0) goto Label0
statements.push_back(tac_gen.new_if_goto_stmt(cond, false_label, true));

// True branch
statements.push_back(tac_gen.new_print_stmt(x));
statements.push_back(tac_gen.new_goto_stmt(end_label));

// Label0:
statements.push_back(tac_gen.new_label_stmt(false_label));

// False branch
statements.push_back(tac_gen.new_print_stmt(y));

// Label1:
statements.push_back(tac_gen.new_label_stmt(end_label));
```

---

## Test Program 4: While Loop

### Source Code
```c
void main() {
    int i, sum;
    i = 0;
    sum = 0;
    while (i < 10) {
        sum = sum + i;
        i = i + 1;
    }
    print sum;
}
```

### Expected TAC Output
```
i = 0
sum = 0
Label0:
temp0 = i < 10
if(!temp0) goto Label1
temp1 = sum + i
sum = temp1
temp2 = i + 1
i = temp2
goto Label0
Label1:
print sum
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *i = new Var_TAC_Opd("i", INT_DATA_TYPE);
Var_TAC_Opd *sum = new Var_TAC_Opd("sum", INT_DATA_TYPE);

statements.push_back(tac_gen.new_move_stmt(i, new Const_TAC_Opd("0", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(sum, new Const_TAC_Opd("0", INT_DATA_TYPE)));

// Labels
Label_TAC_Opd *loop_label = tac_gen.new_label();
Label_TAC_Opd *exit_label = tac_gen.new_label();

// Label0:
statements.push_back(tac_gen.new_label_stmt(loop_label));

// temp0 = i < 10
Temp_TAC_Opd *cond = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond, i, "<", new Const_TAC_Opd("10", INT_DATA_TYPE)));

// if(!temp0) goto Label1
statements.push_back(tac_gen.new_if_goto_stmt(cond, exit_label, true));

// Loop body: sum = sum + i
Temp_TAC_Opd *t1 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t1, sum, "+", i));
statements.push_back(tac_gen.new_move_stmt(sum, t1));

// i = i + 1
Temp_TAC_Opd *t2 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t2, i, "+", new Const_TAC_Opd("1", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(i, t2));

// goto Label0
statements.push_back(tac_gen.new_goto_stmt(loop_label));

// Label1:
statements.push_back(tac_gen.new_label_stmt(exit_label));

// print sum
statements.push_back(tac_gen.new_print_stmt(sum));
```

---

## Test Program 5: Ternary Operator

### Source Code
```c
void main() {
    int a, b, max;
    a = 15;
    b = 20;
    max = (a > b) ? a : b;
    print max;
}
```

### Expected TAC Output
```
a = 15
b = 20
temp0 = a > b
temp1 = ! temp0
if(!temp1) goto Label0
stemp0 = a
goto Label1
Label0:
stemp0 = b
Label1:
max = stemp0
print max
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *a = new Var_TAC_Opd("a", INT_DATA_TYPE);
Var_TAC_Opd *b = new Var_TAC_Opd("b", INT_DATA_TYPE);
Var_TAC_Opd *max = new Var_TAC_Opd("max", INT_DATA_TYPE);

statements.push_back(tac_gen.new_move_stmt(a, new Const_TAC_Opd("15", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(b, new Const_TAC_Opd("20", INT_DATA_TYPE)));

// temp0 = a > b
Temp_TAC_Opd *cond = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond, a, ">", b));

// stemp0 for ternary result
Temp_TAC_Opd *result = tac_gen.new_stemp(INT_DATA_TYPE);

// Labels
Label_TAC_Opd *false_label = tac_gen.new_label();
Label_TAC_Opd *end_label = tac_gen.new_label();

// temp1 = ! temp0
Temp_TAC_Opd *neg = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(neg, "!", cond));

// if(!temp1) goto Label0
statements.push_back(tac_gen.new_if_goto_stmt(neg, false_label));

// True branch: stemp0 = a
statements.push_back(tac_gen.new_move_stmt(result, a));
statements.push_back(tac_gen.new_goto_stmt(end_label));

// Label0:
statements.push_back(tac_gen.new_label_stmt(false_label));

// False branch: stemp0 = b
statements.push_back(tac_gen.new_move_stmt(result, b));

// Label1:
statements.push_back(tac_gen.new_label_stmt(end_label));

// max = stemp0
statements.push_back(tac_gen.new_move_stmt(max, result));

// print max
statements.push_back(tac_gen.new_print_stmt(max));
```

---

## Test Program 6: Logical Operators

### Source Code
```c
void main() {
    bool x, y, z;
    x = 5 > 3;
    y = 2 < 1;
    z = x && y;
    print z;
}
```

### Expected TAC Output
```
temp0 = 5 > 3
x = temp0
temp1 = 2 < 1
y = temp1
temp2 = x && y
z = temp2
print z
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *x = new Var_TAC_Opd("x", BOOL_DATA_TYPE);
Var_TAC_Opd *y = new Var_TAC_Opd("y", BOOL_DATA_TYPE);
Var_TAC_Opd *z = new Var_TAC_Opd("z", BOOL_DATA_TYPE);

// temp0 = 5 > 3
Temp_TAC_Opd *t0 = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, new Const_TAC_Opd("5", INT_DATA_TYPE), 
                                             ">", new Const_TAC_Opd("3", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(x, t0));

// temp1 = 2 < 1
Temp_TAC_Opd *t1 = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t1, new Const_TAC_Opd("2", INT_DATA_TYPE), 
                                             "<", new Const_TAC_Opd("1", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(y, t1));

// temp2 = x && y
Temp_TAC_Opd *t2 = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t2, x, "&&", y));
statements.push_back(tac_gen.new_move_stmt(z, t2));

// print z
statements.push_back(tac_gen.new_print_stmt(z));
```

---

## Test Program 7: Unary Operators

### Source Code
```c
void main() {
    int a, b;
    bool c;
    a = 10;
    b = -a;
    c = !0 > 0;
    print b;
}
```

### Expected TAC Output
```
a = 10
temp0 = - a
b = temp0
temp1 = ! 0
temp2 = temp1 > 0
c = temp2
print b
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *a = new Var_TAC_Opd("a", INT_DATA_TYPE);
Var_TAC_Opd *b = new Var_TAC_Opd("b", INT_DATA_TYPE);
Var_TAC_Opd *c = new Var_TAC_Opd("c", BOOL_DATA_TYPE);

statements.push_back(tac_gen.new_move_stmt(a, new Const_TAC_Opd("10", INT_DATA_TYPE)));

// temp0 = -a (unary minus)
Temp_TAC_Opd *t0 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, "-", a));
statements.push_back(tac_gen.new_move_stmt(b, t0));

// temp1 = !0 (logical NOT)
Temp_TAC_Opd *t1 = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t1, "!", new Const_TAC_Opd("0", INT_DATA_TYPE)));

// temp2 = temp1 > 0
Temp_TAC_Opd *t2 = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t2, t1, ">", new Const_TAC_Opd("0", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(c, t2));

// print b
statements.push_back(tac_gen.new_print_stmt(b));
```

---

## Test Program 8: Do-While Loop

### Source Code
```c
void main() {
    int i;
    i = 0;
    do {
        print i;
        i = i + 1;
    } while (i < 5);
}
```

### Expected TAC Output
```
i = 0
Label0:
print i
temp0 = i + 1
i = temp0
temp1 = i < 5
if(temp1) goto Label0
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *i = new Var_TAC_Opd("i", INT_DATA_TYPE);

statements.push_back(tac_gen.new_move_stmt(i, new Const_TAC_Opd("0", INT_DATA_TYPE)));

Label_TAC_Opd *loop_label = tac_gen.new_label();
statements.push_back(tac_gen.new_label_stmt(loop_label));

// print i
statements.push_back(tac_gen.new_print_stmt(i));

// temp0 = i + 1
Temp_TAC_Opd *t0 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, i, "+", new Const_TAC_Opd("1", INT_DATA_TYPE)));
statements.push_back(tac_gen.new_move_stmt(i, t0));

// temp1 = i < 5
Temp_TAC_Opd *cond = tac_gen.new_temp(BOOL_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(cond, i, "<", new Const_TAC_Opd("5", INT_DATA_TYPE)));

// if(temp1) goto Label0
statements.push_back(tac_gen.new_if_goto_stmt(cond, loop_label));
```

---

## Test Program 9: Read Statement

### Source Code
```c
void main() {
    int x, y, sum;
    read x;
    read y;
    sum = x + y;
    print sum;
}
```

### Expected TAC Output
```
read x
read y
temp0 = x + y
sum = temp0
print sum
```

### C++ Code to Generate
```cpp
Var_TAC_Opd *x = new Var_TAC_Opd("x", INT_DATA_TYPE);
Var_TAC_Opd *y = new Var_TAC_Opd("y", INT_DATA_TYPE);
Var_TAC_Opd *sum = new Var_TAC_Opd("sum", INT_DATA_TYPE);

statements.push_back(tac_gen.new_read_stmt(x));
statements.push_back(tac_gen.new_read_stmt(y));

Temp_TAC_Opd *t0 = tac_gen.new_temp(INT_DATA_TYPE);
statements.push_back(tac_gen.new_assign_stmt(t0, x, "+", y));
statements.push_back(tac_gen.new_move_stmt(sum, t0));

statements.push_back(tac_gen.new_print_stmt(sum));
```

---

## Validation Checklist

### For Each Test Program:
- [ ] AST builds correctly
- [ ] TAC generation produces correct output
- [ ] Operator precedence respected
- [ ] Labels properly sequenced
- [ ] Temporaries correctly numbered
- [ ] Control flow jumps correct
- [ ] Data types consistent
- [ ] Output matches expected TAC

---

## Running TAC Tests

```cpp
// main.cpp - Example test driver
#include "ast_new.h"
#include "tac_new.h"

int main() {
    // Initialize
    tac_file = stdout;
    tac_gen.reset();
    
    // Build AST for test program
    list<Statement_Ast*> body;
    
    // [Add statements based on test program]
    
    // Generate TAC
    Compound_Stmt_Ast *main_stmt = new Compound_Stmt_Ast(body);
    list<TAC_Stmt*> tac_stmts;
    main_stmt->generate_tac(tac_stmts);
    
    // Print TAC
    for (auto stmt : tac_stmts) {
        stmt->print();
    }
    
    return 0;
}
```

---

*These examples demonstrate proper TAC generation for common programming constructs.*
