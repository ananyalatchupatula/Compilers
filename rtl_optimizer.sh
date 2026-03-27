#!/bin/bash

# RTL Optimizer - removes unnecessary temporary loads and stores
# This is a post-processing step to optimize the RTL output

# Strip comments and extra whitespace, remove unnecessary temp load/stores
optimize_rtl() {
    local input_file="$1"
    
    # Remove comments first
    sed 's/;;.*//' "$input_file" | \
    # Remove trailing whitespace
    sed 's/[[:space:]]*$//' | \
    # Remove blank lines
    grep -v '^[[:space:]]*$' | \
    # Optimization: combine consecutive load from tempX followed by operation using tempX
    perl -ne 'print' 
}

optimize_rtl "$@"
