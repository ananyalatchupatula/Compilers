#include <iostream>
using namespace std;

int main() {
    int n;
    cin >> n;
    if(n >= 5 && n <= 10) {
        cout << n << " ";
        n = n - 1;
    }
    return 0;
}
