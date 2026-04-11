#!/bin/bash

# Compare generated RTL files with valid RTL files for Level-5 examples 1-4
# Tests RTL generation

COMPILER="./new-compiler"
TESTDIR="example-programs/Level-5-test-cases"
VALIDDIR="example-programs/Level-5-valid-rtl"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}==================== RTL COMPARISON (L5-1 to L5-4) ====================${NC}"
echo -e "${BLUE}Compiler: ${COMPILER}${NC}"
echo -e "${BLUE}Test Dir: ${TESTDIR}${NC}"
echo -e "${BLUE}Valid Dir: ${VALIDDIR}${NC}"
echo ""

# Validations
[ ! -f "$COMPILER" ] && echo -e "${RED}Error: Compiler not found at $COMPILER${NC}" && exit 1
[ ! -d "$TESTDIR" ]   && echo -e "${RED}Error: Test directory not found at $TESTDIR${NC}" && exit 1
[ ! -d "$VALIDDIR" ]  && echo -e "${RED}Error: Valid directory not found at $VALIDDIR${NC}" && exit 1

echo -e "${YELLOW}Running RTL comparisons...${NC}\n"

PASSED=0
FAILED=0

for i in {1..23}; do
    testfile="$TESTDIR/l5-exmp$i.c"
    validfile="$VALIDDIR/l5-exmp$i.c.correct.rtl"
    generatedfile="${testfile}.rtl"
    
    base="l5-exmp$i.c"
    
    if [ ! -f "$testfile" ]; then
        echo -e "${YELLOW}$base${NC}: SKIPPED (test file not found)"
        continue
    fi
    
    if [ ! -f "$validfile" ]; then
        echo -e "${YELLOW}$base${NC}: SKIPPED (no valid reference file)"
        continue
    fi
    
    # 1. Run the compiler with --show-rtl flag
    "$COMPILER" --show-rtl "$testfile" > /dev/null 2>&1
    
    # 2. Verify the compiler created the file
    if [ ! -f "$generatedfile" ]; then
        echo -e "${RED}$base${NC}: ERROR (Compiler failed to create $base.rtl)"
        ((FAILED++))
        continue
    fi
    
    # 3. Strip comments (;;) and compare with -Bw
    sed 's/ *;;.*//' "$generatedfile" > /tmp/gen_rtl.txt
    sed 's/ *;;.*//' "$validfile" > /tmp/exp_rtl.txt
    
    if diff -Bw /tmp/gen_rtl.txt /tmp/exp_rtl.txt > /dev/null 2>&1; then
        echo -e "${GREEN}$base${NC}: OK"
        ((PASSED++))
        rm "$generatedfile"
    else
        echo -e "${RED}$base${NC}: DIFFER"
        ((FAILED++))
        echo "--- Difference in $base ---"
        diff -Bw /tmp/gen_rtl.txt /tmp/exp_rtl.txt
        echo -e "${BLUE}----------------------------------------${NC}"
    fi
done

echo -e "\n${BLUE}==================== SUMMARY ====================${NC}"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo -e "${BLUE}Total: $((PASSED + FAILED))${NC}"

[ $FAILED -gt 0 ] && exit 1 || exit 0
