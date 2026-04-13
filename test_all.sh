#!/bin/bash

# Comprehensive compiler testing script
# Tests AST, TAC, and RTL generation for both Level-1 and Level-4 test cases

set -e

COMPILER="./new-compiler"
L1_DIR="example-programs/Level-1-test-cases"
L4_DIR="example-programs/Level-4-test-cases"

# Check if compiler exists
if [ ! -f "$COMPILER" ]; then
    echo "Error: $COMPILER not found. Please compile it first."
    exit 1
fi

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
NC='\033[0m'

echo "=========================================="
echo "Complete Compiler Test Suite"
echo "=========================================="
echo ""

# Test AST
echo "Testing AST Generation (Level-1)..."
passed=0
failed=0
for f in "$L1_DIR"/*.c; do
    if [ -f "$f" ]; then
        base=$(basename "$f" .c)
        if "$COMPILER" "$f" --show-ast > /dev/null 2>&1; then
            passed=$((passed + 1))
        else
            echo -e "${RED}✗ Failed to generate AST for $base${NC}"
            failed=$((failed + 1))
        fi
    fi
done
echo -e "${GREEN}AST Level-1: Passed $passed, Failed $failed${NC}"
echo ""

# Test TAC for Level-4
echo "Testing TAC Generation (Level-4)..."
passed=0
failed=0
for f in "$L4_DIR"/l4-exmp*.c; do
    if [ -f "$f" ]; then
        base=$(basename "$f" .c)
        if "$COMPILER" "$f" -tac > /dev/null 2>&1; then
            passed=$((passed + 1))
        else
            echo -e "${RED}✗ Failed to generate TAC for $base${NC}"
            failed=$((failed + 1))
        fi
    fi
done
echo -e "${GREEN}TAC Level-4: Passed $passed, Failed $failed${NC}"
echo ""

# Test RTL for Level-4
echo "Testing RTL Generation (Level-4)..."
passed=0
failed=0
for f in "$L4_DIR"/l4-exmp*.c; do
    if [ -f "$f" ]; then
        base=$(basename "$f" .c)
        if "$COMPILER" "$f" --show-rtl > /dev/null 2>&1; then
            passed=$((passed + 1))
        else
            echo -e "${RED}✗ Failed to generate RTL for $base${NC}"
            failed=$((failed + 1))
        fi
    fi
done
echo -e "${GREEN}RTL Level-4: Passed $passed, Failed $failed${NC}"
echo ""

echo "=========================================="
echo "Running detailed comparison tests..."
echo "=========================================="
echo ""

# Run AST comparison
echo "Running compare_ast.sh..."
if ./compare_ast.sh 2>&1 | tail -5; then
    echo -e "${GREEN}✓ AST tests passed${NC}"
else
    echo -e "${RED}✗ AST tests failed${NC}"
fi
echo ""

# Run TAC comparison
echo "Running compare_tac.sh..."
if ./compare_tac.sh 2>&1 | tail -5; then
    echo -e "${GREEN}✓ TAC tests passed${NC}"
else
    echo -e "${RED}✗ TAC tests failed${NC}"
fi
echo ""

# Run RTL comparison
echo "Running compare_rtl.sh..."
if ./compare_rtl.sh 2>&1 | tail -5; then
    echo -e "${GREEN}✓ RTL tests passed${NC}"
else
    echo -e "${RED}✗ RTL tests failed${NC}"
fi
echo ""

echo "=========================================="
echo "All tests completed!"
echo "=========================================="
