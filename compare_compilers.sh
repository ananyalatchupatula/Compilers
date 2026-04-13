#!/bin/bash

# Compare old compiler (old-compiler) vs new compiler (new-compiler)
# Tests both on same input files and shows differences

OLD_COMPILER="./old-compiler"
NEW_COMPILER="./new-compiler"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}==================== COMPILER COMPARISON ====================${NC}"
echo -e "${BLUE}Old: ${OLD_COMPILER}${NC}"
echo -e "${BLUE}New: ${NEW_COMPILER}${NC}"
echo ""

# Check if both executables exist
if [ ! -f "$OLD_COMPILER" ]; then
    echo -e "${RED}Error: Old compiler not found at $OLD_COMPILER${NC}"
    exit 1
fi

if [ ! -f "$NEW_COMPILER" ]; then
    echo -e "${RED}Error: New compiler not found at $NEW_COMPILER${NC}"
    exit 1
fi

echo -e "${YELLOW}Compiler versions:${NC}"
file "$OLD_COMPILER" | awk -F: '{print "Old: " $2}'
file "$NEW_COMPILER" | awk -F: '{print "New: " $2}'
echo ""

# Test on simple test case
TEST_FILE="test_case_1.c"
if [ ! -f "$TEST_FILE" ]; then
    echo -e "${RED}Test file not found: $TEST_FILE${NC}"
    exit 1
fi

echo -e "${YELLOW}Test Input: ${TEST_FILE}${NC}"
echo "---"
head -20 "$TEST_FILE"
echo "---"
echo ""

# Run old compiler
echo -e "${YELLOW}Running old compiler...${NC}"
OLD_AST_FILE="${TEST_FILE}.ast.old"
OLD_TAC_FILE="${TEST_FILE}.tac.old"

$OLD_COMPILER -d ast -ast "$TEST_FILE" > "$OLD_AST_FILE" 2>&1
$OLD_COMPILER -d tac -tac "$TEST_FILE" > "$OLD_TAC_FILE" 2>&1

echo -e "${GREEN}✓ Old compiler output generated${NC}"

# Run new compiler
echo -e "${YELLOW}Running new compiler...${NC}"
NEW_AST_FILE="${TEST_FILE}.ast.new"
NEW_TAC_FILE="${TEST_FILE}.tac.new"

$NEW_COMPILER -d ast -ast "$TEST_FILE" > "$NEW_AST_FILE" 2>&1
$NEW_COMPILER -d tac -tac "$TEST_FILE" > "$NEW_TAC_FILE" 2>&1

echo -e "${GREEN}✓ New compiler output generated${NC}"
echo ""

# Compare AST output
echo -e "${YELLOW}========== AST COMPARISON ==========${NC}"
if diff -q "$OLD_AST_FILE" "$NEW_AST_FILE" > /dev/null 2>&1; then
    echo -e "${GREEN}✓ AST outputs are IDENTICAL${NC}"
else
    echo -e "${YELLOW}⚠ AST outputs DIFFER:${NC}"
    echo -e "${BLUE}--- Old (my-sclp) ---${NC}"
    head -15 "$OLD_AST_FILE"
    echo ""
    echo -e "${BLUE}--- New (compiler) ---${NC}"
    head -15 "$NEW_AST_FILE"
    echo ""
    echo -e "${BLUE}First difference:${NC}"
    diff "$OLD_AST_FILE" "$NEW_AST_FILE" | head -10
fi

echo ""

# Compare TAC output
echo -e "${YELLOW}========== TAC COMPARISON ==========${NC}"
if diff -q "$OLD_TAC_FILE" "$NEW_TAC_FILE" > /dev/null 2>&1; then
    echo -e "${GREEN}✓ TAC outputs are IDENTICAL${NC}"
else
    echo -e "${YELLOW}⚠ TAC outputs DIFFER:${NC}"
    echo -e "${BLUE}--- Old (my-sclp) ---${NC}"
    head -15 "$OLD_TAC_FILE"
    echo ""
    echo -e "${BLUE}--- New (compiler) ---${NC}"
    head -15 "$NEW_TAC_FILE"
    echo ""
    echo -e "${BLUE}First difference:${NC}"
    diff "$OLD_TAC_FILE" "$NEW_TAC_FILE" | head -10
fi

echo ""

# Run on multiple test cases
echo -e "${YELLOW}========== MULTI-TEST SUMMARY ==========${NC}"
TOTAL=0
SAME=0
DIFFERENT=0

for testfile in test_case_*.c; do
    [ -f "$testfile" ] || continue
    
    TOTAL=$((TOTAL + 1))
    
    # AST comparison
    AST_OLD=$(mktemp)
    AST_NEW=$(mktemp)
    
    $OLD_COMPILER -d ast -ast "$testfile" > "$AST_OLD" 2>&1
    $NEW_COMPILER -d ast -ast "$testfile" > "$AST_NEW" 2>&1
    
    if diff -q "$AST_OLD" "$AST_NEW" > /dev/null 2>&1; then
        SAME=$((SAME + 1))
        echo -e "${GREEN}✓${NC} $testfile"
    else
        DIFFERENT=$((DIFFERENT + 1))
        echo -e "${RED}✗${NC} $testfile (AST differs)"
    fi
    
    rm -f "$AST_OLD" "$AST_NEW"
done

echo ""
echo -e "${BLUE}Summary: ${GREEN}$SAME identical${NC}, ${RED}$DIFFERENT different${NC} out of ${YELLOW}$TOTAL tests${NC}"

# Cleanup
echo ""
echo -e "${YELLOW}Keeping detailed comparison files:${NC}"
echo "  $OLD_AST_FILE, $NEW_AST_FILE"
echo "  $OLD_TAC_FILE, $NEW_TAC_FILE"
echo ""
echo -e "${BLUE}To view detailed diff: diff $OLD_AST_FILE $NEW_AST_FILE${NC}"
