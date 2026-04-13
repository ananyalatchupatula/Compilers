#!/bin/bash

echo "===== MAIN DECLARATION/DEFINITION VALIDATION TESTS ====="
echo ""

tests=(
    "test_main_matching.c:Matching params|SUCCESS"
    "test_main_mismatch.c:Type mismatch|type mismatch"
    "test_main_char.c:Char in declaration|cant parse"
    "test_main_no_decl.c:No declaration|SUCCESS"
    "test_main_name_mismatch.c:Name mismatch|name mismatch"
    "test_main_char_def_only.c:Char in def only|cant parse"
    "test_main_no_params.c:No params both|SUCCESS"
    "test_main_param_count_mismatch.c:Param count mismatch|do not match"
)

for test in "${tests[@]}"; do
    IFS='|' read -r file desc expected <<< "$test"
    echo "Test: $desc"
    echo "File: $file"
    output=$(./compiler "$file" 2>&1)
    if echo "$output" | grep -q "$expected"; then
        echo "✓ PASS"
    else
        echo "✗ FAIL - Got: $output"
    fi
    echo ""
done
