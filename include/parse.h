#ifndef mintdb_mini_parse_h
#define mintdb_mini_parse_h
#include "common.h"

typedef struct {
    float embeddings[EMBEDDING_SIZE];
} Embedding;

int parse_json_document(char *json, Data *data);
int parse_embedding(char *json, Embedding *emb);

#endif