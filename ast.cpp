#include "ast.h"
#include <string.h>

FILE *ast_file = NULL;
int show_ast = 0;
static int first_print = 1;

ASTNode* make_node(const char *label, DataType type,
                   ASTNode *l, ASTNode *r, ASTNode *t)
{
    ASTNode *n = (ASTNode*)malloc(sizeof(ASTNode));
    n->label = strdup(label);
    n->type = type;
    n->left = l;
    n->right = r;
    n->third = t;
    n->temp_id = -1;  // Not assigned yet
    n->stemp_id = -1; // Not assigned yet
    return n;
}

static void print_indent(int n){
    for(int i=0;i<n;i++){
        if(show_ast && ast_file)
            fprintf(ast_file," ");
        else
            printf(" ");
    }
}

static void print_label(const char *label, int indent, int new_line_before){
    if(new_line_before)
        print_indent(indent);
    if(show_ast && ast_file)
        fprintf(ast_file,"%s",label);
    else
        printf("%s",label);
}

void print_ast(ASTNode *n,int indent){
    if(!n) return;
    // int len = strlen(n->label);

    if(strcmp(n->label,")")==0){
        if(show_ast && ast_file)
            fprintf(ast_file,")");
        else
            printf(")");
        return;
    }

    int is_true_leaf =
        n->left &&
        n->left->left == NULL &&
        n->left->right == NULL &&
        (
            strncmp(n->left->label,"Name",4) == 0 ||
            strncmp(n->left->label,"Num",3) == 0 ||
            strncmp(n->left->label,"String",6) == 0
        );

    int is_operand_wrapper =
        strcmp(n->label,"L_Opd (") == 0 ||
        strcmp(n->label,"R_Opd (") == 0 ||
        strcmp(n->label,"True_Part (") == 0 ||
        strcmp(n->label,"False_Part (") == 0;

    if(is_operand_wrapper && is_true_leaf){
        print_indent(indent);
        if(show_ast && ast_file)
            fprintf(ast_file,"%s%s)",n->label,n->left->label);
        else
            printf("%s%s)",n->label,n->left->label);

        print_ast(n->right,indent);
        print_ast(n->third,indent);
        return;
    }

    if(strcmp(n->label,"Write:") == 0 && n->left && n->left->left == NULL && n->left->right == NULL){
        print_indent(indent);
        if(show_ast && ast_file){
            fprintf(ast_file,"\n");
            fprintf(ast_file,"Write: %s",n->left->label);
        }
        else{
            fprintf(ast_file,"\n");
            printf("Write: %s",n->left->label);
        }

        print_ast(n->right,indent);
        print_ast(n->third,indent);
        return;
    }

    int inline_node =
        strncmp(n->label,"Name",4) == 0 ||
        strncmp(n->label,"Num",3) == 0 ||
        strncmp(n->label,"String",6) == 0;

    if(inline_node){
        print_label(n->label,indent,0);
    }
    else{
        if(!first_print){
            if(show_ast && ast_file)
                fprintf(ast_file,"\n");
            else
                printf("\n");
        }
        first_print = 0;
        print_label(n->label,indent,1);
    }

    print_ast(n->left,indent+1);
    print_ast(n->right,indent+1);
    print_ast(n->third,indent+1);
}

void type_error(const char *msg){
    if(show_ast && ast_file)
        fprintf(ast_file,"Type error: %s\n",msg);
    else
        printf("Type error: %s\n",msg);
    exit(1);
}