#include <stdio.h>

int minimum(int *n1, int *n2, int *n3, int *n4) {
    int smallest = *n1;
    if (*n2 < smallest)
        smallest = *n2;
    if (*n3 < smallest)
        smallest = *n3;
    if (*n4 < smallest)
        smallest = *n4;
    return smallest;
}

int maximum(int *n1, int *n2, int *n3, int *n4) {
    int largest = *n1;
    if (*n2 > largest)
        largest = *n2;
    if (*n3 > largest)
        largest = *n3;
    if (*n4 > largest)
        largest = *n4;
    return largest;
}

int prod(int (*min)(int *, int *, int *, int *), int (*max)(int *, int *, int *, int *), int *n1, int *n2, int *n3, int *n4) {
    return min(n1, n2, n3, n4) * max(n1, n2, n3, n4);
}

void main() {
    int num1 = 10;
    int num2 = 20;
    int num3 = 30;
    int num4 = 40;

    int result = prod(minimum, maximum, &num1, &num2, &num3, &num4);
    printf("The product is: %d\n", result);

}
