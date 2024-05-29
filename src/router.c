#include "router.h"

void route(Request *req, char **res)
{
    printf("\n\n\x1b[38;5;50m------REQUEST-----\x1b[0m\n\n");
    printf("\x1b[38;5;50m@%s\x1b[0m %s\n", req->method, req->path);
    printf("%s\n", req->headers);
    printf("\n\x1b[38;5;50m------------------\x1b[0m\n\n");
    if (strncmp(req->path, "/upsert/", strlen("/upsert/")) == 0 && strcmp(req->method, "POST") == 0)
    {
        dev_upsert_handler(req, res);
    }
    else if (strncmp(req->path, "/query/", strlen("/query/")) == 0 && strcmp(req->method, "POST") == 0)
    {
        dev_query_handler(req, res);
    }
    else if (strcmp(req->path, "/echo") == 0 || strcmp(req->path, "/") == 0)
    {
        echo_handler(req, res);
    }
    else if (strcmp(req->path, "/print") == 0 && strcmp(req->method, "GET") == 0)
    {
        dev_print_handler(req, res);
    }
    else if (strncmp(req->path, "/drop/", strlen("/drop/")) == 0 && strcmp(req->method, "DELETE") == 0)
    {
        drop_handler(req, res);
    }
    else if (strcmp(req->path, "/health") == 0 || strcmp(req->path, "/ping") == 0)
    {
        health_handler(req, res);
    }
    else if (strncmp(req->path, "/embeddings/", strlen("/embeddings/")) == 0 && strcmp(req->method, "POST") == 0)
    {
        upsert_handler(req, res);
    }
    else if (strncmp(req->path, "/query_embeddings/", strlen("/query_embeddings/")) == 0 && strcmp(req->method, "POST") == 0)
    {
        query_handler(req, res);
    }
    else
    {
        char message[] = "path not found 😮";
        char content_type[] = "text/plain; charset=UTF-8";
        write_response(res, HTTP_NOT_FOUND, content_type, message);
    }
}