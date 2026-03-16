#include <iostream>
#include <cstring>
#include <cstdio>
#include <string>
#include "tac.h"
using namespace std;

FILE *tac_file = stdout;
extern int show_tac;

// Global TACGenerator instance
TACGenerator tac_gen;

// TACGenerator methods
string TACGenerator::new_temp() {
    char buf[32];
    snprintf(buf, sizeof(buf), "temp%d", temp_counter++);
    return string(buf);
}

string TACGenerator::new_stemp() {
    char buf[32];
    snprintf(buf, sizeof(buf), "stemp%d", stemp_counter++);
    return string(buf);
}

string TACGenerator::new_label() {
    char buf[32];
    snprintf(buf, sizeof(buf), "Label%d", label_counter++);
    return string(buf);
}

void TACGenerator::reset() {
    temp_counter = 0;
    stemp_counter = 0;
    label_counter = 0;
}

// Pre-process pass: Assign temp IDs to all expression nodes that will generate temps
// Uses depth-first in-order traversal to assign temps in execution order
void assign_temp_ids(ASTNode *node) {
    if (!node) return;
    
    // Handle Ternary node
    if(strncmp(node->label, "Ternary:", 8) == 0) {
        // Assign to condition
        assign_temp_ids(node->left);
        
        // Assign to true part (these get temps before the NOT)
        if (node->right && strncmp(node->right->label, "True_Part", 9) == 0) {
            assign_temp_ids(node->right->left);
        }
        
        // Assign to false part (these get temps after true part, including nested ternary NOTs)
        if (node->third && strncmp(node->third->label, "False_Part", 10) == 0) {
            assign_temp_ids(node->third->left);
        }
        
        // Allocate temp for NOT of this ternary's condition (AFTER all children and nested NOT temps)
        // This ensures inner ternary NOTs are allocated before outer ternary NOTs
        int not_id = tac_gen.temp_counter++;
        node->not_temp_id = not_id;
        
        return;
    }
    
    if(strncmp(node->label, "L_Opd", 5) == 0 ||
       strncmp(node->label, "R_Opd", 5) == 0 ||
       strncmp(node->label, "RHS", 3) == 0){
        assign_temp_ids(node->left);
        return;
    }
    
    if(strcmp(node->label, ")") == 0){
        return;
    }
    
    // Check if this node generates a temp (operators and unary ops)
    bool generates_temp =
        strstr(node->label, "Plus") ||
        strstr(node->label, "Mult") ||
        strstr(node->label, "Div") ||
        strstr(node->label, "Minus") ||
        strstr(node->label, "Uminus") ||
        strstr(node->label, "GT") ||
        strstr(node->label, "GE") ||
        strstr(node->label, "LT") ||
        strstr(node->label, "LE") ||
        strstr(node->label, "EQ") ||
        strstr(node->label, "NE") ||
        strstr(node->label, "AND") ||
        strstr(node->label, "OR") ||
        strstr(node->label, "NOT");
    
    // For binary ops: process left, then right, then assign to self
    // For unary ops: process operand, then assign to self
    assign_temp_ids(node->left);
    assign_temp_ids(node->right);
    
    // Assign temp ID AFTER processing both operands
    // This ensures sequential numbering in evaluation order
    if (generates_temp && node->temp_id == -1) {
        node->temp_id = tac_gen.temp_counter++;
    }
}

// Assign temp IDs to all statements in the statement list
void assign_temps_to_statements(ASTNode *node) {
    ASTNode *curr = node;
    while(curr) {
        if(strcmp(curr->label, "Asgn:") == 0) {
            // Assign temps in RHS
            ASTNode *rhs_node = curr->right;
            if(rhs_node && strncmp(rhs_node->label, "RHS", 3) == 0){
                rhs_node = rhs_node->left;
            }
            assign_temp_ids(rhs_node);
        }
        else if(strncmp(curr->label, "Read", 4) == 0) {
            // No temps for read
        }
        else if(strcmp(curr->label, "Write:") == 0) {
            // Assign temps in write expression
            assign_temp_ids(curr->left);
        }
        
        curr = curr->right;
    }
}

