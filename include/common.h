#ifndef mintdb_mini_common_h
#define mintdb_mini_common_h
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_FIELD_SIZE 256
#define EMBEDDING_SIZE 1536
#define BUFFER_SIZE 8192

typedef struct {
    char id[MAX_FIELD_SIZE];
    char title[MAX_FIELD_SIZE];
    char *content;
    char url[MAX_FIELD_SIZE];
    double *embeddings;
} Data;

typedef struct
{
    char line[BUFFER_SIZE];
    char headers[BUFFER_SIZE];
    int content_length;
    char path[BUFFER_SIZE];
    char method[8];
    char *body;
} Request;

#endif