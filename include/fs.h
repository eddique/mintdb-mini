#ifndef mintdb_mini_fs_h
#define mintdb_mini_fs_h
#define MAX_FIELD_SIZE 256
#define MAX_FILE_SIZE 10000
#define EMBEDDING_SIZE 1536

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

typedef struct {
    char id[MAX_FIELD_SIZE];
    char title[MAX_FIELD_SIZE];
    char* content;
    char url[MAX_FIELD_SIZE];
    float embeddings[EMBEDDING_SIZE];
} Data;

void read_document_from_file(const char *path, Data *data);
void parse_and_write_to_file(char *path, char *http_body);

void make_dir(const char *path);
void write_to_file(const char *filepath, const char *data, size_t data_size);
void list_files(const char *path);
char* read_file(const char* path);

#endif
