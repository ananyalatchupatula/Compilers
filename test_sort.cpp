#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

int main() {
    std::vector<std::string> vars1 = {"a", "b"};
    std::sort(vars1.begin(), vars1.end());
    std::cout << "Sorted [a, b]: ";
    for (auto v : vars1) std::cout << v << " ";
    std::cout << "\n";
    
    std::vector<std::string> vars2 = {"D1234", "a"};
    std::sort(vars2.begin(), vars2.end());
    std::cout << "Sorted [D1234, a]: ";
    for (auto v : vars2) std::cout << v << " ";
    std::cout << "\n";
    
    std::vector<std::string> vars3 = {"a_", "D1234_", "b_", "k_"};
    std::sort(vars3.begin(), vars3.end());
    std::cout << "Sorted [a_, D1234_, b_, k_]: ";
    for (auto v : vars3) std::cout << v << " ";
    std::cout << "\n";
    
    return 0;
}
