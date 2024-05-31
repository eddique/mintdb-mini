#include <stdio.h>
#include <math.h>

#define EMBEDDING_SIZE 1536

static double dot_product(double a[], double b[], int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

static double magnitude(double a[], int size) {
    double sum = 0;
    for (int i = 0; i < size; i++) {
        sum += a[i] * a[i];
    }
    return sqrt(sum);
}

double cosine_distance(double a[], double b[], int size) {
    double dot = dot_product(a, b, size);
    double mag_a = magnitude(a, size);
    double mag_b = magnitude(b, size);
    return 1 - (dot / (mag_a * mag_b));
}