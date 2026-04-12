#ifndef ASM_NEW_H
#define ASM_NEW_H

#include <string>
#include <list>
#include <cstdio>

using std::string;
using std::list;

// Forward declarations
class ASM_Opd;

enum ASM_Opd_Type {
    ASM_REGISTER,
    ASM_MEMORY,
    ASM_INT_CONST,
    ASM_LABEL,
    ASM_STR_CONST
};

// Base class for ASM operands
class ASM_Opd {
public:
    ASM_Opd() {}
    virtual ~ASM_Opd() {}
    virtual void print(FILE *file) = 0;
    virtual string to_string() = 0;
    virtual string get_name() = 0;
};

// ASM Register Operand
class ASM_Register_Opd : public ASM_Opd {
private:
    string reg_name;
    
public:
    ASM_Register_Opd(string name) : reg_name(name) {}
    ~ASM_Register_Opd() {}
    void print(FILE *file);
    string to_string();
    string get_name();
    string get_register() { return reg_name; }
};

// ASM Memory Operand
class ASM_Mem_Opd : public ASM_Opd {
private:
    string var_name;
    int offset;
    
public:
    ASM_Mem_Opd(string name, int off = 0) : var_name(name), offset(off) {}
    ~ASM_Mem_Opd() {}
    void print(FILE *file);
    string to_string();
    string get_name();
    int get_offset() { return offset; }
};

// ASM Integer Constant Operand
class ASM_Int_Const_Opd : public ASM_Opd {
private:
    int value;
    
public:
    ASM_Int_Const_Opd(int val) : value(val) {}
    ~ASM_Int_Const_Opd() {}
    void print(FILE *file);
    string to_string();
    string get_name();
    int get_value() { return value; }
};

// ASM Label Operand
class ASM_Label_Opd : public ASM_Opd {
private:
    string label_name;
    
public:
    ASM_Label_Opd(string name) : label_name(name) {}
    ~ASM_Label_Opd() {}
    void print(FILE *file);
    string to_string();
    string get_name();
};

// ASM String Constant Operand
class ASM_Str_Const_Opd : public ASM_Opd {
private:
    string str_value;
    
public:
    ASM_Str_Const_Opd(string val) : str_value(val) {}
    ~ASM_Str_Const_Opd() {}
    void print(FILE *file);
    string to_string();
    string get_name();
    string get_string() { return str_value; }
};

// Base class for ASM statements
class ASM_Stmt {
public:
    ASM_Stmt() {}
    virtual ~ASM_Stmt() {}
    virtual void print(FILE *file) = 0;
};

// ASM Compute Statement (arithmetic operations)
class Compute_ASM_Stmt : public ASM_Stmt {
public:
    enum ASM_Op {
        ASM_OP_ADD,
        ASM_OP_SUB,
        ASM_OP_MUL,
        ASM_OP_DIV,
        ASM_OP_LI,      // Load immediate
        ASM_OP_MOVE
    };

private:
    ASM_Opd *dest;
    ASM_Opd *opd1;
    ASM_Op op;
    ASM_Opd *opd2;
    
public:
    Compute_ASM_Stmt(ASM_Opd *d, ASM_Opd *o1, ASM_Op o, ASM_Opd *o2)
        : dest(d), opd1(o1), op(o), opd2(o2) {}
    ~Compute_ASM_Stmt();
    void print(FILE *file);
    ASM_Op get_op() { return op; }
    ASM_Opd* get_dest() { return dest; }
};

// ASM Load Statement
class Load_ASM_Stmt : public ASM_Stmt {
private:
    ASM_Opd *dest;
    ASM_Opd *source;
    
public:
    Load_ASM_Stmt(ASM_Opd *d, ASM_Opd *s) : dest(d), source(s) {}
    ~Load_ASM_Stmt();
    void print(FILE *file);
    ASM_Opd* get_dest() { return dest; }
    ASM_Opd* get_source() { return source; }
};

// ASM Store Statement
class Store_ASM_Stmt : public ASM_Stmt {
private:
    ASM_Opd *source;
    ASM_Opd *dest;
    
public:
    Store_ASM_Stmt(ASM_Opd *s, ASM_Opd *d) : source(s), dest(d) {}
    ~Store_ASM_Stmt();
    void print(FILE *file);
};

// ASM Label Statement
class Label_ASM_Stmt : public ASM_Stmt {
private:
    string label_name;
    
public:
    Label_ASM_Stmt(string name) : label_name(name) {}
    ~Label_ASM_Stmt();
    void print(FILE *file);
    string get_label() { return label_name; }
};

// ASM Goto Statement
class Goto_ASM_Stmt : public ASM_Stmt {
private:
    string label_name;
    
public:
    Goto_ASM_Stmt(string name) : label_name(name) {}
    ~Goto_ASM_Stmt();
    void print(FILE *file);
    string get_label() { return label_name; }
};

// ASM If-Goto Statement
class If_Goto_ASM_Stmt : public ASM_Stmt {
private:
    ASM_Opd *test_register;
    string label_name;
    
public:
    If_Goto_ASM_Stmt(ASM_Opd *test, string name) 
        : test_register(test), label_name(name) {}
    ~If_Goto_ASM_Stmt();
    void print(FILE *file);
    string get_label() { return label_name; }
};

// ASM Jump Register Statement
class Jump_Reg_ASM_Stmt : public ASM_Stmt {
private:
    ASM_Opd *reg;
    
public:
    Jump_Reg_ASM_Stmt(ASM_Opd *r) : reg(r) {}
    ~Jump_Reg_ASM_Stmt();
    void print(FILE *file);
};

// ASM Procedure Call Statement
class Call_ASM_Stmt : public ASM_Stmt {
private:
    string proc_name;
    
public:
    Call_ASM_Stmt(string name) : proc_name(name) {}
    ~Call_ASM_Stmt();
    void print(FILE *file);
    string get_procedure() { return proc_name; }
};

// ASM Syscall Statement
class Syscall_ASM_Stmt : public ASM_Stmt {
private:
    int syscall_code;
    
public:
    Syscall_ASM_Stmt(int code) : syscall_code(code) {}
    ~Syscall_ASM_Stmt();
    void print(FILE *file);
    int get_code() { return syscall_code; }
};

// ASM Move Statement
class Move_ASM_Stmt : public ASM_Stmt {
private:
    ASM_Opd *dest;
    ASM_Opd *source;
    
public:
    Move_ASM_Stmt(ASM_Opd *d, ASM_Opd *s) : dest(d), source(s) {}
    ~Move_ASM_Stmt();
    void print(FILE *file);
};

// ASM NOP Statement
class NOP_ASM_Stmt : public ASM_Stmt {
public:
    NOP_ASM_Stmt() {}
    ~NOP_ASM_Stmt();
    void print(FILE *file);
};

#endif
