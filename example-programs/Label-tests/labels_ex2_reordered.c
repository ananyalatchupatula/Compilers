int f3(int x) {
    int t = x * 3;
    t = t + 5;
    return t;
}

int f4(int x) {
    int t = x - 1;
    t = t + x;
    return t;
}

int f2(int x) {
    int t = x + 10;
    t = t + 2;
    return t;
}

int main() {
    int a = 4;
    int r2 = f2(a);
    int r3 = f3(a);
    int r4 = f4(a);
    return r2 + r3 + r4;
}
