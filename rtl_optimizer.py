#!/usr/bin/env python3
"""
RTL Post-processor: Removes unnecessary temp variable load/store sequences
"""

import sys
import re

def optimize_rtl(lines):
    """Remove unnecessary temp variable stores and loads"""
    optimized = []
    temp_stores = {}  # Track temp variables and what was stored to them
    
    i = 0
    while i < len(lines):
        line = lines[i]
        
        # Track store operations to temp variables
        store_match = re.match(r'\s*store:\s+(\w+)\s+<-\s+(\w+)', line)
        if store_match:
            dest_var = store_match.group(1)
            src_reg = store_match.group(2)
            
            # Check if next line is loading from this temp variable
            if i + 1 < len(lines):
                next_line = lines[i + 1]
                load_match = re.match(r'\s*load:\s+(\w+)\s+<-\s+' + dest_var, next_line)
                
                if load_match:
                    dest_reg = load_match.group(1)
                    
                    # Check if the line after that uses dest_reg but references dest_var
                    # If so, we can skip the store/load and use src_reg directly
                    if i + 2 < len(lines):
                        third_line = lines[i + 2]
                        
                        # Check if this operation uses the loaded variable
                        if dest_var not in third_line and dest_reg in third_line:
                            # Skip the store and load, continue
                            i += 2
                            continue
        
        optimized.append(line)
        i += 1
    
    return optimized

def main():
    if len(sys.argv) < 2:
        print("Usage: rtl_optimizer.py <rtl_file>", file=sys.stderr)
        sys.exit(1)
    
    rtl_file = sys.argv[1]
    
    try:
        with open(rtl_file, 'r') as f:
            lines = [line.rstrip('\n') for line in f.readlines()]
        
        optimized_lines = optimize_rtl(lines)
        
        for line in optimized_lines:
            print(line)
    
    except FileNotFoundError:
        print(f"Error: File '{rtl_file}' not found", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
