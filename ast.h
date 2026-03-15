#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

extern FILE *ast_file;
extern int show_ast;

typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_CHAR,
    TYPE_VOID,
    TYPE_ERROR,
} DataType;

typedef struct ASTNode {
    const char *label;
    DataType type;
    struct ASTNode *left;
    struct ASTNode *right;
    struct ASTNode *third;
    int temp_id;  // Assigned during TAC preparation
    int stemp_id; // Assigned during TAC preparation for statement temporaries
} ASTNode;

ASTNode* make_node(const char *label, DataType type,
                   ASTNode *l, ASTNode *r, ASTNode *t);

void print_ast(ASTNode *n,int indent);
void type_error(const char *msg);

#endif