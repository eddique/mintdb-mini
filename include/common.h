#ifndef mintdb_mini_common_h
#define mintdb_mini_common_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_FIELD_SIZE 256
#define EMBEDDING_SIZE 1536

typedef struct {
    char id[MAX_FIELD_SIZE];
    char title[MAX_FIELD_SIZE];
    char* content;
    char url[MAX_FIELD_SIZE];
    float embeddings[EMBEDDING_SIZE];
} Data;

#endif