// Pre-allocate stemp IDs for all ternary nodes (pre-order traversal)
// This ensures outer ternaries get lower stemp IDs
void assign_stemp_ids(ASTNode *node) {
    if (!node) return;
    
    // If this is a ternary node, allocate stemp ID FIRST (pre-order)
    if (strncmp(node->label, "Ternary:", 8) == 0) {
        if (node->stemp_id == -1) {
            node->stemp_id = tac_gen.stemp_counter++;
        }
        
        // Then recursively process condition (may contain ternaries)
        assign_stemp_ids(node->left);
        
        // Recursively process true part
        if (node->right && strncmp(node->right->label, "True_Part", 9) == 0) {
            assign_stemp_ids(node->right->left);
        } else {
            assign_stemp_ids(node->right);
        }
        
        // Recursively process false part
        if (node->third && strncmp(node->third->label, "False_Part", 10) == 0) {
            assign_stemp_ids(node->third->left);
        } else {
            assign_stemp_ids(node->third);
        }
        return;
    }
    
    // For non-ternary nodes, recursively process children
    assign_stemp_ids(node->left);
    assign_stemp_ids(node->right);
    assign_stemp_ids(node->third);
}

string get_temp_name(ASTNode *node) {
    if (!node || node->temp_id == -1) return "";
    char buf[32];
    snprintf(buf, sizeof(buf), "temp%d", node->temp_id);
    return string(buf);
}

string get_stemp_name(ASTNode *node) {
    if (!node || node->stemp_id == -1) return "";
    char buf[32];
    snprintf(buf, sizeof(buf), "stemp%d", node->stemp_id);
    return string(buf);
}

// Helper functions
string get_clean_name(string label){
    size_t start = label.find(":") + 2;
    size_t end = label.find("<");
    
    if(end == string::npos)
        end = label.length();
    string result = label.substr(start, end-start);
    
    size_t last = result.find_last_not_of(" ");
    if(last != string::npos) result = result.substr(0, last+1);
    
    return result;
}

string get_val(ASTNode* node){
    if(!node) return "";
    string s = node->label;
    
    size_t start = s.find(":") + 2;
    size_t end = s.find("<");
    if(end == string::npos) end = s.length();
    
    string val = s.substr(start, end-start);
    size_t last = val.find_last_not_of(" ");
    if(last != string::npos) val = val.substr(0, last+1);
    
    return val;
}

string get_clean_val(ASTNode* node){
    if(!node) return "";
    string s = node->label;
    
    size_t colon_pos = s.find(":");
    size_t angle_pos = s.find("<");
    
    if(colon_pos == string::npos) return s;
    
    size_t start = colon_pos + 1;
    size_t end = (angle_pos == string::npos) ? s.length() : angle_pos;
    
    string val = s.substr(start, end-start);
    
    size_t first = val.find_first_not_of(" ");
    size_t last = val.find_last_not_of(" ");
    
    if(first != string::npos && last != string::npos){
        val = val.substr(first, last-first+1);
    }
    else if(first != string::npos){
        val = val.substr(first);
    }
    
    return val;
}

// Forward declaration
string gen_expr(ASTNode *node);

