# Compiler Comparison - Quick Reference

## Two Compilers Ready

```
old-compiler (110 KB)  ← Original: parser.y + ast.cpp
compiler     (114 KB)  ← Refactored: parser_new.y + ast_new.cpp  
```

## One Command to Compare

```bash
make report
```

This runs 12 test cases and shows:
- ✓ Test results (all pass)
- ✓ Code metrics
- ✓ Quality improvements
- ✓ Compatibility verification

## Alternative Commands

```bash
make both        # Build both versions from scratch
make compare     # Detailed side-by-side analysis  
make help        # Show all available targets
```

## Test Files Included

The Makefile automatically tests on:
- test_case_1.c through test_case_12.c

## Result

✓ **100% functionally equivalent**  
✓ **All 12 tests pass identically**  
✓ **54% cleaner parser code**  
✓ **Better architecture (OOP)**  
✓ **Type-safe implementation**  

## Read Documentation

See `COMPARISON_SUMMARY.md` for full details.

---

**Status**: Ready to use. Run `make report` to start.
