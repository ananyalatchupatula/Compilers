#include "ast.h"
#include <stdio.h>
#include <string.h>
extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;

extern FILE *ast_file;
extern FILE *tac_file;

FILE *tok_file = NULL;

int show_tokens = 0;
int show_ast = 0;
int show_tac = 0;

int sa_scan = 0;
int sa_parse = 0;
int sa_ast = 0;

extern int lex_error;

int main(int argc, char *argv[])
{
    const char *input_file = NULL;

    ast_file = NULL;
    tac_file = NULL;

    /* parse command line arguments */

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "--sa-scan") == 0)
            sa_scan = 1;

        else if (strcmp(argv[i], "--sa-parse") == 0)
            sa_parse = 1;

        else if (strcmp(argv[i], "--sa-ast") == 0)
            sa_ast = 1;

        else if (strcmp(argv[i], "--show-tokens") == 0)
            show_tokens = 1;

        else if (strcmp(argv[i], "--show-ast") == 0)
            show_ast = 1;

        else if (strcmp(argv[i], "--show-tac") == 0)
            show_tac = 1;

        else
            input_file = argv[i];
    }

    /* stage conflict checks */

    if (sa_scan && (sa_parse || sa_ast || show_ast || show_tac))
    {
        fprintf(stderr, "Invalid option combination with --sa-scan\n");
        return 1;
    }

    if (sa_parse && (sa_ast || show_ast || show_tac))
    {
        fprintf(stderr, "Invalid option combination with --sa-parse\n");
        return 1;
    }

    if (sa_ast && show_tac)
    {
        fprintf(stderr, "--sa-ast cannot be used with --show-tac\n");
        return 1;
    }

    if (!input_file)
    {
        fprintf(stderr, "No input file provided\n");
        return 1;
    }

    /* open input file */

    yyin = fopen(input_file, "r");
    if (!yyin)
    {
        perror("Input file open failed");
        return 1;
    }

    /* open tokens file if needed */

    if (show_tokens)
    {
        char toks_name[256];

        snprintf(toks_name, sizeof(toks_name), "%s.toks", input_file);

        tok_file = fopen(toks_name, "w");

        if (!tok_file)
        {
            perror("toks file open failed");
            return 1;
        }
    }

    /* SCANNING ONLY */

    if (sa_scan)
    {
        while (yylex() != 0);

        int result = lex_error ? 1 : 0;

        if (tok_file)
            fclose(tok_file);

        fclose(yyin);

        return result;
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



    /* PARSING */

    int parse_result = yyparse();

    if (parse_result != 0)
    {
        if (tok_file)
            fclose(tok_file);

        if (ast_file)
            fclose(ast_file);

        if (tac_file)
            fclose(tac_file);

        fclose(yyin);

        return parse_result;
    }

    /* stop after parsing */

    if (sa_parse)
    {
        if (tok_file)
            fclose(tok_file);

        if (ast_file)
            fclose(ast_file);

        fclose(yyin);

        return 0;
    }

    /* stop after AST */

    if (sa_ast)
    {
        if (tok_file)
            fclose(tok_file);

        if (ast_file)
            fclose(ast_file);

        fclose(yyin);

        return 0;
    }

    /* TAC generation happens inside parser/AST code if show_tac enabled */

    if (tok_file)
        fclose(tok_file);

    if (ast_file)
        fclose(ast_file);

    if (tac_file)
        fclose(tac_file);
 

    fclose(yyin);

    return 0;
}