
#include "fs.h"

char base_data_path[256];

Cache *cache = NULL;

void print_titles()
{
    if (cache != NULL)
    {
        pthread_mutex_lock(&cache->mutex);
        Document *doc = cache->documents;
        while (doc != NULL)
        {
            Document *next = doc->next;
            printf("Cache Loaded - document: %s\n", doc->data->title);
            doc = next;
        }
        pthread_mutex_unlock(&cache->mutex);
    }
}

void init_fs(const char *base_path)
{
    printf("creating directory %s\n", base_path);
    strcpy(base_data_path, base_path);
    snprintf(base_data_path, sizeof(base_data_path), "%s/%s", base_path, "ds/");
    make_dir(base_data_path);
}

void reset_cache()
{
    if (cache != NULL)
    {
        pthread_mutex_lock(&cache->mutex);
        memset(cache->tb, 0, MAX_FIELD_SIZE);
        free(cache->documents);
        cache->documents = NULL;
        pthread_mutex_unlock(&cache->mutex);

    }
}

void free_cache()
{
    if (cache != NULL)
    {
        pthread_mutex_lock(&cache->mutex);
        Document *doc = cache->documents;
        while (doc != NULL)
        {
            Document *next = doc->next;
            free(doc->data);
            free(doc);
            doc = next;
        }
        pthread_mutex_unlock(&cache->mutex);
        pthread_mutex_destroy(&cache->mutex);
        free(cache);
        cache = NULL;
    }
} 
void write_doc(const char *tb, Data *document)
{
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s/%s%s", base_data_path, tb, "/");
    make_dir(dir_path);
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s.%s", dir_path, document->id, "bin");
    FILE *file = fopen(file_path, "w");
    if (file != NULL)
    {
        char *embedding = "[0.3,0.1,0.2,0.3]";
        size_t data_len = strlen(document->id) + strlen(document->title) + strlen(document->content) +
                          strlen(document->url) + strlen(embedding) + 4;
        char *data = malloc((data_len + 1) * sizeof(char));
        if (data != NULL)
        {
            snprintf(data, data_len + 1, "%s|%s|%s|%s|%s",
                     document->id,
                     document->title,
                     document->content,
                     document->url,
                     embedding);

            fwrite(data, sizeof(char), data_len + 1, file);
            free(data);
        }
        fclose(file);
    }
}

void make_dir(const char *path)
{
    char tmp[256];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strnlen(path, sizeof(tmp));

    if (len >= sizeof(tmp))
    {
        fprintf(stderr, "Path is too long\n");
        exit(EXIT_FAILURE);
    }

    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = '\0';
            if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
            {
                perror("mkdir");
                exit(EXIT_FAILURE);
            }
            *p = '/';
        }
    }

    if (mkdir(tmp, S_IRWXU) != 0 && errno != EEXIST)
    {
        perror("mkdir");
        exit(EXIT_FAILURE);
    }
}
void read_document_from_file(const char *path, Data *data)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        printf("Could not open file for reading.\n");
        return;
    }

    char file_contents[MAX_FILE_SIZE];
    fread(file_contents, sizeof(char), MAX_FILE_SIZE, file);

    char *token = strtok(file_contents, "|");
    strncpy(data->id, token, MAX_FIELD_SIZE);

    token = strtok(NULL, "|");
    strncpy(data->title, token, MAX_FIELD_SIZE);

    token = strtok(NULL, "|");
    data->content = malloc(strlen(token) + 1);
    strcpy(data->content, token);

    token = strtok(NULL, "|");
    strncpy(data->url, token, MAX_FIELD_SIZE);

    token = strtok(NULL, "|");
    token++;
    for (int i = 0; i < EMBEDDING_SIZE; i++)
    {
        token = strtok(token, ",]");
        if (token == NULL)
        {
            break;
        }
        data->embeddings[i] = atof(token);
        token = NULL;
    }

    fclose(file);
}
void init_cache()
{
    cache = malloc(sizeof(Cache));
    if (cache == NULL)
    {
        perror("malloc cache:");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&cache->mutex, NULL);
    memset(cache->tb, 0, MAX_FIELD_SIZE);
    cache->documents = NULL;
}
int set_cache(const char *tb)
{
    DIR *dir;
    struct dirent *entry;
    char path[512];
    snprintf(path, sizeof(path), "%s/%s", base_data_path, tb);
    dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        return 1;
    }
    pthread_mutex_lock(&cache->mutex);
    strcpy(cache->tb, tb);
    Document *lastDoc = NULL;

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        struct stat statbuf;
        if (stat(full_path, &statbuf) == -1)
        {
            perror("stat");
            continue;
        }
        if (S_ISREG(statbuf.st_mode) && strstr(entry->d_name, ".bin") != NULL)
        {
            Data *data = malloc(sizeof(Data));
            Document *doc = malloc(sizeof(Document));
            read_document_from_file(full_path, data);
            doc->data = data;
            if (lastDoc == NULL)
            {
                cache->documents = doc;
            }
            else
            {
                lastDoc->next = doc;
            }
            lastDoc = doc;
        }
    }
    pthread_mutex_unlock(&cache->mutex);
    if (closedir(dir) == -1)
    {
        perror("closedir");
        return 2;
    }
    return 0;
}

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);
    if (rv)
        perror(fpath);
    return rv;
}

int rmrf(char *tb)
{
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s%s", base_data_path, tb);
    printf("deleting directory: %s\n", dir_path);
    return nftw(dir_path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}