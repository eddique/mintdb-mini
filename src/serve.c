#include <serve.h>

typedef struct
{
    char line[BUFFER_SIZE];
    char headers[BUFFER_SIZE];
    char method[32];
    char *body;
} Request;

void parse_request(char *buffer, ssize_t bytes, Request *request)
{
    char *line_end = strstr(buffer, "\r\n");
    char *headers_end = strstr(buffer, "\r\n\r\n");

    if (line_end == NULL || headers_end == NULL)
    {
        printf("Invalid HTTP request\n");
        return;
    }

    *line_end = '\0';
    char *line = buffer;
    *headers_end = '\0';
    char *headers = line_end + 2;
    char *body = headers_end + 4;
    request->body = malloc(strlen(body + 1) * sizeof(char));
    if (request->body == NULL)
    {
        printf("Unable to malloc body");
        exit(EXIT_FAILURE);
    }
    strncpy(request->line, line, BUFFER_SIZE - 1);
    strncpy(request->headers, headers, BUFFER_SIZE - 1);
    strcpy(request->body, body);
}

void *handle_client(void *arg)
{
    int client_socket = *(int *)arg;
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

        memcpy(request_data + total_bytes_received, buffer, bytes_received);
        total_bytes_received += bytes_received;
        if (strstr(request_data, "\r\n\r\n") != NULL)
        {
            break;
        }
    }
    request_data[total_bytes_received] = '\0';
    Request *req = malloc(sizeof(Request));
    parse_request(request_data, total_bytes_received, req);
    char response[BUFFER_SIZE];
    char message[] = "Got it!";
    snprintf(response, BUFFER_SIZE - 1, HTTP_OK, strlen(message), message);
    printf("response: %s\n", response);
    send(client_socket, response, strlen(response), 0);
    free(req->body);
    free(req);
    close(client_socket);
    return NULL;
}

void run(int port)
{
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
    printf("listening on http://localhost:%d\n", port);
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
}