#ifndef mintdb_mini_serve_h
#define mintdb_mini_serve_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
void run(int port);

const char *HTTP_OK = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: %lu\r\n"
    "\r\n"
    "%s";
const char *HTTP_INTERNAL_ERROR = 
    "HTTP/1.1 500 INTERNAL ERROR\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: %lu\r\n"
    "\r\n"
    "%s";
#endif