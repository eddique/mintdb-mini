
#include "fs.h"
#include "heap.h"
#include "emb.h"

char base_data_path[256];

Cache *cache = NULL;

void print_titles()
{
    if (cache != NULL)
    {
        pthread_mutex_lock(&cache->mutex);
        if (strlen(cache->tb) > 0)
        {
            Document *doc = cache->documents;
            while (doc != NULL)
            {
                Document *next = doc->next;
                printf("Cache Loaded - document: %s\n", doc->data->title);
                doc = next;
            }
        }
        pthread_mutex_unlock(&cache->mutex);
    }
}

void init_fs(const char *base_path)
{
    printf("\x1b[38;5;50mcreating directory \"%s\"\x1b[0m\n", base_path);
    strcpy(base_data_path, base_path);
    snprintf(base_data_path, sizeof(base_data_path), "%s/%s", base_path, "ds/");
    make_dir(base_data_path);
}

char *cache_tb()
{
    if (cache != NULL)
    {
        char *tb;
        pthread_mutex_lock(&cache->mutex);
        if (strlen(cache->tb) > 0)
        {

            tb = strdup(cache->tb);
        }
        else
        {
            tb = "";
        }
        pthread_mutex_unlock(&cache->mutex);
        return tb;
    }
    return NULL;
}

void reset_cache()
{
    if (cache != NULL)
    {
        pthread_mutex_lock(&cache->mutex);
        if (strlen(cache->tb) > 0)
        {

            memset(cache->tb, 0, MAX_FIELD_SIZE);
            Document *node = cache->documents;
            Document *next;
            while (node != NULL)
            {
                next = node->next;
                if (node->data != NULL)
                {
                    free(node->data->content);
                    free(node->data->embeddings);
                    free(node->data);
                }
                free(node);
                node = next;
            }
        }
        pthread_mutex_unlock(&cache->mutex);
    }
}

void free_cache()
{
    if (cache != NULL)
    {
        pthread_mutex_lock(&cache->mutex);
        Document *node = cache->documents;
        Document *next;
        while (node != NULL)
        {
            next = node->next;
            free(node->data->content);
            free(node->data->embeddings);
            free(node->data);
            free(node);
            node = next;
        }
        pthread_mutex_unlock(&cache->mutex);
        pthread_mutex_destroy(&cache->mutex);
        free(cache);
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
        char *data = (char *)malloc((data_len + 1) * sizeof(char));
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
void write_document(const char *tb, Data *document)
{
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "%s/%s%s", base_data_path, tb, "/");
    make_dir(dir_path);
    char file_path[512];
    snprintf(file_path, sizeof(file_path), "%s/%s.%s", dir_path, document->id, "bin");
    FILE *file = fopen(file_path, "w");
    if (file != NULL)
    {
        char *embedding = (char *)malloc((EMBEDDING_SIZE * 38 + 2) * sizeof(char));
        char *p = embedding;
        *p++ = '[';
        for (int i = 0; i < EMBEDDING_SIZE; i++)
        {
            int written = snprintf(p, 38, "%.32f,", document->embeddings[i]);
            p += written;
        }
        *(p - 1) = ']';
        *p = '\0';

        size_t data_len = strlen(document->id) + strlen(document->title) + strlen(document->content) +
                          strlen(document->url) + strlen(embedding) + 4;
        char *data = (char *)malloc((data_len + 1) * sizeof(char));
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
            free(embedding);
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
void query_embeddings(char *table, double *embeddings, char *response)
{
    if (cache != NULL)
    {
        char *tb = cache_tb();
        if (tb != NULL && strncmp(tb, table, strlen(table)) != 0)
        {
            reset_cache();
            set_cache(table);
        }
        int capacity = 5;
        MaxHeap *heap = create_heap(capacity);
        Document *document = cache->documents;
        while (document != NULL)
        {
            double distance = cosine_distance(embeddings, document->data->embeddings, EMBEDDING_SIZE);
            if (heap->size < heap->capacity)
            {
                push(heap, distance, document->data);
            }
            else
            {
                HeapNode *top = peek(heap);
                if (distance < top->cosine_distance)
                {
                    top = pop(heap);
                    free(top);
                    push(heap, distance, document->data);
                }
            }
            document = document->next;
        }
        HeapNode **nodes = (HeapNode **)malloc(heap->capacity * sizeof(HeapNode *));
        int index = heap->capacity - 1;
        HeapNode *node = pop(heap);
        while (node != NULL)
        {
            nodes[index--] = node;
            node = pop(heap);
        }
        sprintf(response, "{\"results\":[");
        for (int i = 0; i < heap->capacity; i++)
        {
            Data *data = nodes[i]->data;
            HeapNode *node = nodes[i];
            sprintf(response + strlen(response),
                    "{\"id\": \"%s\", \"title\": \"%s\", \"content\": \"%s\", \"url\": \"%s\", \"distance\": %.7f},",
                    data->id, data->title, data->content, data->url, node->cosine_distance);
        }

        response[strlen(response) - 1] = ']';
        strcat(response, "}");
        free(nodes);
        free_heap(heap);
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

    struct stat st;

    // TODO: Handle error
    fstat(fileno(file), &st);
    size_t file_size = st.st_size;
    char *file_contents = (char *)malloc(file_size + 1);

    fread(file_contents, sizeof(char), file_size, file);
    file_contents[file_size] = '\0';
    char *token = strtok(file_contents, "|");
    strncpy(data->id, token, MAX_FIELD_SIZE);

    token = strtok(NULL, "|");
    strncpy(data->title, token, MAX_FIELD_SIZE);

    token = strtok(NULL, "|");
    data->content = (char *)malloc(strlen(token) + 1);
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
        data->embeddings[i] = strtod(token, NULL);
        token = NULL;
    }
    free(file_contents);
    fclose(file);
}
void init_cache()
{
    cache = (Cache *)malloc(sizeof(Cache));
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
    snprintf(path, sizeof(path), "%s%s", base_data_path, tb);
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
            Data *data = (Data *)malloc(sizeof(Data));
            data->embeddings = (double *)malloc(EMBEDDING_SIZE * sizeof(double));
            Document *doc = (Document *)malloc(sizeof(Document));
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