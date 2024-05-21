#include <stdio.h>
#include <math.h>

#define EMBEDDING_SIZE 1536

static float dot_product(float a[], float b[], int size) {
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}

static float magnitude(float a[], int size) {
    float sum = 0;
    for (int i = 0; i < size; i++) {
        sum += a[i] * a[i];
    }
    return sqrt(sum);
}

float cosine_distance(float a[], float b[], int size) {
    return 1 - (dot_product(a, b, size) / (magnitude(a, size) * magnitude(b, size)));
}