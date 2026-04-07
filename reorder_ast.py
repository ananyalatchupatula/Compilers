#!/usr/bin/env python3
"""
Reorder functions in AST output to match declaration order.
Reads an AST file and reorders function blocks based on their first appearance in a program.
"""

import sys
import re

def parse_ast_file(filename):
    """Parse AST file and extract functions in order they appear"""
    with open(filename, 'r') as f:
        content = f.read()
    
    # Split by "**PROCEDURE:" to identify function blocks
    # But we need to be careful about the structure
    functions = []
    main_block = None
    
    # Find all procedures
    procedure_pattern = r'\*\*PROCEDURE: (\w+)\n'
    matches = list(re.finditer(procedure_pattern, content))
    
    if not matches:
        return content  # No procedures found, return as is
    
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
        
        if func_name == "main":
            main_block = block
        else:
            blocks.append((func_name, block))
    
    return main_block, blocks

def get_function_declaration_order(program_filename):
    """Extract function declaration order from program"""
    try:
        with open(program_filename, 'r') as f:
            content = f.read()
    except:
        return []  # Can't determine order
    
    # Extract function declarations (both forward decls and definitions)
    # Look for patterns: type name(...) ;  or type name(...) {
    decl_pattern = r'(?:void|int|float|bool|char|string)\s+(\w+)\s*\('
    matches = re.findall(decl_pattern, content)
    
    # Remove duplicates while preserving order
    seen = set()
    order = []
    for name in matches:
        if name not in seen and name != "main":
            seen.add(name)
            order.append(name)
    
    return order

def reorder_functions(ast_filename, program_filename):
    """Reorder functions in AST file based on declaration order"""
    main_block, func_blocks = parse_ast_file(ast_filename)
    
    if not main_block and not func_blocks:
        return  # Nothing to reorder
    
    # Get declaration order from program
    order = get_function_declaration_order(program_filename)
    
    # Build result: functions in declaration order, then main at the end
    result = []
    
    # Create dict with underscore-stripped names as keys
    func_dict = {}
    for name, block in func_blocks:
        # Remove trailing underscore for matching
        clean_name = name.rstrip('_')
        func_dict[clean_name] = (name, block)
    
    # Add functions in the order they were declared (excluding main)
    for func_name in order:
        if func_name in func_dict:
            _, block = func_dict[func_name]
            result.append(block)
    
    # Add any remaining non-main functions not in order
    added = set(order)
    for name, block in func_blocks:
        clean_name = name.rstrip('_')
        if clean_name not in added:
            result.append(block)
    
    # Add main at the end
    if main_block:
        result.append(main_block)
    
    # Write reordered content
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
    print(f"Reordered {ast_file} based on {prog_file}")
