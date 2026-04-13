double d1(double a, double b) {
    double t = a + b;
    return t;
}

double d2(double a, double b) {
    double t = a * 2.0;
    t = t + b;
    return t;
}

int main() {
    double x = 1.5;
    double y = 2.5;

    double r1 = d1(x, y);
    double r2 = d2(x, y);

    int k = (int)(r1 + r2);
    return k;
}
