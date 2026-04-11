# SCLP Architecture - Visual Guide

## 1. Complete Class Hierarchy

```
┌─────────────────────────────────────────────────────────────────┐
│                          Ast (Base)                             │
│                    (Abstract, Virtual Methods)                  │
└──────────────────┬──────────────────────────┬──────────────────┘
                   │                          │
        ┌──────────▼────────────┐  ┌──────────▼───────────────┐
        │  Expression_Ast       │  │   Statement_Ast         │
        │  (for expressions)    │  │   (for statements)      │
        └──────────┬────────────┘  └──────────┬───────────────┘
                   │                          │
        ┌──────────┴─────────┬─────┬──────┐  └──┬─────┬──┬──┬──┬──┐
        │                    │     │      │     │     │  │  │  │  │
    ┌───▼──┐  ┌──────┐ ┌───▼─┐┌──▼──┐┌──▼┐ ┌──▼┐┌──▼┐ │  │  │  │
    │Unary │  │Binary│ │Tern │Const │Name │ │Asgn │Prnt │ │  │  │
    │Expr  │  │Expr  │ │ary  │Expr  │Expr │ │Stmt │Stmt │ │  │  │
    └──────┘  └──────┘ └─────┴──────┴────┘ └────┴────┘ │  │  │  │
                                                         │  │  │  │
                                    Read  If While Do  Comp
                                    Stmt  Stmt Stmt  Stmt  Stmt
```

## 2. TAC Class Hierarchy

```
┌─────────────────────────────────────────────────────────────────┐
│                    TAC_Opd (Base Operand)                       │
│                 (Abstract, Virtual Methods)                     │
└──────────────────┬──────────────────────────────────────────────┘
                   │
        ┌──────────┴───────────┬──────────┬──────────┐
        │                      │          │          │
    ┌───▼──────┐  ┌───────────▼┐ ┌───────▼┐ ┌──────▼──┐
    │Const_TAC │  │Var_TAC_Opd │ │Temp_TAC│ │Label_TAC│
    │_Opd      │  │            │ │_Opd    │ │_Opd     │
    │(int,     │  │(variables) │ │(temps) │ │(labels) │
    │float,str)│  │            │ │        │ │         │
    └──────────┘  └────────────┘ └────────┘ └─────────┘

┌─────────────────────────────────────────────────────────────────┐
│                   TAC_Stmt (Base Statement)                     │
│                 (Abstract, Virtual Methods)                     │
└──────────────────┬──────────────────────────────────────────────┘
                   │
        ┌──────────┴────────┬──────────┬───────┬──────┬────┐
        │                   │          │       │      │    │
    ┌───▼──────┐  ┌────────▼┐ ┌──────▼┐ ┌────▼┐ ┌───▼┐┌──▼──┐
    │Assign_TAC│  │Compute_ │ │Label_ │ │Goto_│ │Cond│Print │
    │_Stmt     │  │TAC_Stmt │ │TAC_   │ │TAC_ │ │Goto│_TAC_ │
    │          │  │(x=y±z)  │ │Stmt   │ │Stmt │ │Stmt│Stmt  │
    │(x=y)     │  │         │ │(Lbl:) │ │(JMP)│ │(JC)│(PRT) │
    └──────────┘  └─────────┘ └───────┘ └─────┘ └────┘└──────┘
                                                        │
                                                    Read_TAC
                                                    _Stmt
                                                    (READ)
```

## 3. Compilation Flow

```
        Source Code (.c)
              │
              ▼
        ┌─────────────┐
        │   Scanner   │ (lex)
        │  (Tokenize) │
        └─────────────┘
              │
              ▼ Tokens
        ┌─────────────┐
        │   Parser    │ (yacc)
        │  (Recognize)│
        └─────────────┘
              │
              ▼ AST Objects
        ┌─────────────────────────────┐
        │  AST Generator              │
        │  (Create AST classes)       │
        │  - Expr classes             │
        │  - Statement classes        │
        └─────────────────────────────┘
              │
              ▼ Populated AST
        ┌─────────────────────────────┐
        │  TAC Generator              │
        │  (Call generate_tac() on    │
        │   AST nodes)                │
        │  - Creates TAC statements   │
        │  - Manages temporaries      │
        │  - Generates labels         │
        └─────────────────────────────┘
              │
              ▼ TAC List
        ┌─────────────┐
        │  (To RTL)   │
        │  (To ASM)   │
        │  (Output)   │
        └─────────────┘
```