// Handle ternary operator (condition ? true_expr : false_expr)
string gen_ternary(ASTNode *node) {
    if (!node) return "";
    
    // node->left = condition
    // node->right = true_part
    // node->third = false_part
    
    // Evaluate condition
    string cond_result = gen_expr(node->left);
    
    // Generate labels
    string false_label = tac_gen.new_label();
    string end_label = tac_gen.new_label();
    
    // Use pre-allocated NOT temp ID
    string cond_neg;
    if (node->not_temp_id != -1) {
        char buf[32];
        snprintf(buf, sizeof(buf), "temp%d", node->not_temp_id);
        cond_neg = string(buf);
    } else {
        // Fallback (shouldn't happen if assign_temps_to_statements was called)
        cond_neg = tac_gen.new_temp();
    }

    fprintf(tac_file, "%s = ! %s\n", cond_neg.c_str(), cond_result.c_str());
    fprintf(tac_file, "if(%s) goto %s\n", cond_neg.c_str(), false_label.c_str());
    
    // True branch - evaluate true expression (node->right is True_Part node)
    ASTNode *true_part_node = node->right;
    ASTNode *true_expr = NULL;
    if (true_part_node && strncmp(true_part_node->label, "True_Part", 9) == 0) {
        true_expr = true_part_node->left;
    } else {
        true_expr = true_part_node;
    }
    
    string true_result = gen_expr(true_expr);
    // Use pre-allocated stemp ID
    string result = get_stemp_name(node);
    if (result.empty()) {
        // Fallback (shouldn't happen if assign_temps_to_statements was called)
        result = tac_gen.new_stemp();
    }
    fprintf(tac_file, "%s = %s\n", result.c_str(), true_result.c_str());
    fprintf(tac_file, "goto %s\n", end_label.c_str());
    
    // False label
    fprintf(tac_file, "%s:\n", false_label.c_str());
    
    // False branch - evaluate false expression (node->third is False_Part node)
    ASTNode *false_part_node = node->third;
    ASTNode *false_expr = NULL;
    if (false_part_node && strncmp(false_part_node->label, "False_Part", 10) == 0) {
        false_expr = false_part_node->left;
    } else {
        false_expr = false_part_node;
    }
    
    string false_result = gen_expr(false_expr);
    fprintf(tac_file, "%s = %s\n", result.c_str(), false_result.c_str());
    
    // End label
    fprintf(tac_file, "%s:\n", end_label.c_str());
    
    return result;
}

