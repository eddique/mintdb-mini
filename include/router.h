#ifndef mintdb_mini_router_h
#define mintdb_mini_router_h

#define BUFFER_SIZE 8192
#include "fs.h"

typedef struct
{
    char line[BUFFER_SIZE];
    char headers[BUFFER_SIZE];
    int content_length;
    char path[BUFFER_SIZE];
    char method[8];
    char *body;
} Request;


extern const char *HTTP_OK;
extern const char *HTTP_NO_CONTENT;
extern const char *HTTP_NOT_FOUND;
extern const char *HTTP_INTERNAL_ERROR;

void route(Request *req, char *res);
void parse_json(char *json, Data *data);

#endif