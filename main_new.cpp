#include "ast_new.h"
#include "rtl_new.h"
#include "rtl_generator.h"
#include "asm_generator.h"
#include "tac_generator.h"
#include "tac_str.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <list>
#include <algorithm>
#include <utility>
#include <cctype>

using std::vector;
using std::string;
using std::list;
using std::sort;

extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;

/* Struct for deferred TAC generation (also defined in parser_new.y) */
struct DeferredFunction {
    std::string name;
    int return_type;
    Statement_Ast* body;
    int return_label_id;  // Pre-allocated label ID from func_header
};

extern vector<DeferredFunction> deferred_functions;

/* Define TYPE constants from parser */
#define TYPE_INT    1
#define TYPE_FLOAT  2
#define TYPE_STRING 3
#define TYPE_BOOL   4
#define TYPE_CHAR   5
#define TYPE_VOID   6
#define TYPE_ERROR -1

static DataType int_to_datatype(int t)
{
    switch(t) {
        case TYPE_INT:    return INT_DATA_TYPE;
        case TYPE_FLOAT:  return FLOAT_DATA_TYPE;
        case TYPE_BOOL:   return BOOL_DATA_TYPE;
        case TYPE_STRING: return STRING_DATA_TYPE;
        case TYPE_CHAR:   return CHAR_DATA_TYPE;
        case TYPE_VOID:   return VOID_DATA_TYPE;
        default:          return ERROR_DATA_TYPE;
    }
}


extern FILE *ast_file;
extern FILE *tac_file;

FILE *rtl_file = NULL;
FILE *asm_file = NULL;

FILE *tok_file = NULL;

int show_tokens = 0;
int show_ast = 0;
int show_tac = 0;
int show_rtl = 0;
int show_asm = 0;

int sa_scan = 0;
int sa_parse = 0;
int sa_ast = 0;
int sa_tac = 0;

extern int lex_error;

/* Helper function to create natural sort key matching reorder_tac.py */
pair<int, string> get_natural_sort_key(const string& name) {
    string clean_name = name;
    // Remove trailing underscore for comparison
    if (!clean_name.empty() && clean_name.back() == '_') {
        clean_name.pop_back();
    }
    
    // Check if name has any digits
    bool has_numbers = false;
    for (char c : clean_name) {
        if (isdigit(c)) {
            has_numbers = true;
            break;
        }
    }
    
    // If no numbers, sort AFTER all names with numbers
    if (!has_numbers) {
        return make_pair(1, clean_name);
    } else {
        // Names with numbers: sort lexicographically as strings
        return make_pair(0, clean_name);
    }
}

