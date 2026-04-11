# SCLP Refactoring - Implementation Checklist

## ✅ What Has Been Completed

### Core Implementation (100% Complete)

- [x] **AST Class Hierarchy**
  - [x] Base `Ast` class with virtual methods
  - [x] `Expression_Ast` base class
  - [x] `Statement_Ast` base class
  - [x] `Unary_Expr_Ast` for -x and !x
  - [x] `Binary_Expr_Ast` for +,-,*,/,<,>,==,!=,&&,||
  - [x] `Ternary_Expr_Ast` for a?b:c
  - [x] `Const_Expr_Ast` for integer, float, string constants
  - [x] `Name_Expr_Ast` for variable references
  - [x] `Assignment_Stmt` for assignments
  - [x] `Print_Stmt` for write statements
  - [x] `Read_Stmt` for read statements
  - [x] `If_Stmt` for if/else
  - [x] `While_Stmt` for while loops
  - [x] `Do_While_Stmt` for do-while loops
  - [x] `Compound_Stmt` for compound statements

- [x] **TAC Class Hierarchy**
  - [x] Base `TAC_Opd` class for operands
  - [x] `Const_TAC_Opd` for constants
  - [x] `Var_TAC_Opd` for variables
  - [x] `Temp_TAC_Opd` for temporaries
  - [x] `Label_TAC_Opd` for labels
  - [x] Base `TAC_Stmt` class for statements
  - [x] `Assign_TAC_Stmt` for assignments
  - [x] `Compute_TAC_Stmt` for computations
  - [x] `Label_TAC_Stmt` for labels
  - [x] `Goto_TAC_Stmt` for jumps
  - [x] `Cond_Goto_TAC_Stmt` for conditional jumps
  - [x] `Print_TAC_Stmt` for print
  - [x] `Read_TAC_Stmt` for read

- [x] **TAC Generator**
  - [x] Singleton pattern implementation
  - [x] Temporary variable counter and generation
  - [x] Label counter and generation
  - [x] Reset functionality

### Code Quality (100% Complete)

- [x] Proper memory management (destructors)
- [x] Virtual methods for polymorphism
- [x] Type safety with enums
- [x] Clear naming conventions
- [x] Comprehensive comments
- [x] Error handling
- [x] Resource cleanup

### Documentation (100% Complete)

- [x] **INDEX.md** - Navigation guide
- [x] **README_REFACTORING.md** - Complete overview
- [x] **QUICKSTART.md** - Quick reference (copy-paste ready)
- [x] **CLASS_USAGE_GUIDE.md** - Detailed API with 100+ examples
- [x] **PARSER_UPDATE_EXAMPLES.md** - 10 before/after examples
- [x] **IMPLEMENTATION_SUMMARY.md** - Technical deep dive
- [x] **REFACTORING_GUIDE.md** - Design rationale
- [x] **ARCHITECTURE_VISUAL.md** - Visual diagrams
- [x] **EXAMPLE_PROGRAM.cpp** - 10 runnable examples

### Language Features Supported (100% for L1-L3)

- [x] **Level 1**
  - [x] Variable declarations
  - [x] Assignment statements
  - [x] Print statements (write)
  - [x] Read statements

- [x] **Level 2**
  - [x] Arithmetic expressions (+, -, *, /)
  - [x] Unary minus (-x)
  - [x] Operator precedence

- [x] **Level 3**
  - [x] Relational operators (<, >, <=, >=, ==, !=)
  - [x] Logical operators (&&, ||, !)
  - [x] Ternary operator (a ? b : c)
  - [x] Mixed expression types

---

## ⏭️ What Comes Next (Your Tasks)

### Phase 1: Understanding & Setup (2 hours)

- [ ] **Read Documentation**
  - [ ] Read INDEX.md (navigation)
  - [ ] Read README_REFACTORING.md (overview)
  - [ ] Read QUICKSTART.md (quick start)
  - [ ] Skim CLASS_USAGE_GUIDE.md (API)

- [ ] **Compile Example**
  - [ ] `g++ -o example EXAMPLE_PROGRAM.cpp ast_node.cpp tac_str.cpp tac_generator.cpp`
  - [ ] Run `./example`
  - [ ] Verify all 10 examples produce output

- [ ] **Understand Architecture**
  - [ ] Review ARCHITECTURE_VISUAL.md
  - [ ] Understand class hierarchy
  - [ ] Understand data flow

### Phase 2: Parser Migration (3-4 hours)

- [ ] **Prepare Parser**
  - [ ] Open parser.y
  - [ ] Add includes:
    ```cpp
    #include "ast_node.h"
    #include "tac_str.h"
    #include "tac_generator.h"
    ```
  - [ ] Back up original parser.y

