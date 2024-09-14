#include "handler.h"

const char *HTTP_OK =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %lu\r\n"
    "\r\n"
    "%s";
const char *HTTP_NOT_FOUND =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %lu\r\n"
    "\r\n"
    "%s";
const char *HTTP_NO_CONTENT =
    "HTTP/1.1 204 No Content\r\n"
    "Content-Length: 0\r\n"
    "\r\n";
const char *HTTP_INTERNAL_ERROR =
    "HTTP/1.1 500 INTERNAL ERROR\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %lu\r\n"
    "\r\n"
    "%s";

ssize_t r_size(const char *http, char *content_type, char *data)
{
    ssize_t size = strlen(http);
    if (content_type != NULL)
    {
        size += strlen(content_type);
    }
    if (data != NULL)
    {
        size += strlen(data);
    }
    size += 50;
    return size;
}

void dev_upsert_handler(Request *req, char **res)
{
    char path[strlen(req->path) + 1];
    strncpy(path, req->path, strlen(req->path) + 1);
    char *tb = strtok(path, "/");
    tb = strtok(NULL, "/");
    if (tb == NULL)
        tb = "default";
    Data *data = (Data *)malloc(sizeof(Data));
    data->embeddings = (double *)malloc(EMBEDDING_SIZE * sizeof(double));
    // TODO: Handler error
    if (parse_json_document(req->body, data))
    {
        char content_type[] = "text/plain; charset=UTF-8";
        char message[] = "error upserting 😮";
        write_response(res, HTTP_INTERNAL_ERROR, content_type, message);
        free(data->content);
        free(data->embeddings);
        free(data);
        return;
    }
    printf("upserting idx: %s -> document: %s\n", tb, data->title);
    write_doc(tb, data);
    free(data->content);
    free(data->embeddings);
    free(data);
    char message[] = "Got it!";
    char content_type[] = "text/plain; charset=UTF-8";
    write_response(res, HTTP_OK, content_type, message);
}
void dev_query_handler(Request *req, char **res)
{
    char path[strlen(req->path) + 1];
    strncpy(path, req->path, strlen(req->path) + 1);
    char *tb = strtok(path, "/");
    tb = strtok(NULL, "/");
    if (tb == NULL)
        tb = "default";
    char message[] = "[{\"id\":\"sample_doc\",\"title\":\"sample_doc_title\",\"content\":\"Some sample content\",\"url\":\"https://samle.url.com/1234\",\"distance\":0.1102575}]";
    char content_type[] = "application/json; charset=UTF-8";
    write_response(res, HTTP_OK, content_type, message);
}
void dev_print_handler(Request *req, char **res)
{
    // TODO: setting cache to invalid directory causes double free
    // in reset_cache free(node->data);
    // Checking if tb is > 0, not great though
    set_cache("test_data");
    print_titles();
    reset_cache();
    write_response(res, HTTP_NO_CONTENT, NULL, NULL);
}
void health_handler(Request *req, char **res)
{
    char message[] = "{\"ok\":true,\"message\":\"all good 😎\"}";
    char content_type[] = "application/json; charset=UTF-8";
    write_response(res, HTTP_OK, content_type, message);
}
void echo_handler(Request *req, char **res)
{
    char content_type[] = "text/plain; charset=UTF-8";
    write_response(res, HTTP_OK, content_type, req->body);
}
void drop_handler(Request *req, char **res)
{
    char path[strlen(req->path) + 1];
    strncpy(path, req->path, strlen(req->path) + 1);
    char *tb = strtok(path, "/");
    tb = strtok(NULL, "/");
    if (tb == NULL)
        tb = "default";
    if (rmrf(tb) != 0)
    {
        char message[] = "error deleting 😮";
        char content_type[] = "text/plain; charset=UTF-8";
        write_response(res, HTTP_NOT_FOUND, content_type, message);
        return;
    }
    write_response(res, HTTP_NO_CONTENT, NULL, NULL);
}
void upsert_handler(Request *req, char **res)
{
    char path[strlen(req->path) + 1];
    strncpy(path, req->path, strlen(req->path) + 1);
    char *tb = strtok(path, "/");
    tb = strtok(NULL, "/");
    if (tb == NULL)
        tb = "default";
    Data *data = (Data *)malloc(sizeof(Data));
    data->embeddings = (double *)malloc(EMBEDDING_SIZE * sizeof(double));
    // TODO: Handle error
    if (parse_json_document(req->body, data))
        printf("invalid json!\n");
    write_document(tb, data);
    free(data->content);
    free(data->embeddings);
    free(data);
    char message[] = "Got it!";
    char content_type[] = "text/plain; charset=UTF-8";
    write_response(res, HTTP_OK, content_type, message);
}
void query_handler(Request *req, char **res)
{
    char path[strlen(req->path) + 1];
    strncpy(path, req->path, strlen(req->path) + 1);
    char *tb = strtok(path, "/");
    tb = strtok(NULL, "/");
    if (tb == NULL)
        tb = "default";
    char *results = (char *)malloc(BUFFER_SIZE * 2 * sizeof(char));
    Embedding *emb = (Embedding *)malloc(sizeof(Embedding));
    emb->embeddings = (double *)malloc(EMBEDDING_SIZE * sizeof(double));
    parse_embedding(req->body, emb);
    query_embeddings(tb, emb->embeddings, results);
    char content_type[] = "application/json; charset=UTF-8";
    write_response(res, HTTP_OK, content_type, results);
    free(results);
    free(emb->embeddings);
    free(emb);
}

void write_response(char **res, const char *http_header, char *content_type, char *body)
{
    if (strcmp(http_header, HTTP_NO_CONTENT) == 0)
    {
        ssize_t response_size = strlen(http_header) + 1;
        *res = (char *)malloc(response_size * sizeof(char));
        snprintf(*res, response_size, "%s", http_header);
        return;
    }
    ssize_t response_size = r_size(http_header, content_type, body);
    *res = (char *)malloc(response_size * sizeof(char));
    snprintf(*res, response_size - 1, http_header, content_type, strlen(body), body);
}