int g(int x) {
    int t;
    if (x > 0) {
        t = x + 1;
    } else {
        t = x - 1;
    }
    return t;
}

int h(int y) {
    int t = y * 2;
    return t;
}

int main() {
    int a = 5;
    int r1 = g(a);
    int r2 = h(a);
    return r1 + r2;
}
