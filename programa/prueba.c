#include <stdio.h>
#include <math.h>

int main() {
    int a = 1000;
    int b = 1200;

    float r = a - b;
    printf("%.2f\n", r);

    float m = r / b;
    printf("%.2f\n", m);

    int v = fabs(m * 100);
    printf("%d\n", v);

    return 0;
}