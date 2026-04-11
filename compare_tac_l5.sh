#!/bin/bash

# Compare generated tac files with valid tac files
# Tests all Level-5 test cases

COMPILER="./new-compiler"
TESTDIR="example-programs/Level-5-test-cases"
VALIDDIR="example-programs/Level-5-valid-tac"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}==================== TAC COMPARISON (Level-5) ====================${NC}"
echo -e "${BLUE}Compiler: ${COMPILER}${NC}"
echo -e "${BLUE}Test Dir: ${TESTDIR}${NC}"
echo -e "${BLUE}Valid Dir: ${VALIDDIR}${NC}"
echo ""

# Validations
[ ! -f "$COMPILER" ] && echo -e "${RED}Error: Compiler not found at $COMPILER${NC}" && exit 1
[ ! -d "$TESTDIR" ]   && echo -e "${RED}Error: Test directory not found at $TESTDIR${NC}" && exit 1
[ ! -d "$VALIDDIR" ]  && echo -e "${RED}Error: Valid directory not found at $VALIDDIR${NC}" && exit 1

echo -e "${YELLOW}Running TAC comparisons...${NC}\n"

PASSED=0
FAILED=0

for testfile in "$TESTDIR"/*.c; do
    [ -e "$testfile" ] || continue
    
    base=$(basename "$testfile")
    validfile="$VALIDDIR/$base.correct.tac"
    
    generatedfile="${testfile}.tac"
    
    if [ ! -f "$validfile" ]; then
        echo -e "${YELLOW}$base${NC}: SKIPPED (no valid reference file)"
        continue
    fi
    
    # 1. Run the compiler with the flag to generate the .tac file
    "$COMPILER" --show-tac "$testfile" > /dev/null 2>&1
    
    # 2. Verify the compiler actually created the file
    if [ ! -f "$generatedfile" ]; then
        echo -e "${RED}$base${NC}: ERROR (Compiler failed to create $base.tac)"
        ((FAILED++))
        continue
    fi
    
    # 3. Compare with the valid reference
    if diff -Bw "$generatedfile" "$validfile" > /dev/null 2>&1; then
        echo -e "${GREEN}$base${NC}: OK"
        ((PASSED++))
        rm "$generatedfile"
    else
        echo -e "${RED}$base${NC}: DIFFER"
        ((FAILED++))
        echo "--- Difference in $base ---"
        diff -Bw "$generatedfile" "$validfile"
        echo -e "${BLUE}----------------------------------------${NC}"
    fi
done

echo -e "\n${BLUE}==================== SUMMARY ====================${NC}"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"
echo -e "${BLUE}Total: $((PASSED + FAILED))${NC}"

[ $FAILED -gt 0 ] && exit 1 || exit 0
