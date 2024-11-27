#include <stdio.h>

void sort(int *a, int n);

int main() {
    int a[256], n;
    for (;;) {
        int x;
        scanf("%d", &x);
        if (x == -1) break;
        a[n++] = x;
    }
    for (int i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }
    putchar('\n');
    sort(a, n);
    for (int i = 0; i < n; ++i) {
        printf("%d ", a[i]);
    }
    putchar('\n');
    return 0;
}
