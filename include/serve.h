#ifndef mintdb_mini_serve_h
#define mintdb_mini_serve_h
#include "common.h"
#include "fs.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#define MAX_CLIENTS 100


void run(int port);
#endif