#include "ast.h"
#include <stdio.h>
#include <string.h>

extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;
extern FILE *tac_file;
int show_tac = 0;

FILE *tok_file = NULL;
int show_tokens = 0;
int sa_scan = 0;

extern int lex_error;

int main(int argc, char *argv[]) {
    const char *input_file = NULL;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--sa-scan") == 0) {
            sa_scan = 1;
        }
        else if (strcmp(argv[i], "--show-tokens") == 0) {
            show_tokens = 1;
        }
        else if (strcmp(argv[i], "--show-ast") == 0) {
            show_ast = 1;
        }
        else if (strcmp(argv[i], "--show-tac") == 0) {
            show_tac = 1;
        }

        else {
            input_file = argv[i];
        }
    }

    if (!input_file) {
        fprintf(stderr, "No input file provided\n");
        return 1;
    }

    yyin = fopen(input_file, "r");
    if (!yyin) {
        perror("Input file open failed");
        return 1;
    }

    if (show_tokens) {
        char toks_name[256];
        snprintf(toks_name, sizeof(toks_name), "%s.toks", input_file);
        tok_file = fopen(toks_name, "w");
        if (!tok_file) {
            perror("toks file open failed");
            return 1;
        }
    }

    if (sa_scan) {
        while (yylex() != 0);
        if (tok_file) fclose(tok_file);
        fclose(yyin);
        return lex_error ? 1 : 0;
    }

    if (show_ast) {
        char ast_name[256];
        snprintf(ast_name, sizeof(ast_name), "%s.ast", input_file);

        ast_file = fopen(ast_name, "w");
        if (!ast_file) {
            perror("ast file open failed");
            return 1;
        }
    }

    if (show_tac) {
    char tac_name[256];
    snprintf(tac_name, sizeof(tac_name), "%s.tac", input_file);

    tac_file = fopen(tac_name, "w");
    if (!tac_file) {
        perror("tac file open failed");
        return 1;
    }
    }   


    int parse_result = yyparse();
    if (tok_file) fclose(tok_file);
    if (ast_file) fclose(ast_file);
    if (tac_file) fclose(tac_file);
    fclose(yyin);

    return parse_result;
}
