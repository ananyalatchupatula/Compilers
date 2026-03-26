# SCLP Refactoring Guide - Class-Based Design

## Overview
This document outlines the refactoring from your current C-based AST to the object-oriented design shown on the SCLP website.

## Current State
- Your AST uses simple `ASTNode` struct with `char label[100]`
- TAC generation uses procedural approach with temp counters
- No clear separation between expression and statement types

## Target Design (From SCLP Website)

### 1. AST Class Hierarchy
```
Ast (Abstract Base Class)
├── Expression_Ast
│   ├── Unary_Expr_Ast (unary minus, NOT)
│   ├── Binary_Expr_Ast (arithmetic, relational, logical)
│   ├── Ternary_Expr_Ast (a ? b : c)
│   └── Const/Name_Expr_Ast (leaves - constants and variables)
│
└── Statement_Ast
    ├── Assignment_Stmt
    ├── Print_Stmt
    ├── Read_Stmt
    ├── If_Stmt
    ├── While_Stmt
    ├── Do_While_Stmt
    ├── Compound_Stmt
    ├── Call_Stmt
    └── Return_Stmt
```

### 2. TAC Class Hierarchy
```
TAC_Stmt (Abstract Base Class)
├── Move (dest = src)
├── Compute (dest = op1 op op2)
├── Label (LABEL: )
├── Goto (GOTO label)
├── Cond_Goto (IFGOTO cond label)
└── Call/Return (for functions)

TAC_Opd (Abstract Base Class)
├── Const_TAC_Opd (constants)
├── Var_TAC_Opd (variables)
└── Temp_TAC_Opd (temporary variables)
```

### 3. Refactoring Steps
1. **Create base AST classes** with virtual methods like:
   - `print()` - for debugging
   - `process()` - for semantic analysis
   - `generate_tac()` - for TAC generation

2. **Create TAC classes** with:
   - Virtual `print()` method
   - Operand references
   - Operator information

3. **Update Parser** to create proper class instances instead of generic nodes

4. **Maintain backward compatibility** during transition

## Key Design Patterns
- Use **virtual functions** for polymorphic behavior
- Use **inheritance** to share common functionality
- Use **virtual methods** for print/generate operations
- Store **operand pointers** rather than label strings

## Files to Create
- `ast_node.h` - Base AST classes
- `ast_expr.h` - Expression AST classes
- `ast_stmt.h` - Statement AST classes
- `tac_stmt.h` - TAC statement classes
- `tac_opd.h` - TAC operand classes
- `tac_generator.h` - TAC generation infrastructure

## Migration Plan
1. Keep old code working while developing new classes
2. Update parser.y gradually to use new classes
3. Update TAC generation to use new structure
4. Remove old code once everything works