string gen_expr(ASTNode *node){
    if(node == NULL) return "";
    
    // Handle Ternary node
    if(strncmp(node->label, "Ternary:", 8) == 0) {
        return gen_ternary(node);
    }
    
    if(strncmp(node->label, "L_Opd", 5) == 0 ||
       strncmp(node->label, "R_Opd", 5) == 0 ||
       strncmp(node->label, "RHS", 3) == 0){
        return gen_expr(node->left);
    }
    
    if(strcmp(node->label, ")") == 0){
        return ")";
    }
    
    bool left_is_closing = (node->left == NULL) || (node->left && strcmp(node->left->label, ")") == 0);
    bool right_is_closing = (node->right == NULL) || (node->right && strcmp(node->right->label, ")") == 0);
    
    if(left_is_closing && right_is_closing){
        return get_clean_val(node);
    }
    
    string left = gen_expr(node->left);
    string right = gen_expr(node->right);
    
    // Use pre-assigned temp ID
    string temp = get_temp_name(node);
    if (temp.empty()) {
        // Fallback (shouldn't happen if assign_temp_ids was called)
        temp = tac_gen.new_temp();
    }
    
    if(strstr(node->label, "Plus"))
        fprintf(tac_file, "%s = %s + %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "Mult"))
        fprintf(tac_file, "%s = %s * %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "Div"))
        fprintf(tac_file, "%s = %s / %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "Minus") && !strstr(node->label, "Uminus"))
        fprintf(tac_file, "%s = %s - %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "Uminus"))
        fprintf(tac_file, "%s = - %s\n", temp.c_str(), left.c_str());
    else if(strstr(node->label, "NOT"))
        fprintf(tac_file, "%s = ! %s\n", temp.c_str(), left.c_str());
    // Comparison operators
    else if(strstr(node->label, "GT"))
        fprintf(tac_file, "%s = %s > %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "GE"))
        fprintf(tac_file, "%s = %s >= %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "LT"))
        fprintf(tac_file, "%s = %s < %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "LE"))
        fprintf(tac_file, "%s = %s <= %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "EQ"))
        fprintf(tac_file, "%s = %s == %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "NE"))
        fprintf(tac_file, "%s = %s != %s\n", temp.c_str(), left.c_str(), right.c_str());
    // Logical operators
    else if(strstr(node->label, "AND"))
        fprintf(tac_file, "%s = %s && %s\n", temp.c_str(), left.c_str(), right.c_str());
    else if(strstr(node->label, "OR"))
        fprintf(tac_file, "%s = %s || %s\n", temp.c_str(), left.c_str(), right.c_str());
        
    return temp;
}

void gen_assign(ASTNode *node){
    string lhs = get_clean_val(node->left->left);
    ASTNode *rhs_node = node->right;
    if(rhs_node && strncmp(rhs_node->label, "RHS", 3) == 0){
        rhs_node = rhs_node->left;
    }
    
    // Check if this is a ternary assignment
    if (rhs_node && strncmp(rhs_node->label, "Ternary", 7) == 0) {
        string result = gen_ternary(rhs_node);
        fprintf(tac_file, "%s = %s\n", lhs.c_str(), result.c_str());
    } else {
        string rhs = gen_expr(rhs_node);
        fprintf(tac_file, "%s = %s\n", lhs.c_str(), rhs.c_str());
    }
}

void gen_read(ASTNode *node) {
    string s = node->label;
    
    // Find the second colon (after "Read: Name ")
    size_t first_colon = s.find(":");
    if (first_colon != string::npos) {
        size_t second_colon = s.find(":", first_colon + 1);
        if (second_colon != string::npos) {
            size_t start = second_colon + 1;
            size_t end = s.find("<");
            
            if (end != string::npos && start < end) {
                string name = s.substr(start, end - start);
                
                // Trim whitespace
                size_t first = name.find_first_not_of(" ");
                size_t last = name.find_last_not_of(" ");
                
                if (first != string::npos && last != string::npos) {
                    name = name.substr(first, last - first + 1);
                }
                
                fprintf(tac_file, "read %s\n", name.c_str());
                return;
            }
        }
    }
    
    fprintf(tac_file, "read unknown\n");
}

void gen_write(ASTNode *node){
    string val = gen_expr(node->left);
    fprintf(tac_file, "write %s\n", val.c_str());
}

void traverse(ASTNode *node){
    ASTNode *curr = node;
    while(curr){
        if(strcmp(curr->label, "Asgn:") == 0)
            gen_assign(curr);
        else if(strncmp(curr->label, "Read", 4) == 0)
            gen_read(curr);
        else if(strcmp(curr->label, "Write:") == 0)
            gen_write(curr);
            
        curr = curr->right;
    }
}

void generate_TAC(ASTNode *root){
    tac_gen.reset();
    
    // Get the statement list
    ASTNode *curr = root->left->right->right->left;
    
    // Only output TAC if there are actual statements
    if(curr != NULL && show_tac) {
        // Pre-pass: Assign temp IDs to all expression nodes
        assign_temps_to_statements(curr);
        
        // Pre-pass: Assign stemp IDs to all ternary nodes
        ASTNode *stmt = curr;
        while(stmt) {
            if(strcmp(stmt->label, "Asgn:") == 0) {
                ASTNode *rhs_node = stmt->right;
                if(rhs_node && strncmp(rhs_node->label, "RHS", 3) == 0){
                    rhs_node = rhs_node->left;
                }
                assign_stemp_ids(rhs_node);
            }
            else if(strcmp(stmt->label, "Write:") == 0) {
                assign_stemp_ids(stmt->left);
            }
            stmt = stmt->right;
        }
        
        fprintf(tac_file, "**PROCEDURE: main\n");
        fprintf(tac_file, "**BEGIN: Three Address Code Statements\n");
        
        traverse(curr);
        
        fprintf(tac_file, "**END: Three Address Code Statements\n");
    }
}