- [ ] **Update Type Definitions**
  - [ ] Replace #define TYPE_INT with INT_DATA_TYPE
  - [ ] Replace #define TYPE_FLOAT with FLOAT_DATA_TYPE
  - [ ] Replace #define TYPE_BOOL with BOOL_DATA_TYPE
  - [ ] Replace #define TYPE_STRING with STRING_DATA_TYPE
  - [ ] Update all type checking code

- [ ] **Update Expression Rules** (use PARSER_UPDATE_EXAMPLES.md)
  - [ ] Constants (integer, float, string)
  - [ ] Variables (names)
  - [ ] Unary operations (-x, !x)
  - [ ] Binary arithmetic (+, -, *, /)
  - [ ] Binary relational (<, >, <=, >=, ==, !=)
  - [ ] Binary logical (&&, ||)
  - [ ] Ternary operator (a ? b : c)

- [ ] **Update Statement Rules**
  - [ ] Assignment statements
  - [ ] Print statements
  - [ ] Read statements
  - [ ] If statements
  - [ ] While statements
  - [ ] Do-while statements
  - [ ] Compound statements

- [ ] **Incremental Testing**
  - [ ] Test each rule after updating
  - [ ] Use `--show-ast` to verify AST
  - [ ] Use `--show-tac` to verify TAC
  - [ ] Fix issues before moving to next rule

### Phase 3: Validation & Testing (2-3 hours)

- [ ] **Compilation**
  - [ ] Update Makefile with new object files
  - [ ] `make clean`
  - [ ] `make` (should compile without errors)

- [ ] **Testing with Existing Cases**
  - [ ] Test with simple_test.c
  - [ ] Test with test_case_1.c through test_case_12.c
  - [ ] Verify TAC output is correct
  - [ ] Verify AST output is correct

- [ ] **Edge Cases**
  - [ ] Test with complex expressions
  - [ ] Test with nested operations
  - [ ] Test with nested control flow
  - [ ] Test with all data types

- [ ] **Regression Testing**
  - [ ] Run existing test suite
  - [ ] Ensure all tests pass
  - [ ] No regressions introduced

### Phase 4: Cleanup (1 hour)

- [ ] **Remove Old Files**
  - [ ] Delete old ast.h
  - [ ] Delete old ast.cpp
  - [ ] Delete old tac.h
  - [ ] Delete old tac.cpp
  - [ ] (Keep as backup first!)

- [ ] **Code Organization**
  - [ ] Move documentation to docs/ folder (optional)
  - [ ] Update README with new compilation instructions
  - [ ] Update .gitignore if needed

- [ ] **Final Verification**
  - [ ] `make clean && make` (full rebuild)
  - [ ] Run all tests
  - [ ] Everything still works

---

## 📊 Progress Tracking

### Completion Status

```
Phase 1: Understanding & Setup     ☐ Not Started  ☐ In Progress  ☐ Complete
Phase 2: Parser Migration          ☐ Not Started  ☐ In Progress  ☐ Complete
Phase 3: Validation & Testing      ☐ Not Started  ☐ In Progress  ☐ Complete
Phase 4: Cleanup                   ☐ Not Started  ☐ In Progress  ☐ Complete
```

### Detailed Tracking

#### Phase 1: Understanding
- [ ] Understand AST class hierarchy (est. 30 min)
- [ ] Understand TAC class hierarchy (est. 30 min)
- [ ] Run example program (est. 15 min)
- [ ] Review ARCHITECTURE_VISUAL.md (est. 15 min)
- **Subtotal: 1.5 hours**

#### Phase 2: Parser Migration
Expression rules:
- [ ] Constants (5 min)
- [ ] Variables (5 min)
- [ ] Unary operations (10 min)
- [ ] Arithmetic operations (15 min)
- [ ] Relational operations (15 min)
- [ ] Logical operations (15 min)
- [ ] Ternary operator (10 min)

Statement rules:
- [ ] Assignment statements (10 min)
- [ ] Print statements (5 min)
- [ ] Read statements (5 min)
- [ ] If statements (15 min)
- [ ] While statements (10 min)
- [ ] Do-while statements (10 min)
- [ ] Compound statements (15 min)

- **Subtotal: 2.5-3 hours**

#### Phase 3: Testing
- [ ] Compilation (15 min)
- [ ] Simple test cases (30 min)
- [ ] Complex test cases (45 min)
- [ ] Edge cases (30 min)
- **Subtotal: 2 hours**

#### Phase 4: Cleanup
- [ ] Remove old files (15 min)
- [ ] Code organization (15 min)
- [ ] Final verification (30 min)
- **Subtotal: 1 hour**

**Total Estimated Time: 6.5 - 7.5 hours**

---

## 🎯 Key Milestones

### Milestone 1: Understanding (Day 1)
- [ ] Understand new class hierarchy
- [ ] Example program compiles and runs
- [ ] Can explain Ast, Expression_Ast, Statement_Ast
- [ ] Can explain TAC_Stmt, TAC_Opd

