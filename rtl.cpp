#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

// ---------------- REGISTER GENERATOR ----------------
int reg_count = 0;

string new_reg() {
    return "v" + to_string(reg_count++);
}

// ---------------- RTL CONVERSION ----------------
void convert_TAC_to_RTL(const vector<string> &tac_lines) {

    cout << "PROCEDURE: main\n";
    cout << "BEGIN: RTL Statements\n\n";

    for (string line : tac_lines) {

        // ---------------- LABEL ----------------
        if (line.find(":") != string::npos && line.find("Label") != string::npos) {
            cout << line << endl;
            continue;
        }

        if (line.find("if(") != string::npos) {
            string cond = line.substr(3, line.find(")") - 3);
            string label = line.substr(line.find("goto") + 5);

            string r = new_reg();
            cout << "load: " << r << " <- " << cond << endl;
            cout << "bgtz: " << r << " -> " << label << endl;
            continue;
        }

        // ---------------- ASSIGNMENT / OPERATION ----------------
        // Example: temp1 = a + b
        size_t eq_pos = line.find("=");
        if (eq_pos != string::npos) {

            string lhs = line.substr(0, eq_pos - 1);
            string rhs = line.substr(eq_pos + 2);

            // Check if binary op
            if (rhs.find("+") != string::npos ||
                rhs.find("-") != string::npos ||
                rhs.find("*") != string::npos ||
                rhs.find("/") != string::npos) {

                string op;
                if (rhs.find("+") != string::npos) op = "add";
                else if (rhs.find("-") != string::npos) op = "sub";
                else if (rhs.find("*") != string::npos) op = "mul";
                else if (rhs.find("/") != string::npos) op = "div";

                stringstream ss(rhs);
                string a, sym, b;
                ss >> a >> sym >> b;

                string r1 = new_reg();
                string r2 = new_reg();
                string r3 = new_reg();

                cout << "load: " << r1 << " <- " << a << endl;
                cout << "load: " << r2 << " <- " << b << endl;
                cout << op << ": " << r3 << " <- " << r1 << ", " << r2 << endl;
                cout << "store: " << lhs << " <- " << r3 << endl;
            }

            // Unary NOT / minus
            else if (rhs.find("!") != string::npos || rhs.find("-") != string::npos) {

                string r1 = new_reg();
                string r2 = new_reg();

                cout << "load: " << r1 << " <- " << rhs.substr(2) << endl;
                cout << "not: " << r2 << " <- " << r1 << endl;
                cout << "store: " << lhs << " <- " << r2 << endl;
            }

            // Simple assignment
            else {
                string r = new_reg();
                cout << "load: " << r << " <- " << rhs << endl;
                cout << "store: " << lhs << " <- " << r << endl;
            }
        }
    }

    cout << "\nEND: RTL Statements\n";
}