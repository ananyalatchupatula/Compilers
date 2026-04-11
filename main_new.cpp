#include "ast_new.h"
#include "rtl_new.h"
#include "rtl_generator.h"
#include "tac_generator.h"
#include "tac_str.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <list>
#include <algorithm>

using std::vector;
using std::string;
using std::list;
using std::sort;

extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;

/* Struct for deferred TAC generation */
struct DeferredFunction {
    string name;
    int return_type;
    Statement_Ast* body;
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

FILE *tok_file = NULL;

int show_tokens = 0;
int show_ast = 0;
int show_tac = 0;
int show_rtl = 0;

int sa_scan = 0;
int sa_parse = 0;
int sa_ast = 0;
int sa_tac = 0;

extern int lex_error;

/* Process deferred functions: pre_allocate_temps for all, then generate_tac for all */
void process_deferred_functions()
{
    // Find main's index
    int main_index = -1;
    for (int i = 0; i < deferred_functions.size(); i++) {
        if (deferred_functions[i].name == "main") {
            main_index = i;
            break;
        }
    }
    
    // Create a sorted list of non-main functions
    // Reorder_tac.py sorts functions lexicographically, with main first
    // So we process main first, then others in alphabetical order
    
    // Process main first (if it exists)
    if (main_index >= 0) {
        TAC_Generator::get_instance()->reset_counters();
        deferred_functions[main_index].body->pre_allocate_temps();
    }
    
    // Collect non-main functions and sort them alphabetically
    vector<pair<string, int>> others;  // (name, index)
    for (int i = 0; i < deferred_functions.size(); i++) {
        if (i != main_index) {
            others.push_back({deferred_functions[i].name, i});
        }
    }
    sort(others.begin(), others.end());  // Sort by name
    
    // Process non-main functions in alphabetical order
    for (auto& [name, idx] : others) {
        TAC_Generator::get_instance()->reset_counters();
        deferred_functions[idx].body->pre_allocate_temps();
    }
    
    // Generate TAC in declaration order (as stored in deferred_functions)
    for (auto& df : deferred_functions) {
        list<TAC_Stmt*> tac_stmts;
        DataType ret_data_type = int_to_datatype(df.return_type);
        
        TAC_Generator::get_instance()->reset_counters();
        df.body->generate_tac(tac_stmts);
        
        // Cast body to Compound_Stmt for access to return methods
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
        
        // Generate RTL
        if (show_rtl && rtl_file && !tac_stmts.empty()) {
            RTL_Generator::get_instance()->reset();
            list<RTL_Stmt*> rtl_stmts =
                RTL_Generator::get_instance()->generate_rtl(tac_stmts);
            
            fprintf(rtl_file, "**PROCEDURE: %s\n", df.name.c_str());
            fprintf(rtl_file, "**BEGIN: RTL Statements\n");
            for (auto stmt : rtl_stmts) {
                stmt->print(rtl_file);
            }
            fprintf(rtl_file, "**END: RTL Statements\n");
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
    
    fclose(ip_file);
    return 0;
}
