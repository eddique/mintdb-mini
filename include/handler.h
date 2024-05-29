#ifndef mintdb_mini_handler_h
#define mintdb_mini_handler_h
#include "common.h"
#include "fs.h"
#include "parse.h"

extern const char *HTTP_OK;
extern const char *HTTP_NO_CONTENT;
extern const char *HTTP_NOT_FOUND;
extern const char *HTTP_INTERNAL_ERROR;

void dev_upsert_handler(Request *req, char **res);
void dev_query_handler(Request *req, char **res);
void dev_print_handler(Request *req, char **res);
void health_handler(Request *req, char **res);
void echo_handler(Request *req, char **res);
void drop_handler(Request *req, char **res);
void upsert_handler(Request *req, char **res);
void query_handler(Request *req, char **res);
void write_response(char **res, const char * http_header, char *content_type, char *body);

#endif