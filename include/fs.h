#ifndef mintdb_mini_fs_h
#define mintdb_mini_fs_h
#define MAX_FILE_SIZE 10000

#include "common.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <ftw.h>
#include <pthread.h>

typedef struct Document Document;

struct Document {
    Data *data;
    Document *next;
};

typedef struct {
    pthread_mutex_t mutex;
    char tb[MAX_FIELD_SIZE];
    Document *documents;
} Cache;

void init_fs(const char *path);
void init_cache();
int set_cache(const char *tb);
void reset_cache();
void free_cache();
void write_doc(const char *tb, Data *document);
void read_document_from_file(const char *path, Data *data);
void make_dir(const char *path);
void print_titles();
int rmrf(char *tb);

#endif
