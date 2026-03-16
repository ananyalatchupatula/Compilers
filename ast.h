#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <cstring>

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

struct ASTNode {
    char label[100];

    DataType type;      // ⭐ REQUIRED (parser.y uses this)

    ASTNode *left;
    ASTNode *right;
    ASTNode *third;

    int temp_id;
    int stemp_id;
    int not_temp_id;
    
};

ASTNode* create_node(char *label);
ASTNode* make_node(const char *label, DataType type,
                   ASTNode *l, ASTNode *r, ASTNode *t);



void print_ast(ASTNode *n,int indent);
void type_error(const char *msg);

#endif