%option yylineno
%option noyywrap
%option nounput
%option noinput
%{
#include <stdio.h>
#include <stdlib.h>
#include "ast_new.h"
#include "parser_new.tab.hpp"
#include <cstring>
#include <cstdlib>

extern int show_tokens;
extern FILE *tok_file;

FILE *ast_file = NULL;
FILE *tac_file = NULL;

int token_dump_done = 0;
int lex_error = 0;
%}

%%

"int" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: INTEGER\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return INTEGER;
}

"string" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: STRING\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return STRING;
}

"bool" {
    if (show_tokens)
        fprintf(tok_file, "\tToken Name: BOOL\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return BOOL;
}

"float" {
    if (show_tokens)
        fprintf(tok_file, "\tToken Name: FLOAT\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return FLOAT;
}

"char" {
    if (show_tokens)
        fprintf(tok_file, "\tToken Name: CHAR\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return CHAR;
}

"void" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: VOID\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return VOID;
}

"read" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: READ\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return READ;
}

"print" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: WRITE\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return WRITE;
}

"write" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: WRITE\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return WRITE;
}

"if" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: IF\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return IF;
}

"else" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: ELSE\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return ELSE;
}

"while" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: WHILE\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return WHILE;
}

"do" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: DO\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return DO;
}

"return" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: RETURN\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return RETURN;
}

"==" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: EQ\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return EQUAL;
}

"!=" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: NE\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return NOT_EQUAL;
}

">=" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: GE\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return GREATER_THAN_EQUAL;
}
"<=" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: LE\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return LESS_THAN_EQUAL;
}

"&&" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: AND\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return AND;
}

"||" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: OR\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return OR;
}

"//".*    { /* ignore single-line comment */ }

"/*"([^*]|\*+[^*/])*\*+"/"    { /* ignore multi-line comment */ }

">" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: GT\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return GREATER_THAN;
}

"<" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: LT\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return LESS_THAN;
}

"=" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: ASSIGN_OP\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return ASSIGN_OP;
}

"?" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: QUESTION\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return QUESTION_MARK;
}

":" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return COLON;
}



"," {
     if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno); 
    return COMMA; }
";" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return SEMICOLON; }

"(" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return LEFT_ROUND_BRACKET; }
")" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return RIGHT_ROUND_BRACKET; }
"{" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return LEFT_CURLY_BRACKET; }
"}" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return RIGHT_CURLY_BRACKET; }

"+" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return PLUS; }
"-" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return MINUS; }
"*" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return MULT; }
"/" {  if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: COLON\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
        return DIV; }
"!" {
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: NOT\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return NOT;
}


[0-9]+"."[0-9]+ {
    yylval.str = strdup(yytext);
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: FLOAT_NUM\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return FLOAT_NUM;
}

[0-9]+ {
    yylval.str = strdup(yytext);
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: INT_NUM\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return INT_NUM;
}

\"([^\"\\]|\\.)*\" {
    yylval.str = strdup(yytext);
    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: STR_CONST\tLexeme: %s\tLine no: %d\n", yytext, yylineno);
    return STR_CONST;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.name = strdup(yytext);

    if (show_tokens && !token_dump_done)
        fprintf(tok_file, "\tToken Name: NAME\tLexeme: %s\tLine no: %d\n", yytext, yylineno);

    return NAME;
}


[ \t\r\n]+ ;

<<EOF>> {
    token_dump_done = 1;
    return 0;
}

. {
    fprintf(stderr, "Unknown token: %s at line %d\n", yytext, yylineno);
    lex_error = 1;
    exit(1);
}

%%