/* Process deferred functions using TAC output order for label allocation */
void process_deferred_functions()
{
    // Phase 1: Set return label IDs on compound statements
    for (size_t i = 0; i < deferred_functions.size(); i++) {
        auto& df = deferred_functions[i];
        Compound_Stmt* compound = dynamic_cast<Compound_Stmt*>(df.body);
        if (!compound) continue;
        
        // Set the return label ID that was pre-allocated during parsing
        if (df.return_label_id >= 0) {
            compound->set_return_label_id(df.return_label_id);
        }
    }
    
    // Phase 2: Create sorted list of functions by output order (to match reorder_tac.py)
    vector<size_t> sorted_indices;
    for (size_t i = 0; i < deferred_functions.size(); i++) {
        sorted_indices.push_back(i);
    }
    
    // Sort indices by function name (using natural sort key)
    sort(sorted_indices.begin(), sorted_indices.end(),
         [](size_t a, size_t b) {
            return get_natural_sort_key(deferred_functions[a].name) <
                   get_natural_sort_key(deferred_functions[b].name);
         });
    
    // Phase 3: Pre-allocate temps and control flow labels in TAC output order
    for (size_t idx : sorted_indices) {
        auto& df = deferred_functions[idx];
        TAC_Generator::get_instance()->reset_counters();
        df.body->pre_allocate_temps();
    }
    
    // Phase 4: Generate TAC in source order (original deferred_functions order)
    // NOTE: Do NOT reset counters - labels must be globally unique across all functions
    //       Return labels are already allocated from parser; control flow labels continue from there
    for (size_t i = 0; i < deferred_functions.size(); i++) {
        auto& df = deferred_functions[i];
        list<TAC_Stmt*> tac_stmts;
        DataType ret_data_type = int_to_datatype(df.return_type);
        
        df.body->generate_tac(tac_stmts);
        
        Compound_Stmt* compound = dynamic_cast<Compound_Stmt*>(df.body);
        if (!compound) {
            fprintf(stderr, "Error: Function body is not a Compound_Stmt\n");
            continue;
        }
        
        // Add return label and statement if not void
        if (ret_data_type != VOID_DATA_TYPE) {
            int ret_label_id = compound->get_return_label_id();
            int return_stemp_id = compound->get_return_stemp_id();
            
            TAC_Opd* ret_stemp;
            if (return_stemp_id >= 0) {
                ret_stemp = new Temp_TAC_Opd(return_stemp_id, ret_data_type, "stemp");
            } else {
                ret_stemp = new Temp_TAC_Opd(0, ret_data_type, "stemp");
            }
            
            Label_TAC_Opd* ret_label = new Label_TAC_Opd(ret_label_id);
            tac_stmts.push_back(new Label_TAC_Stmt(ret_label));
            tac_stmts.push_back(new Return_TAC_Stmt(ret_stemp));
        }
        
        // Print TAC
        if (show_tac && tac_file && !tac_stmts.empty()) {
            if (df.name == "main") {
                fprintf(tac_file, "**PROCEDURE: main\n");
            } else {
                fprintf(tac_file, "**PROCEDURE: %s_\n", df.name.c_str());
            }
            fprintf(tac_file, "**BEGIN: Three Address Code Statements\n");
            for (auto stmt : tac_stmts) {
                stmt->print(tac_file);
            }
            fprintf(tac_file, "**END: Three Address Code Statements\n");
        }
        
        // Generate RTL (always for ASM, but only write file if show_rtl)
        if (!tac_stmts.empty()) {
            RTL_Generator::get_instance()->reset();
            list<RTL_Stmt*> rtl_stmts =
                RTL_Generator::get_instance()->generate_rtl(tac_stmts);
            
            // Write to RTL file only if show_rtl flag is set
            if (show_rtl && rtl_file) {
                if (df.name == "main") {
                    fprintf(rtl_file, "**PROCEDURE: main\n");
                } else {
                    fprintf(rtl_file, "**PROCEDURE: %s_\n", df.name.c_str());
                }
                fprintf(rtl_file, "**BEGIN: RTL Statements\n");
                for (auto stmt : rtl_stmts) {
                    stmt->print(rtl_file);
                }
                fprintf(rtl_file, "**END: RTL Statements\n");
                fprintf(rtl_file, "\n\n");
            }
            
            // Always generate ASM from RTL - use RTL output directly since it has correct registers
            if (asm_file && !rtl_stmts.empty()) {
                // Output SPIM prologue
                fprintf(asm_file, ".text          # The .text assembler directive indicates\n");
                string func_label = (df.name == "main") ? "main" : (df.name + "_");
                fprintf(asm_file, "    .globl %s      # The following is the code (as opposed to data)\n", func_label.c_str());
                fprintf(asm_file, "%s:                # .globl makes main know to the outside of the program.\n", func_label.c_str());
                fprintf(asm_file, "# Prologue begins\n");
                fprintf(asm_file, "    sw $ra, 0($sp)    # Save the return address\n");
                fprintf(asm_file, "    sw $fp, -4($sp)   # Save the frame pointer\n");
                fprintf(asm_file, "    sub $fp, $sp, 4   # Update the frame pointer\n");
                
                // Calculate stack size - count local variables from RTL
                int stack_size = 12;
                fprintf(asm_file, "    sub $sp, $sp, %d  # Make space for the locals\n", stack_size);
                fprintf(asm_file, "# Prologue ends\n\n");
                
                // Output function body - print RTL statements (they contain MIPS-style operands)
                for (auto rtl_stmt : rtl_stmts) {
                    fprintf(asm_file, "    ");  // Indent MIPS instructions
                    rtl_stmt->print(asm_file);
                }
                
                // Output SPIM epilogue
                fprintf(asm_file, "epilogue_%s:\n", func_label.c_str());
                fprintf(asm_file, "    add $sp, $sp, %d  # Increment stack pointer for local variables\n", stack_size);
                fprintf(asm_file, "    lw $fp, -4($sp)   # Set $fp to $sp-4\n");
                fprintf(asm_file, "    lw $ra, 0($sp)    # Save ra\n");
                fprintf(asm_file, "    jr $ra            # Jump back to the called procedure\n");
                fprintf(asm_file, "# Epilogue Ends\n\n");
            }
        }
    }
}