## 4. Data Flow for Simple Expression

```
Input: x = a + b

Stage 1: Parsing
    a        b
    │        │
    └────┬───┘
         │
         ▼ Binary_Expr_Ast(PLUS_OP)
    Assignment_Stmt("x")

Stage 2: Type Checking
    Assignment_Stmt::set_data_type(INT_DATA_TYPE)
    Binary_Expr_Ast::set_data_type(INT_DATA_TYPE)

Stage 3: TAC Generation
    list<TAC_Stmt*> tac_list;
    assignment->generate_tac(tac_list);
    
    ┌─────────────────────────┐
    │ TAC_Generator creates:  │
    │ - temp0 (Temp_TAC_Opd)  │
    └─────────────────────────┘
    
    TAC Output:
    temp0 = a + b      ← Compute_TAC_Stmt
    x = temp0          ← Assign_TAC_Stmt

Stage 4: Output
    temp0 = a + b
    x = temp0
```

## 5. Class Interaction Example: if (x > 0) write x;

```
Parser Creates:
    ┌────────────────┐
    │ If_Stmt        │
    │ ├─ cond        │ ──→ Binary_Expr_Ast(GT_OP) ──→ Name_Expr_Ast("x"), Const_Expr_Ast(0)
    │ ├─ then_stmt   │ ──→ Print_Stmt ──→ Name_Expr_Ast("x")
    │ └─ else_stmt   │ ──→ NULL
    └────────────────┘

TAC Generator Creates:
    If_Stmt::generate_tac()
        │
        ├─ cond->generate_tac() ──→ Var_TAC_Opd("x")
        │
        ├─ TAC_Generator::create_new_label() ──→ Label_TAC_Opd(0)
        │
        ├─ Cond_Goto_TAC_Stmt(x, Label0)
        │
        ├─ Goto_TAC_Stmt(Label1)
        │
        ├─ Label_TAC_Stmt(Label0)
        │
        ├─ then_stmt->generate_tac() ──→ Print_TAC_Stmt(x)
        │
        └─ Label_TAC_Stmt(Label1)

Output TAC:
    IF (x) GOTO Label0
    GOTO Label1
    Label0:
    PRINT x
    Label1:
```

## 6. Memory Layout

### Old Approach
```
ASTNode { char label[100], ASTNode* left, right, third, int temp_id... }
     ▲
     │ Generic for ALL node types
     │ 120+ bytes per node
```

### New Approach
```
Const_Expr_Ast { int/double/string value }  ← Only what's needed
              ~40 bytes per node

Binary_Expr_Ast { Expr* lhs, Expr* rhs, op }
                ~60 bytes per node

If_Stmt { Expr* cond, Stmt* then, else }
        ~50 bytes per node
```

## 7. Method Dispatch

```
Old Code (Runtime String Comparison):
    ASTNode* node = ...
    if (strcmp(node->label, "Plus") == 0) { ... }
    else if (strcmp(node->label, "Minus") == 0) { ... }
    // Slow! O(n) string comparisons for each type check

New Code (Virtual Dispatch):
    Expression_Ast* expr = ...
    expr->generate_tac(tac_list);  // Calls correct method
    // Fast! O(1) virtual table lookup
```

## 8. Inheritance Example

```
Class: Binary_Expr_Ast

Public Methods:
    Binary_Expr_Ast(lhs, op, rhs)     // Constructor
    ~Binary_Expr_Ast()                // Destructor
    print(FILE*)                      // From Ast
    generate_tac(list<TAC_Stmt*>&)   // From Ast
    get_data_type()                   // From Ast
    set_data_type(DataType)           // From Ast
    get_op()                          // Own method
    get_lhs()                         // Own method
    get_rhs()                         // Own method

Private Members:
    Binary_Op oper;
    Expression_Ast* lhs;
    Expression_Ast* rhs;
```

## 9. Type System Flow

