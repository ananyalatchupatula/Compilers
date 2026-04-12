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
#include <map>
#include <set>
#include <algorithm>
#include <utility>
#include <cctype>
#include <unistd.h>

using std::vector;
using std::string;
using std::list;
using std::map;
using std::sort;

extern int yylex(void);
extern int yyparse(void);
extern FILE *yyin;

/* Struct for deferred TAC generation (also defined in parser_new.y) */
struct FuncParam {
    std::string name;
    int type;
};

struct LocalVar {
    std::string name;
    int type;
};

struct DeferredFunction {
    std::string name;
    int return_type;
    Statement_Ast* body;
    int return_label_id;  // Pre-allocated label ID from func_header
    vector<FuncParam> parameters;  // List of parameters
    vector<LocalVar> locals;       // List of local variables (excluding globals)
};

struct GlobalVar {
    std::string name;
    int type;
};

// Global map to collect string literals during RTL generation
std::map<std::string, std::string> all_string_literals;  // label -> content

extern vector<DeferredFunction> deferred_functions;
extern vector<GlobalVar> global_vars;

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

/* Helper function to check if a variable is global */
bool is_global_variable(const string& var_name) {
    // Variable names in RTL/TAC have underscores, but global_vars has names without underscores
    string clean_name = var_name;
    if (!clean_name.empty() && clean_name.back() == '_') {
        clean_name.pop_back();
    }
    
    for (const auto& gvar : global_vars) {
        if (gvar.name == clean_name) {
            return true;
        }
    }
    return false;
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

    // Prepare storage for per-function RTL so we can emit ASM
    // in a separate pass (possibly different order) without affecting RTL files.
    std::vector< std::list<RTL_Stmt*> > rtl_per_function(deferred_functions.size());
    
    // Phase 4: Generate TAC/RTL in original source order
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
        
        // Generate RTL from TAC (only if TAC is non-empty)
        list<RTL_Stmt*> rtl_stmts;
        if (!tac_stmts.empty()) {
            RTL_Generator::get_instance()->reset();
            rtl_stmts = RTL_Generator::get_instance()->generate_rtl(tac_stmts);
            // Store for later ASM generation
            rtl_per_function[i] = rtl_stmts;
            
            // Collect string literals from Loadaddr statements
            if (!rtl_stmts.empty()) {
                for (auto rtl : rtl_stmts) {
                    if (auto loadaddr = dynamic_cast<Loadaddr_RTL_Stmt*>(rtl)) {
                        if (auto src_mem = dynamic_cast<Memory_RTL_Opd*>(loadaddr->get_source())) {
                            // Extract string label
                            std::string label = src_mem->get_name();
                            
                            // Try to get the actual string from the label using TAC_Generator's map
                            std::string str_content = "";
                            if (label.find("_str_") == 0) {
                                str_content = TAC_Generator::get_instance()->get_string_literal_from_label(label);
                                // Remove quotes if they're present
                                if (!str_content.empty() && str_content[0] == '"' && str_content[str_content.length()-1] == '"') {
                                    str_content = str_content.substr(1, str_content.length() - 2);
                                }
                            }
                            
                            all_string_literals[label] = str_content;
                        }
                    }
                }
            }
            
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
        }
    }

    // Phase 5: Generate ASM in natural/lexicographic function order
    if (asm_file) {
        for (size_t idx : sorted_indices) {
            auto& df = deferred_functions[idx];
            list<RTL_Stmt*>& rtl_stmts = rtl_per_function[idx];

            // Determine return data type for this function
            DataType ret_data_type = int_to_datatype(df.return_type);
            bool is_void_func = (ret_data_type == VOID_DATA_TYPE);

            // Collect all unique variables in RTL 
            std::map<std::string, int> var_offsets;
            
            // Get parameter names from deferred function info
            std::set<std::string> param_names;
            for (const auto& param : df.parameters) {
                param_names.insert(param.name + "_");  // Add underscore suffix
            }

            // Get local (non-parameter) variable names for this function.
            std::set<std::string> local_base_names;
            for (const auto &lv : df.locals) {
                // Parameters are already tracked separately; keep everything
                // else as potential locals that can shadow globals.
                bool is_param = false;
                for (const auto &p : df.parameters) {
                    if (p.name == lv.name) { is_param = true; break; }
                }
                if (!is_param) {
                    local_base_names.insert(lv.name);
                }
            }

            auto strip_trailing_underscore = [](const std::string &name) {
                if (!name.empty() && name.back() == '_') {
                    return name.substr(0, name.size() - 1);
                }
                return name;
            };

            // Helper: within this function's scope, treat any name that
            // matches a parameter or a local declaration as non-global, even
            // if a global with the same base name exists.
            auto is_func_global = [&](const std::string &vname) -> bool {
                if (param_names.find(vname) != param_names.end()) {
                    return false;
                }
                std::string base = strip_trailing_underscore(vname);
                if (local_base_names.find(base) != local_base_names.end()) {
                    return false;
                }
                return is_global_variable(vname);
            };
            
            // Find all variables in RTL in order of first appearance
            std::vector<std::string> all_vars;
            std::set<std::string> seen_vars;
            if (!rtl_stmts.empty()) {
                for (auto rtl : rtl_stmts) {
                    if (auto load = dynamic_cast<Load_RTL_Stmt*>(rtl)) {
                        RTL_Opd* src = load->get_source();
                        if (auto mem = dynamic_cast<Memory_RTL_Opd*>(src)) {
                            string var_name = mem->get_name();
                            if (seen_vars.find(var_name) == seen_vars.end()) {
                                all_vars.push_back(var_name);
                                seen_vars.insert(var_name);
                            }
                        }
                    } else if (auto store = dynamic_cast<Store_RTL_Stmt*>(rtl)) {
                        RTL_Opd* dest = store->get_dest();
                        if (auto mem = dynamic_cast<Memory_RTL_Opd*>(dest)) {
                            string var_name = mem->get_name();
                            if (seen_vars.find(var_name) == seen_vars.end()) {
                                all_vars.push_back(var_name);
                                seen_vars.insert(var_name);
                            }
                        }
                    }
                }
            }

            // Identify a dedicated "return slot" local (for non-void functions)
            // as the last integer load into $v1 from a non-global memory operand.
            // Also, for void functions, track a "print slot" local as the last
            // integer load into $a0 from a non-global memory operand; this is the
            // value printed at the end of the function and, in the reference
            // compiler, typically lives at -4($fp).
            std::string ret_slot_var;
            std::string print_slot_var;
            if (!rtl_stmts.empty()) {
                for (auto rtl : rtl_stmts) {
                    if (auto load = dynamic_cast<Load_RTL_Stmt*>(rtl)) {
                        auto dest_reg = dynamic_cast<Register_RTL_Opd*>(load->get_dest());
                        auto src_mem  = dynamic_cast<Memory_RTL_Opd*>(load->get_source());
                        if (dest_reg && src_mem && !load->get_is_float()) {
                            std::string dst_name = dest_reg->get_name();
                            std::string var_name = src_mem->get_name();
                            if (!is_global_variable(var_name)) {
                                if (!is_void_func && dst_name == "v1") {
                                    // Keep the last such occurrence
                                    ret_slot_var = var_name;
                                }
                                if (dst_name == "a0") {
                                    // Track the last local printed via a0
                                    print_slot_var = var_name;
                                }
                            }
                        }
                    }
                }
            }
            
            // Separate locals (exclude parameters) preserving order of first appearance.
            // Discard pure globals that have no local/parameter with the same name.
            std::vector<std::string> local_vars;
            for (const auto& vname : all_vars) {
                if (param_names.find(vname) != param_names.end()) {
                    continue; // parameters handled separately
                }
                if (!is_func_global(vname)) {
                    // Exclude the dedicated return-slot variable from general locals;
                    // it will be assigned to -4($fp) separately so that user locals
                    // naturally start at -8($fp), matching the reference layout.
                    if (!ret_slot_var.empty() && vname == ret_slot_var) {
                        continue;
                    }
                    local_vars.push_back(vname);
                }
            }
            
            // Check if any local variables are floats by scanning RTL
            std::set<std::string> float_vars;
            for (auto rtl : rtl_stmts) {
                if (auto store = dynamic_cast<Store_RTL_Stmt*>(rtl)) {
                    if (auto src_reg = dynamic_cast<Register_RTL_Opd*>(store->get_source())) {
                        if (src_reg->is_float_register()) {
                            if (auto dest_mem = dynamic_cast<Memory_RTL_Opd*>(store->get_dest())) {
                                float_vars.insert(dest_mem->get_name());
                            }
                        }
                    }
                }
                else if (auto load = dynamic_cast<Load_RTL_Stmt*>(rtl)) {
                    if (auto dest_reg = dynamic_cast<Register_RTL_Opd*>(load->get_dest())) {
                        if (dest_reg->is_float_register()) {
                            if (auto src_mem = dynamic_cast<Memory_RTL_Opd*>(load->get_source())) {
                                float_vars.insert(src_mem->get_name());
                            }
                        }
                    }
                }
            }
            bool has_float_locals = !float_vars.empty();
            
            // Assign parameter offsets based on declaration order and type information
            // Parameters always start at 8($fp). Integers/bools/etc use 4 bytes, floats
            // use 8 bytes (treated as doubles in the reference implementation).
            int param_offset = 8;
            for (const auto& param : df.parameters) {
                std::string vname = param.name + "_";
                int size = (param.type == TYPE_FLOAT) ? 8 : 4;
                var_offsets[vname] = param_offset;
                param_offset += size;
            }

            // Assign locals in first-seen order. For non-void functions, reserve
            // -4($fp) exclusively for the return slot, so user locals always start
            // at -8($fp) (then -12, -16, ...). For void functions, the first local
            // can use -4($fp) unless there are float locals, in which case we start
            // at -8($fp) for alignment. Each float/double local consumes 8 bytes,
            // others consume 4 bytes.
            int local_offset;
            if (!is_void_func) {
                local_offset = -8;
            } else {
                local_offset = has_float_locals ? -8 : -4;
            }
            for (const auto &vname : local_vars) {
                bool is_float_local = (float_vars.find(vname) != float_vars.end());
                int size = is_float_local ? 8 : 4;
                var_offsets[vname] = local_offset;
                local_offset -= size;
            }

            // Ensure the dedicated return-slot variable (if any) is placed at
            // -4($fp), without disturbing the ordering of other locals that now
            // start from -8($fp) downward.
            if (!is_void_func && !ret_slot_var.empty()) {
                auto it_ret = var_offsets.find(ret_slot_var);
                if (it_ret == var_offsets.end()) {
                    // No offset assigned yet – reserve -4($fp) for the return slot.
                    var_offsets[ret_slot_var] = -4;
                } else if (it_ret->second != -4) {
                    // Fallback: if it somehow already has an offset, swap with
                    // whichever local currently owns -4($fp).
                    std::string at_minus4;
                    for (const auto &p : var_offsets) {
                        if (p.second == -4) {
                            at_minus4 = p.first;
                            break;
                        }
                    }
                    int old_offset = it_ret->second;
                    var_offsets[ret_slot_var] = -4;
                    if (!at_minus4.empty() && at_minus4 != ret_slot_var) {
                        var_offsets[at_minus4] = old_offset;
                    }
                }
            }

            // For void functions, if there is a distinguished "print slot" local
            // (the last local loaded into $a0), bias it to live at -4($fp) as in
            // the reference compiler. This keeps frequently printed locals like
            // "a" in examples such as l5-exmp4.c nearest to $fp, while other
            // locals (e.g., "b") remain at -8($fp), -12($fp), etc.
            if (is_void_func && ret_slot_var.empty() && !print_slot_var.empty()) {
                auto it_print = var_offsets.find(print_slot_var);
                if (it_print != var_offsets.end() && it_print->second != -4) {
                    std::string at_minus4;
                    for (const auto &p : var_offsets) {
                        if (p.second == -4) {
                            at_minus4 = p.first;
                            break;
                        }
                    }
                    int old_offset = it_print->second;
                    var_offsets[print_slot_var] = -4;
                    if (!at_minus4.empty() && at_minus4 != print_slot_var) {
                        var_offsets[at_minus4] = old_offset;
                    }
                }
            }
            
            // Calculate stack size based on maximum local depth
            int max_local_depth = 0;
            for (const auto& pair : var_offsets) {
                if (pair.second < 0) {
                    max_local_depth = std::max(max_local_depth, -pair.second);
                }
            }
            int stack_size = 8 + max_local_depth;

            // Output SPIM prologue for this function
            fprintf(asm_file, ".text          # The .text assembler directive indicates\n");
            string func_label = (df.name == "main") ? "main" : (df.name + "_");
            fprintf(asm_file, "    .globl %s      # The following is the code (as opposed to data)\n", func_label.c_str());
            fprintf(asm_file, "%s:                # .globl makes main know to the outside of the program.\n", func_label.c_str());
            fprintf(asm_file, "# Prologue begins\n");
            fprintf(asm_file, "    sw $ra, 0($sp)    # Save the return address\n");
            fprintf(asm_file, "    sw $fp, -4($sp)   # Save the frame pointer\n");
            fprintf(asm_file, "    sub $fp, $sp, 4   # Update the frame pointer\n");
            fprintf(asm_file, "    sub $sp, $sp, %d  # Make space for the locals\n", stack_size);
            fprintf(asm_file, "# Prologue ends\n\n");

            // Output function body if there are RTL statements
            if (!rtl_stmts.empty()) {
                // Track whether each push was of a float/double value so pops
                // can restore the correct amount of stack space (4 vs 8 bytes).
                std::vector<bool> push_is_float_stack;
                for (auto rtl : rtl_stmts) {
                    if (auto load = dynamic_cast<Load_RTL_Stmt*>(rtl)) {
                        RTL_Opd* dest = load->get_dest();
                        RTL_Opd* src = load->get_source();
                        if (auto dest_reg = dynamic_cast<Register_RTL_Opd*>(dest)) {
                            if (auto src_const = dynamic_cast<Const_RTL_Opd*>(src)) {
                                // Treat as float load if either the RTL flag or
                                // the destination register indicates float.
                                bool is_float_load = load->get_is_float() || dest_reg->is_float_register();
                                if (is_float_load) {
                                    fprintf(asm_file, "    li.d $%s, %.2f\n",
                                        dest_reg->get_name().c_str(),
                                        src_const->get_float_value());
                                } else {
                                    fprintf(asm_file, "    li $%s, %d\n",
                                        dest_reg->get_name().c_str(),
                                        src_const->get_int_value());
                                }
                            } else if (auto src_mem = dynamic_cast<Memory_RTL_Opd*>(src)) {
                                string var_name = src_mem->get_name();
                                bool is_float_load = load->get_is_float() || dest_reg->is_float_register();
                                if (is_float_load) {
                                    if (is_func_global(var_name)) {
                                        fprintf(asm_file, "    l.d $%s, %s\n",
                                            dest_reg->get_name().c_str(), var_name.c_str());
                                    } else {
                                        int offset = var_offsets[var_name];
                                        fprintf(asm_file, "    l.d $%s, %d($fp)\n",
                                            dest_reg->get_name().c_str(), offset);
                                    }
                                } else {
                                    if (is_func_global(var_name)) {
                                        fprintf(asm_file, "    lw $%s, %s\n",
                                            dest_reg->get_name().c_str(), var_name.c_str());
                                    } else {
                                        int offset = var_offsets[var_name];
                                        fprintf(asm_file, "    lw $%s, %d($fp)\n",
                                            dest_reg->get_name().c_str(), offset);
                                    }
                                }
                            }
                        }
                    }
                    else if (auto store = dynamic_cast<Store_RTL_Stmt*>(rtl)) {
                        RTL_Opd* dest = store->get_dest();
                        RTL_Opd* src = store->get_source();
                        if (auto dest_mem = dynamic_cast<Memory_RTL_Opd*>(dest)) {
                            if (auto src_reg = dynamic_cast<Register_RTL_Opd*>(src)) {
                                string var_name = dest_mem->get_name();
                                bool is_float_store = src_reg->is_float_register();
                                if (is_float_store) {
                                    if (is_func_global(var_name)) {
                                        fprintf(asm_file, "    s.d $%s, %s\n",
                                            src_reg->get_name().c_str(), var_name.c_str());
                                    } else {
                                        int offset = var_offsets[var_name];
                                        fprintf(asm_file, "    s.d $%s, %d($fp)\n",
                                            src_reg->get_name().c_str(), offset);
                                    }
                                } else {
                                    if (is_func_global(var_name)) {
                                        fprintf(asm_file, "    sw $%s, %s\n",
                                            src_reg->get_name().c_str(), var_name.c_str());
                                    } else {
                                        int offset = var_offsets[var_name];
                                        fprintf(asm_file, "    sw $%s, %d($fp)\n",
                                            src_reg->get_name().c_str(), offset);
                                    }
                                }
                            }
                        }
                    }
                    else if (auto compute = dynamic_cast<Compute_RTL_Stmt*>(rtl)) {
                        RTL_Opd* dest = compute->get_dest();
                        RTL_Opd* opd1 = compute->get_opd1();
                        RTL_Opd* opd2 = compute->get_opd2();
                        if (auto dest_reg = dynamic_cast<Register_RTL_Opd*>(dest)) {
                            if (compute->get_op() == Compute_RTL_Stmt::RTL_OP_NOT) {
                                if (auto op1_reg = dynamic_cast<Register_RTL_Opd*>(opd1)) {
                                    fprintf(asm_file, "    xori $%s, $%s, 1\n",
                                        dest_reg->get_name().c_str(),
                                        op1_reg->get_name().c_str());
                                }
                            }
                            else if (auto op1_reg = dynamic_cast<Register_RTL_Opd*>(opd1)) {
                                if (auto op2_reg = dynamic_cast<Register_RTL_Opd*>(opd2)) {
                                    const char* op_str = "add";
                                    bool is_float_compute = dest_reg->is_float_register();
                                    switch (compute->get_op()) {
                                        case Compute_RTL_Stmt::RTL_OP_ADD:
                                        case Compute_RTL_Stmt::RTL_OP_ADD_D:
                                            op_str = is_float_compute ? "add.d" : "add"; break;
                                        case Compute_RTL_Stmt::RTL_OP_SUB:
                                        case Compute_RTL_Stmt::RTL_OP_SUB_D:
                                            op_str = is_float_compute ? "sub.d" : "sub"; break;
                                        case Compute_RTL_Stmt::RTL_OP_MUL:
                                        case Compute_RTL_Stmt::RTL_OP_MUL_D:
                                            op_str = is_float_compute ? "mul.d" : "mul"; break;
                                        case Compute_RTL_Stmt::RTL_OP_DIV:
                                        case Compute_RTL_Stmt::RTL_OP_DIV_D:
                                            op_str = is_float_compute ? "div.d" : "div"; break;
                                        case Compute_RTL_Stmt::RTL_OP_SGT: op_str = "sgt"; break;
                                        case Compute_RTL_Stmt::RTL_OP_SLT: op_str = "slt"; break;
                                        case Compute_RTL_Stmt::RTL_OP_SGE: op_str = "sge"; break;
                                        case Compute_RTL_Stmt::RTL_OP_SLE: op_str = "sle"; break;
                                        case Compute_RTL_Stmt::RTL_OP_SEQ: op_str = "seq"; break;
                                        case Compute_RTL_Stmt::RTL_OP_SNE: op_str = "sne"; break;
                                        case Compute_RTL_Stmt::RTL_OP_AND: op_str = "and"; break;
                                        case Compute_RTL_Stmt::RTL_OP_OR:  op_str = "or";  break;
                                        default: op_str = "add"; break;
                                    }
                                    fprintf(asm_file, "    %s $%s, $%s, $%s\n",
                                        op_str,
                                        dest_reg->get_name().c_str(),
                                        op1_reg->get_name().c_str(),
                                        op2_reg->get_name().c_str());
                                }
                            }
                        }
                    }
                    else if (auto loadaddr = dynamic_cast<Loadaddr_RTL_Stmt*>(rtl)) {
                        if (auto dest_reg = dynamic_cast<Register_RTL_Opd*>(loadaddr->get_dest())) {
                            if (auto src_mem = dynamic_cast<Memory_RTL_Opd*>(loadaddr->get_source())) {
                                fprintf(asm_file, "    la $%s, %s\n",
                                    dest_reg->get_name().c_str(),
                                    src_mem->get_name().c_str());
                            }
                        }
                    }
                    else if (auto label = dynamic_cast<Label_RTL_Stmt*>(rtl)) {
                        fprintf(asm_file, "%s:\n", label->get_label().c_str());
                    }
                    else if (auto goto_stmt = dynamic_cast<Goto_RTL_Stmt*>(rtl)) {
                        fprintf(asm_file, "    j %s\n", goto_stmt->get_label().c_str());
                    }
                    else if (auto bgtz = dynamic_cast<Bgtz_RTL_Stmt*>(rtl)) {
                        if (auto opd_reg = dynamic_cast<Register_RTL_Opd*>(bgtz->get_opd())) {
                            fprintf(asm_file, "    bgtz $%s, %s\n",
                                opd_reg->get_name().c_str(),
                                bgtz->get_label().c_str());
                        }
                    }
                    else if (auto call = dynamic_cast<Call_RTL_Stmt*>(rtl)) {
                        fprintf(asm_file, "    jal %s\n", call->get_func_name().c_str());
                    }
                    else if (auto acall = dynamic_cast<CallAssign_RTL_Stmt*>(rtl)) {
                        fprintf(asm_file, "    jal %s\n", acall->get_func_name().c_str());
                    }
                    else if (auto push = dynamic_cast<Push_RTL_Stmt*>(rtl)) {
                        if (auto val_reg = dynamic_cast<Register_RTL_Opd*>(push->get_value())) {
                            bool is_float_push = val_reg->is_float_register();
                            push_is_float_stack.push_back(is_float_push);
                            if (is_float_push) {
                                // For doubles, match reference: store at -4($sp) then
                                // move stack by 8 bytes.
                                fprintf(asm_file, "    s.d $%s, -4($sp)\n", val_reg->get_name().c_str());
                                fprintf(asm_file, "    sub $sp, $sp, 8\n");
                            } else {
                                fprintf(asm_file, "    sw $%s, 0($sp)\n", val_reg->get_name().c_str());
                                fprintf(asm_file, "    sub $sp, $sp, 4\n");
                            }
                        }
                    }
                    else if (auto pop = dynamic_cast<Pop_RTL_Stmt*>(rtl)) {
                        // Pop must mirror the last push's size.
                        if (!push_is_float_stack.empty()) {
                            bool last_float = push_is_float_stack.back();
                            push_is_float_stack.pop_back();
                            if (last_float) {
                                fprintf(asm_file, "    add $sp, $sp, 8\n");
                            } else {
                                fprintf(asm_file, "    add $sp, $sp, 4\n");
                            }
                        } else {
                            // Fallback if we somehow lost tracking.
                            fprintf(asm_file, "    add $sp, $sp, 4\n");
                        }
                    }
                    else if (auto move = dynamic_cast<Move_RTL_Stmt*>(rtl)) {
                        if (auto dest_reg = dynamic_cast<Register_RTL_Opd*>(move->get_dest())) {
                            if (auto src_reg = dynamic_cast<Register_RTL_Opd*>(move->get_src())) {
                                if (move->get_is_float()) {
                                    fprintf(asm_file, "    mov.d $%s, $%s\n",
                                        dest_reg->get_name().c_str(),
                                        src_reg->get_name().c_str());
                                } else {
                                    fprintf(asm_file, "    move $%s, $%s\n",
                                        dest_reg->get_name().c_str(),
                                        src_reg->get_name().c_str());
                                }
                            }
                        }
                    }
                    else if (auto write = dynamic_cast<Write_RTL_Stmt*>(rtl)) {
                        fprintf(asm_file, "    syscall\n");
                    }
                    else if (auto ret = dynamic_cast<Return_RTL_Stmt*>(rtl)) {
                        fprintf(asm_file, "    j epilogue_%s\n", func_label.c_str());
                    }
                }
            }

            // Output SPIM epilogue for this function
            fprintf(asm_file, "epilogue_%s:\n", func_label.c_str());
            fprintf(asm_file, "    add $sp, $sp, %d  # Increment stack pointer for local variables\n", stack_size);
            fprintf(asm_file, "    lw $fp, -4($sp)   # Set $fp to $sp-4\n");
            fprintf(asm_file, "    lw $ra, 0($sp)    # Save ra\n");
            fprintf(asm_file, "    jr $ra            # Jump back to the called procedure\n");
            fprintf(asm_file, "# Epilogue Ends\n\n");
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
    
    // Close ASM file before rewriting it
    if (asm_file) {
        fclose(asm_file);
        asm_file = nullptr;
    }
    
    // Now read the generated ASM file and prepend the .data section
    if (!global_vars.empty() || !all_string_literals.empty()) {
        // Read the existing ASM file
        FILE* temp_asm = fopen(asm_name, "r");
        if (temp_asm) {
            // Create a temporary file
            char temp_name[256];
            snprintf(temp_name, sizeof(temp_name), "%s.tmp", input_file);
            FILE* temp_file = fopen(temp_name, "w");
            if (temp_file) {
                // Write .data section to temp file
                fprintf(temp_file, ".data\n");
                
                // Output global variables
                for (const auto& var : global_vars) {
                    if (var.type == 2) {  // TYPE_FLOAT
                        fprintf(temp_file, "%s_: .double 0.0\n", var.name.c_str());
                    } else {
                        fprintf(temp_file, "%s_: .word 0\n", var.name.c_str());
                    }
                }
                
                // Output string literals
                for (const auto& kv : all_string_literals) {
                    fprintf(temp_file, "%s: .asciiz \"%s\"\n", kv.first.c_str(), kv.second.c_str());
                }
                
                fprintf(temp_file, "\n");
                
                // Copy the rest of the ASM file
                int c;
                while ((c = fgetc(temp_asm)) != EOF) {
                    fputc(c, temp_file);
                }
                
                fclose(temp_asm);
                fclose(temp_file);
                
                // Replace the original ASM file with the temp file
                rename(temp_name, asm_name);
            } else {
                fclose(temp_asm);
            }
        }
    }

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
