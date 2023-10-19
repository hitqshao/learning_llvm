#include <stdio.h>

int main() {
    int x = 5;
    int y = 10;
    int z = 0;

    if (x < y) {
        for (int i = 0; i < 5; i++) {
            z += x;
        }
    } else {
        while (y > 0) {
            z -= y;
            y--;
        }
    }

    printf("Result: %d\n", z);

    return 0;
}

