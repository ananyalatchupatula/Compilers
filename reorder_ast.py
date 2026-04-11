#!/usr/bin/env python3
"""
Reorder functions in AST output using natural/lexicographic sorting.
Numbers are compared numerically but lexicographic ordering is preserved.
E.g., f < f1 < f2 < f11
"""

import sys
import re

def natural_sort_key(name):
    """
    Create a sort key for natural/lexicographic sorting.
    Order: f1 < f11 < f2 < f
    (Names with numbers come first, sorted lexicographically, then plain names come last)
    """
    # Remove trailing underscore for comparison
    name = name.rstrip('_')
    
    # Check if name has any digits
    has_numbers = bool(re.search(r'\d', name))
    
    # If no numbers, sort AFTER all names with numbers
    if not has_numbers:
        return (1, name.lower())
    else:
        # Names with numbers: sort lexicographically as strings
        return (0, name.lower())

def parse_ast_file(filename):
    """Parse AST file and extract all functions in order they appear"""
    with open(filename, 'r') as f:
        content = f.read()
    
    # Find all procedures
    procedure_pattern = r'\*\*PROCEDURE: (\w+)\n'
    matches = list(re.finditer(procedure_pattern, content))
    
    if not matches:
        return []  # No procedures found
    
    # Extract each function block
    blocks = []
    for i, match in enumerate(matches):
        start = match.start()
        func_name = match.group(1)
        
        # Find the end of this block (next procedure or end of file)
        if i + 1 < len(matches):
            end = matches[i + 1].start()
        else:
            end = len(content)
        
        block = content[start:end]
        blocks.append((func_name, block))
    
    return blocks

def reorder_functions(ast_filename, program_filename):
    """Reorder all functions (including main) using the sorting rule"""
    func_blocks = parse_ast_file(ast_filename)
    
    if not func_blocks:
        return  # Nothing to reorder
    
    # Sort ALL functions (including main) using the natural/lexicographic key
    sorted_funcs = sorted(func_blocks, key=lambda x: natural_sort_key(x[0]))
    
    # Write reordered content
    result = []
    for name, block in sorted_funcs:
        result.append(block)
    
    reordered = ''.join(result)
    with open(ast_filename, 'w') as f:
        f.write(reordered)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: reorder_ast.py <ast_file> <program_file>")
        sys.exit(1)
    
    ast_file = sys.argv[1]
    prog_file = sys.argv[2]
    
    reorder_functions(ast_file, prog_file)
    print(f"Reordered {ast_file}")