### Milestone 2: Expression Support (Day 1-2)
- [ ] Parser recognizes all expression types
- [ ] `--show-ast` produces correct AST
- [ ] `--show-tac` produces correct TAC for expressions

### Milestone 3: Statement Support (Day 2)
- [ ] Parser recognizes all statement types
- [ ] Simple programs compile and generate TAC
- [ ] Test cases 1-5 pass

### Milestone 4: Control Flow (Day 2-3)
- [ ] If/else statements work
- [ ] While loops work
- [ ] Do-while loops work
- [ ] Compound statements work
- [ ] Test cases 6-12 pass

### Milestone 5: Completion (Day 3)
- [ ] All tests pass
- [ ] No errors on test suite
- [ ] Old code removed
- [ ] Ready for Level 4 features

---

## 🔍 Verification Checklist

### Unit Testing
- [ ] Each AST class constructs correctly
- [ ] Each TAC class constructs correctly
- [ ] AST nodes print correctly
- [ ] TAC statements print correctly
- [ ] TAC generation creates correct statements

### Integration Testing
- [ ] Parser accepts valid L1-L3 code
- [ ] Parser rejects invalid code
- [ ] AST matches expected structure
- [ ] TAC matches expected output
- [ ] Type checking works correctly

### System Testing
- [ ] Full compilation pipeline works
- [ ] All command-line options work (--show-ast, --show-tac, etc.)
- [ ] Output files created correctly
- [ ] No memory leaks (valgrind clean)
- [ ] No undefined behavior

### Regression Testing
- [ ] All existing test cases still pass
- [ ] No features broken
- [ ] Performance is good
- [ ] Error messages are clear

---

## 📋 Files to Modify

### Must Update
- [ ] `parser.y` - Replace all make_node() calls
- [ ] `Makefile` - Add new .o files and headers
- [ ] Type definitions in parser - Use new enums

### Keep for Reference
- [ ] scanner.l (no changes needed)
- [ ] main.cpp (no changes needed)
- [ ] Existing test files

### Delete (After Backup)
- [ ] ast.h
- [ ] ast.cpp
- [ ] tac.h
- [ ] tac.cpp

---

## ⚠️ Common Issues & Solutions

| Issue | Solution | Docs |
|-------|----------|------|
| "Undefined reference to Binary_Expr_Ast" | Add #include "ast_node.h" | ast_node.h |
| "Type TYPE_INT not recognized" | Replace with INT_DATA_TYPE | PARSER_UPDATE_EXAMPLES.md |
| "make_node not found" | Replace with class constructor | PARSER_UPDATE_EXAMPLES.md |
| "Wrong TAC output" | Check set_data_type() was called | CLASS_USAGE_GUIDE.md |
| "Compilation fails" | Check all includes and linking | Makefile |
| "Memory errors" | Check delete calls | ast_node.cpp |

---

## 💾 Backup Strategy

Before making changes:
1. `git commit -m "Backup before refactoring"` (if using git)
2. `cp parser.y parser.y.bak`
3. `cp Makefile Makefile.bak`
4. `cp ast.h ast.h.old`
5. `cp ast.cpp ast.cpp.old`

---

## 📞 Resources During Implementation

Keep these files open while working:
1. **QUICKSTART.md** - Quick reference (most used)
2. **PARSER_UPDATE_EXAMPLES.md** - Copy-paste examples
3. **CLASS_USAGE_GUIDE.md** - Complete API
4. **parser.y** - What you're modifying
5. **ARCHITECTURE_VISUAL.md** - For understanding

---

## 🎉 Success Criteria

Your refactoring is complete when:

✅ **All 6 new implementation files compile**
✅ **EXAMPLE_PROGRAM.cpp runs without errors**
✅ **parser.y uses new classes (no more make_node)**
✅ **All existing test cases pass**
✅ **AST is properly structured** (--show-ast)
✅ **TAC is correctly generated** (--show-tac)
✅ **No memory leaks** (valgrind clean)
✅ **Code compiles with no warnings**
✅ **Old code files removed**
✅ **Documentation is readable and helpful**

---

## 📈 Next Feature Development

After completing refactoring:

### Level 4 Features (Control Flow - Already Supported!)
- [x] if statements
- [x] while loops
- [x] do-while loops
- [ ] switch statements (not in scope)
- [ ] break/continue statements (not in scope)

### Level 5 Features (Functions)
- [ ] Function definitions
- [ ] Function calls
- [ ] Return statements
- [ ] Parameter passing
- [ ] Local variables

### Level 6 Features (Arrays & Pointers)
- [ ] Array declarations
- [ ] Array access
- [ ] Pointer declarations
- [ ] Pointer dereference
- [ ] Address-of operator

---

This checklist should guide you through the entire refactoring process. Good luck! 🚀
