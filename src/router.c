#include "router.h"

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
    "Content-Length: %lu\r\n"
    "\r\n";
const char *HTTP_INTERNAL_ERROR =
    "HTTP/1.1 500 INTERNAL ERROR\r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %lu\r\n"
    "\r\n"
    "%s";

void route(Request *req, char *res)
{
    printf("\x1b[38;5;50m@%s\x1b[0m %s\n", req->method, req->path);
    printf("%s\n", req->headers);
    if (strncmp(req->path, "/upsert/", strlen("/upsert/")) == 0 && strcmp(req->method, "POST") == 0)
    {
        char path[strlen(req->path) + 1];
        strncpy(path, req->path, strlen(req->path) + 1);
        char *tb = strtok(path, "/");
        tb = strtok(NULL, "/");
        if (tb == NULL)
            tb = "default";
        printf("table: %s\n", tb);
        Data *data = malloc(sizeof(Data));
        // TODO: Handler error
        parse_json(req->body, data);
        printf("upserting idx: %s -> document: %s\n", tb, data->title);
        write_doc(tb, data);
        free(data->content);
        free(data);
        char message[] = "Got it!";
        snprintf(res, BUFFER_SIZE - 1, HTTP_OK, "text/plain; charset=UTF-8", strlen(message), message);
    }
    else if (strncmp(req->path, "/query/", strlen("/query/")) == 0 && strcmp(req->method, "POST") == 0)
    {
        char path[strlen(req->path) + 1];
        strncpy(path, req->path, strlen(req->path) + 1);
        char *tb = strtok(path, "/");
        tb = strtok(NULL, "/");
        if (tb == NULL)
            tb = "default";
        printf("table: %s\n", tb);
        // Data *data = malloc(sizeof(Data));
        char message[] = "[{\"id\":\"sample_doc\",\"title\":\"sample_doc_title\",\"content\":\"Some sample content\",\"url\":\"https://samle.url.com/1234\",\"distance\":0.1102575}]";
        snprintf(res, BUFFER_SIZE - 1, HTTP_OK, "application/json; charset=UTF-8", strlen(message), message);
    }
    else if (strcmp(req->path, "/echo") == 0 || strcmp(req->path, "/") == 0)
    {
        snprintf(res, BUFFER_SIZE - 1, HTTP_OK, "text/plain; charset=UTF-8", strlen(req->body), req->body);
    }
    else if (strcmp(req->path, "/print") == 0 && strcmp(req->method, "GET") == 0)
    {
        set_cache("test_data");
        print_titles();
        reset_cache();
        snprintf(res, BUFFER_SIZE - 1, HTTP_NO_CONTENT, 0);
    }
    else if (strncmp(req->path, "/drop/", strlen("/drop/")) == 0 && strcmp(req->method, "DELETE") == 0)
    {
        char path[strlen(req->path) + 1];
        strncpy(path, req->path, strlen(req->path) + 1);
        char *tb = strtok(path, "/");
        tb = strtok(NULL, "/");
        if (tb == NULL)
            tb = "default";
        printf("table: %s\n", tb);
        if (rmrf(tb) != 0){
            char message[] = "error deleting 😮";
            snprintf(res, BUFFER_SIZE - 1, HTTP_NOT_FOUND, "text/plain; charset=UTF-8", strlen(message), message);
            return;
        }
        snprintf(res, BUFFER_SIZE - 1, HTTP_NO_CONTENT, 0);
    }
    else if (strcmp(req->path, "/health") == 0 || strcmp(req->path, "/ping") == 0)
    {
        char message[] = "{\"ok\":true,\"message\":\"all good 😎\"}";
        snprintf(res, BUFFER_SIZE - 1, HTTP_OK, "application/json; charset=UTF-8", strlen(message), message);
    }
    else
    {
        char message[] = "path not found 😮";
        snprintf(res, BUFFER_SIZE - 1, HTTP_NOT_FOUND, "text/plain; charset=UTF-8", strlen(message), message);
    }
}

void parse_json(char *json, Data *data)
{
    char *key = strstr(json, "\"id\":");
    if (key != NULL)
    {
        key += 5;
        while (*key == ' ')
            key++;
        if (*key == '\"')
            key++;
        char *id_end = strstr(key, "\"");
        if (id_end != NULL)
        {
            *id_end = '\0';
        }
        strncpy(data->id, key, strlen(key));
        data->id[strlen(key)] = '\0';
        *id_end = '\"';
    }
    char *title_key = strstr(json, "\"title\":");
    if (title_key != NULL)
    {
        title_key += 8;
        while (*title_key == ' ')
            title_key++;
        if (*title_key == '\"')
            title_key++;
        char *title_end = strstr(title_key, "\"");
        if (title_end != NULL)
        {
            *title_end = '\0';
        }
        strncpy(data->title, title_key, strlen(title_key));
        data->title[strlen(title_key)] = '\0';
        *title_end = '\"';
    }
    char *url_key = strstr(json, "\"url\":");
    if (url_key != NULL)
    {
        url_key += 6;
        while (*url_key == ' ')
            url_key++;
        if (*url_key == '\"')
            url_key++;
        char *url_end = strstr(url_key, "\"");
        if (url_end != NULL)
        {
            *url_end = '\0';
        }
        strncpy(data->url, url_key, strlen(url_key));
        data->url[strlen(url_key)] = '\0';
        *url_end = '\"';
    }
    char *content_key = strstr(json, "\"content\":");
    if (content_key != NULL)
    {
        content_key += 10;
        while (*content_key == ' ')
            content_key++;
        if (*content_key == '\"')
            content_key++;
        char *content_end = strstr(content_key, "\",");
        if (content_end != NULL)
        {
            *content_end = '\0';
        }
        data->content = malloc(strlen(content_key) + 1);
        memcpy(data->content, content_key, strlen(content_key));
        data->content[strlen(content_key)] = '\0';
        *content_end = '\"';
        *(content_end + 1) = ',';
    }
}