```
┌──────────────────────────────────────────────┐
│ Data Type Enum                               │
├──────────────────────────────────────────────┤
│ INT_DATA_TYPE      (integers: 1, 2, 3...)   │
│ FLOAT_DATA_TYPE    (floats: 1.5, 2.7...)    │
│ BOOL_DATA_TYPE     (booleans from relations)│
│ STRING_DATA_TYPE   (strings: "hello")       │
│ VOID_DATA_TYPE     (no return value)        │
└──────────────────────────────────────────────┘
         ▲
         │ set_data_type(DATA_TYPE)
         │
    ┌─────────────────────────────┐
    │ AST Node                    │
    │ ├─ node_data_type           │
    │ └─ get_data_type() return $ │
    └─────────────────────────────┘
```

## 10. TAC Generator Singleton Pattern

```
Global Access:
    TAC_Generator* gen = TAC_Generator::get_instance();
    
    ┌─────────────────────────┐
    │ TAC_Generator (static)  │
    │ ├─ temp_counter: 0      │
    │ ├─ label_counter: 0     │
    │ ├─ create_new_temp()    │
    │ └─ create_new_label()   │
    └─────────────────────────┘
         │
         ├─ temp0 ──→ Temp_TAC_Opd(0)
         ├─ temp1 ──→ Temp_TAC_Opd(1)
         ├─ Label0 ──→ Label_TAC_Opd(0)
         └─ Label1 ──→ Label_TAC_Opd(1)
```

## 11. Example Program Flow

```
main()
  │
  ├─ Expression_Ast* a = new Name_Expr_Ast("a")
  │
  ├─ Expression_Ast* b = new Name_Expr_Ast("b")
  │
  ├─ Expression_Ast* plus = new Binary_Expr_Ast(a, PLUS_OP, b)
  │                         plus->set_data_type(INT_DATA_TYPE)
  │
  ├─ Statement_Ast* assign = new Assignment_Stmt("x", plus)
  │
  ├─ list<TAC_Stmt*> tac_list;
  │
  ├─ assign->generate_tac(tac_list);
  │        │
  │        ├─ plus->generate_tac(tac_list)
  │        │   │
  │        │   ├─ a->generate_tac() ──→ Var_TAC_Opd("a")
  │        │   │
  │        │   ├─ b->generate_tac() ──→ Var_TAC_Opd("b")
  │        │   │
  │        │   └─ Create Compute_TAC_Stmt(temp0, a, PLUS, b)
  │        │
  │        └─ Create Assign_TAC_Stmt(x, temp0)
  │
  ├─ for (auto stmt : tac_list) stmt->print()
  │
  └─ delete assign;  // Recursively cleans up entire tree
```

## 12. File Organization

```
Your Project Directory
│
├── Implementation Files (NEW - 6 files)
│   ├── ast_node.h          ┐
│   ├── ast_node.cpp        ├─ AST Infrastructure (1,200 lines)
│   ├── tac_str.h           │
│   ├── tac_str.cpp         ├─ TAC Infrastructure (1,010 lines)
│   ├── tac_generator.h     │
│   └── tac_generator.cpp   ┘
│
├── Documentation Files (NEW - 7 files)
│   ├── INDEX.md                      (Main navigation)
│   ├── README_REFACTORING.md         (Complete overview)
│   ├── QUICKSTART.md                 (Quick reference)
│   ├── CLASS_USAGE_GUIDE.md          (API documentation)
│   ├── PARSER_UPDATE_EXAMPLES.md     (Parser migration)
│   ├── IMPLEMENTATION_SUMMARY.md     (Technical details)
│   └── REFACTORING_GUIDE.md          (Design rationale)
│
├── Example Files (NEW)
│   └── EXAMPLE_PROGRAM.cpp           (10 runnable examples)
│
├── Your Existing Files (Keep for reference)
│   ├── parser.y                      (TO BE UPDATED)
│   ├── scanner.l
│   ├── main.cpp
│   ├── ast.h, ast.cpp                (OLD - to be removed)
│   ├── tac.h, tac.cpp                (OLD - to be removed)
│   └── ...
│
└── Makefile (TO BE UPDATED)
    Add: ast_node.o tac_str.o tac_generator.o
```

---

This visual guide should help you understand the complete architecture and how all the pieces fit together!
