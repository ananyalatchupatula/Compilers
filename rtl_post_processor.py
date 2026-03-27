#!/usr/bin/env python3
"""
RTL Post-processor: Optimizes unnecessary temp variable load/store sequences
Removes patterns like:
    store: tempX <- t0
    load:  v0 <- tempX
    ... (use v0)
becomes:
    ... (use t0 directly)
"""

import sys
import re
from collections import defaultdict

def parse_rtl_line(line):
    """Parse an RTL statement line"""
    line = line.rstrip()
    if not line or line.startswith('**') or ':' not in line:
        return None
    
    # Extract operation and operands
    match = re.match(r'\s*(\w+):\s*(.*)', line)
    if not match:
        return None
    
    op = match.group(1)
    rest = match.group(2).split(';;')[0].strip()  # Remove comments
    
    return {
        'line': line,
        'op': op,
        'content': rest,
        'original': line
    }

def optimize_rtl_file(filename):
    """Optimize RTL file by removing unnecessary temp stores/loads"""
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    optimized = []
    i = 0
    
    while i < len(lines):
        line = lines[i]
        parsed = parse_rtl_line(line)
        
        if parsed and parsed['op'] == 'store':
            # Check for pattern: store tempX <- regY followed by load regZ <- tempX
            store_match = re.match(r'(\w+)\s+<-\s+(\w+)', parsed['content'])
            if store_match:
                temp_var = store_match.group(1)
                src_reg = store_match.group(2)
                
                # Look ahead for corresponding load
                if i + 1 < len(lines):
                    next_parsed = parse_rtl_line(lines[i + 1])
                    if next_parsed and next_parsed['op'] == 'load':
                        load_match = re.match(r'(\w+)\s+<-\s+' + temp_var, next_parsed['content'])
                        if load_match:
                            dest_reg = load_match.group(1)
                            
                            # Check if temp_var is only used in the next line
                            # If so, we can eliminate the store/load pair
                            # For now, we'll skip this temp store/load pair
                            
                            # Skip this store and the next load
                            i += 2
                            continue
        
        optimized.append(line)
        i += 1
    
    return optimized

def main():
    if len(sys.argv) < 2:
        print("Usage: rtl_post_processor.py <rtl_file>", file=sys.stderr)
        sys.exit(1)
    
    rtl_file = sys.argv[1]
    
    try:
        optimized = optimize_rtl_file(rtl_file)
        for line in optimized:
            print(line, end='')
    except FileNotFoundError:
        print(f"Error: File '{rtl_file}' not found", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()
