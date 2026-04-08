#include "ast_new.h"
#include "rtl_new.h"
#include "rtl_generator.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;

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
