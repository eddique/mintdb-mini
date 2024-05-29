#include "serve.h"
#include "router.h"

int read_request(char *buffer, ssize_t bytes, Request *request)
{
    char *line_end = strstr(buffer, "\r\n");
    char *headers_end = strstr(buffer, "\r\n\r\n");
    if (line_end == NULL || headers_end == NULL)
    {
        printf("Invalid HTTP request\n");
        return 1;
    }

    *line_end = '\0';
    char *line = buffer;
    request->content_length = 0;
    sscanf(line, "%s %s", request->method, request->path);
    *headers_end = '\0';
    char *headers = line_end + 2;
    char *content_length_start = strstr(headers, "Content-Length: ");
    if (content_length_start != NULL)
    {
        sscanf(content_length_start, "Content-Length: %d", &request->content_length);
    }
    else
    {
        content_length_start = strstr(headers, "content-length: ");
        if (content_length_start != NULL)
        {
            sscanf(content_length_start, "content-length: %d", &request->content_length);
        }
    }
    strncpy(request->line, line, BUFFER_SIZE - 1);
    strncpy(request->headers, headers, BUFFER_SIZE - 1);

    char *body_start = headers_end + 4;
    ssize_t received = bytes - (body_start - buffer);
    request->body = (char *)malloc(request->content_length + 1);
    if (request->body == NULL)
    {
        perror("unable to malloc request body");
        exit(EXIT_FAILURE);
    }
    memcpy(request->body, body_start, received);
    request->body[received] = '\0';
    return 0;
}

int parse_request(int client_socket, Request *req)
{
    char buffer[BUFFER_SIZE];
    char *request_data = NULL;
    ssize_t total_bytes_received = 0;

    while (1)
    {
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
        {
            break;
        }

        request_data = realloc(request_data, total_bytes_received + bytes_received + 1);
        if (request_data == NULL)
        {
            break;
        }
        buffer[bytes_received] = '\0';
        memcpy(request_data + total_bytes_received, buffer, bytes_received);
        total_bytes_received += bytes_received;
        if (strstr(request_data, "\r\n\r\n") != NULL)
        {
            break;
        }
    }
    request_data[total_bytes_received] = '\0';
    if (read_request(request_data, total_bytes_received, req) != 0 )
    {
        printf("error reading request\n");
        free(request_data);
        return 1;
    }
    ssize_t total_bytes_received_body = strlen(req->body);
    while (total_bytes_received_body < req->content_length)
    {
        printf("total_bytes: %zd < %d ?\n", total_bytes_received_body, req->content_length);
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        printf("bytes_received: %zd\n", bytes_received);
        if (bytes_received <= 0)
        {
            printf("exiting loop\n");
            break;
        }
        memcpy(req->body + total_bytes_received_body, buffer, bytes_received);
        total_bytes_received_body += bytes_received;
    }
    req->body[total_bytes_received_body] = '\0';
    free(request_data);
    return 0;
}

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
    char *response = NULL;
    Request *req = (Request *)malloc(sizeof(Request));
    if (req == NULL)
    {
        perror("unable to malloc request");
        exit(EXIT_FAILURE);
    }
    if (parse_request(client_socket, req) != 0)
    {
        free(req->body);
        free(req);
        char message[] = "Invalid HTTP Request";
        char *content_type = "text/plain; charset=UTF-8";
        write_response(&response, HTTP_INTERNAL_ERROR, content_type, message);
        send(client_socket, response, strlen(response), 0);
        
        close(client_socket);
        free(response);
        response = NULL;
        return NULL;
    }
    route(req, &response);
    printf("\n\n-----RESPONSE-----\n\n\x1b[38;5;50m%s\x1b[0m\n\n------------------\n\n", response);
    size_t total_sent = 0;
    size_t response_length = strlen(response);
    while (total_sent < response_length)
    {
        ssize_t sent = send(client_socket, response + total_sent, response_length - total_sent, 0);
        if (sent == -1)
        {
            break;
        }
        total_sent += sent;
    }
    free(req->body);
    free(req);
    free(response);
    response = NULL;
    close(client_socket);
    return NULL;
}

void run(int port)
{
    init_cache();
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = htonl(INADDR_ANY)};

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, MAX_CLIENTS) == -1)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("\x1b[38;5;50mlistening on http://localhost:%d\x1b[0m\n", port);
    while (1)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, &client_socket) != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread);
    }

    close(server_socket);
    free_cache();
}