int main(int argc, char *argv[])
{
    const char *input_file = NULL;

    ast_file = NULL;
    tac_file = NULL;
    rtl_file = NULL;

    /* parse command line arguments */

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--sa-scan") == 0)
            sa_scan = 1;

        else if (strcmp(argv[i], "--sa-parse") == 0)
            sa_parse = 1;

        else if (strcmp(argv[i], "--sa-ast") == 0)
            sa_ast = 1;

        else if (strcmp(argv[i], "--sa-tac") == 0)
            sa_tac = 1;

        else if (strcmp(argv[i], "--show-tokens") == 0)
            show_tokens = 1;

        else if (strcmp(argv[i], "--show-ast") == 0 || strcmp(argv[i], "-ast") == 0)
            show_ast = 1;

        else if (strcmp(argv[i], "--show-tac") == 0 || strcmp(argv[i], "-tac") == 0)
            show_tac = 1;

        else if (strcmp(argv[i], "--show-rtl") == 0 || strcmp(argv[i], "-rtl") == 0)
            show_rtl = 1;

        else if (strcmp(argv[i], "--show-asm") == 0 || strcmp(argv[i], "-asm") == 0) {
            show_asm = 1;
        }

        else if ((argv[i][0] != '-') && (input_file == NULL))
            input_file = argv[i];
    }

    if (input_file == NULL)
    {
        fprintf(stderr, "No input file specified\n");
        return 1;
    }

    FILE *ip_file = fopen(input_file, "r");
    if (ip_file == NULL)
    {
        fprintf(stderr, "Input file open failed: No such file or directory\n");
        return 1;
    }

    yyin = ip_file;

    if (sa_scan)
    {
        printf("Lexical Analysis Alone\n");
        int token;
        while ((token = yylex()) != 0)
            ;
        if (lex_error)
        {
            printf("Lexical Errors\n");
            return 1;
        }
        printf("Lexical Analysis Done\n");
        fclose(ip_file);
        return 0;
    }

    /* open AST file if needed */
    if (show_ast || sa_ast)
    {
        char ast_name[256];
        snprintf(ast_name, sizeof(ast_name), "%s.ast", input_file);
        ast_file = fopen(ast_name, "w");
        if (!ast_file)
        {
            perror("ast file open failed");
            return 1;
        }
    }

    /* open TAC file if needed */
    if (show_tac)
    {
        char tac_name[256];
        snprintf(tac_name, sizeof(tac_name), "%s.tac", input_file);
        tac_file = fopen(tac_name, "w");
        if (!tac_file)
        {
            perror("tac file open failed");
            return 1;
        }
    }

    /* open RTL file if needed */
    if (show_rtl)
    {
        char rtl_name[256];
        snprintf(rtl_name, sizeof(rtl_name), "%s.rtl", input_file);
        rtl_file = fopen(rtl_name, "w");
        if (!rtl_file)
        {
            perror("rtl file open failed");
            return 1;
        }
    }

    /* open ASM file - always generate .spim */
    char asm_name[256];
    snprintf(asm_name, sizeof(asm_name), "%s.spim", input_file);
    asm_file = fopen(asm_name, "w");
    if (!asm_file)
    {
        perror("asm file open failed");
        return 1;
    }

    if (sa_parse)
    {
        printf("Parsing Alone\n");
        yyparse();
        printf("Parsing Done\n");
        
        if (ast_file) fclose(ast_file);
        fclose(ip_file);
        return 0;
    }

    yyparse();
    
    // Process deferred TAC generation with proper label allocation order
    process_deferred_functions();

    if (sa_ast)
    {
        if (ast_file) fclose(ast_file);
        
        /* Reorder functions in AST file based on declaration order */
        if (show_ast || sa_ast) {
            char cmd[512];
            snprintf(cmd, sizeof(cmd), "python3 reorder_ast.py '%s.ast' '%s' 2>/dev/null", input_file, input_file);
            system(cmd);
        }
        
        fclose(ip_file);
        return 0;
    }

    if (sa_tac)
    {
        if (ast_file) fclose(ast_file);
        if (tac_file) fclose(tac_file);
        
        /* Reorder functions in AST file based on declaration order */
        if (show_ast) {
            char cmd[512];
            snprintf(cmd, sizeof(cmd), "python3 reorder_ast.py '%s.ast' '%s' 2>/dev/null", input_file, input_file);
            system(cmd);
        }
        
        fclose(ip_file);
        return 0;
    }

    if (ast_file) fclose(ast_file);
    if (tac_file) fclose(tac_file);
    if (rtl_file) fclose(rtl_file);
    if (asm_file) fclose(asm_file);
    
    /* Reorder functions in AST file */
    if (show_ast) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "python3 reorder_ast.py '%s.ast' '%s' 2>/dev/null", input_file, input_file);
        system(cmd);
    }
    
    /* Reorder functions in TAC file */
    if (show_tac) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "python3 reorder_tac.py '%s.tac' '%s' 2>/dev/null", input_file, input_file);
        system(cmd);
    }

    /* Reorder functions in RTL file */
    if (show_rtl) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd), "python3 reorder_rtl.py '%s.rtl' '%s' 2>/dev/null", input_file, input_file);
        system(cmd);
    }
    
    fclose(ip_file);
    return 0;
}
