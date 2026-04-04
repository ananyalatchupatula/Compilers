#ifndef RTL_NEW_H
#define RTL_NEW_H

#include <string>
#include <list>
#include <cstdint>
#include <cstdio>

using namespace std;

enum RTL_Opd_Type {
    REGISTER,
    MEMORY,
    LABEL
};

class RTL_Opd {
public:
    RTL_Opd() {}
    virtual ~RTL_Opd() {}
    virtual void print(FILE *file) = 0;
    virtual string to_string() = 0;
};

class Register_RTL_Opd : public RTL_Opd {
private:
    string reg_name;

public:
    Register_RTL_Opd(string name);
    ~Register_RTL_Opd();
    void print(FILE *file);
    string to_string();
    string get_name();
    bool is_float_register();
};

class Memory_RTL_Opd : public RTL_Opd {
    
private:
    string var_name;

public:
    Memory_RTL_Opd(string name);
    ~Memory_RTL_Opd();
    void print(FILE *file);
    string to_string();
    string get_name();
};

class Label_RTL_Opd : public RTL_Opd {
private:
    string label_name;

public:
    Label_RTL_Opd(string name);
    ~Label_RTL_Opd();
    void print(FILE *file);
    string to_string();
    string get_name();
};

class Const_RTL_Opd : public RTL_Opd {
private:
    int int_value;
    double float_value;
    bool is_float;

public:
    Const_RTL_Opd(int val);
    Const_RTL_Opd(double val);
    ~Const_RTL_Opd();
    void print(FILE *file);
    string to_string();
    bool get_is_float() { return is_float; }
};

class RTL_Stmt {
public:
    RTL_Stmt() {}
    virtual ~RTL_Stmt() {}
    virtual void print(FILE *file) = 0;
};

class Compute_RTL_Stmt : public RTL_Stmt {
public:
    enum RTL_Op {
        RTL_OP_ADD_D,
        RTL_OP_SUB_D,
        RTL_OP_MUL_D,
        RTL_OP_DIV_D,
        RTL_OP_SLT_D,
        RTL_OP_SLE_D,
        RTL_OP_ILOAD,
        RTL_OP_FLOAD,
        RTL_OP_ADD,
        RTL_OP_SUB,
        RTL_OP_MUL,
        RTL_OP_DIV,
        RTL_OP_SLT,
        RTL_OP_SGT,
        RTL_OP_SLE,
        RTL_OP_SGE,
        RTL_OP_SEQ,
        RTL_OP_SNE,
        RTL_OP_NOT,
        RTL_OP_NEG,
        RTL_OP_UMINUS,
        RTL_OP_AND,
        RTL_OP_OR,
        RTL_OP_MOVE,
        RTL_OP_MOVT,
        RTL_OP_MOVF
    };

private:
    RTL_Opd *dest, *opd1, *opd2;
    RTL_Op op;

public:
    Compute_RTL_Stmt(RTL_Opd *d, RTL_Opd *op1, RTL_Op operation, RTL_Opd *op2);
    ~Compute_RTL_Stmt();
    void print(FILE *file);
};

class Load_RTL_Stmt : public RTL_Stmt {
private:
    RTL_Opd *dest, *source;
    bool is_float;

public:
    Load_RTL_Stmt(RTL_Opd *d, RTL_Opd *s, bool float_op = false);
    ~Load_RTL_Stmt();
    void print(FILE *file);
};

class Loadaddr_RTL_Stmt : public RTL_Stmt {
private:
    RTL_Opd *dest, *source;
    string string_value;  // For comment tracking

public:
    Loadaddr_RTL_Stmt(RTL_Opd *d, RTL_Opd *s, string str_val = "");
    ~Loadaddr_RTL_Stmt();
    void print(FILE *file);
};

class Store_RTL_Stmt : public RTL_Stmt {
private:
    RTL_Opd *dest, *source;
    bool is_float;

public:
    Store_RTL_Stmt(RTL_Opd *d, RTL_Opd *s, bool float_op = false);
    ~Store_RTL_Stmt();
    void print(FILE *file);
};

class Label_RTL_Stmt : public RTL_Stmt {
private:
    string label_name;

public:
    Label_RTL_Stmt(string name);
    ~Label_RTL_Stmt();
    void print(FILE *file);
};

class Goto_RTL_Stmt : public RTL_Stmt {
private:
    string label_name;

public:
    Goto_RTL_Stmt(string name);
    ~Goto_RTL_Stmt();
    void print(FILE *file);
};

class Bgtz_RTL_Stmt : public RTL_Stmt {
private:
    RTL_Opd *opd;
    string label_name;

public:
    Bgtz_RTL_Stmt(RTL_Opd *o, string label);
    ~Bgtz_RTL_Stmt();
    void print(FILE *file);
};

class Write_RTL_Stmt : public RTL_Stmt {
public:
    Write_RTL_Stmt();
    ~Write_RTL_Stmt();
    void print(FILE *file);
};

class Read_RTL_Stmt : public RTL_Stmt {
public:
    Read_RTL_Stmt();
    ~Read_RTL_Stmt();
    void print(FILE *file);
};

#endif
