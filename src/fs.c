
#include "fs.h"

char *data_path[256];

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

void parse_and_write_to_file(char *path, char *http_body)
{
    FILE *file = fopen(path, "wb");

    if (file == NULL)
    {
        printf("Could not open file for writing.\n");
        return;
    }
    fwrite(http_body, sizeof(char), strlen(http_body), file);
    fclose(file);
}

void write_to_file(const char *filepath, const char *data, size_t data_size)
{
    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd == -1)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    size_t written = 0;
    while (written < data_size)
    {
        ssize_t result = write(fd, data + written, data_size - written);
        if (result == -1)
        {
            perror("Error writing to file");
            close(fd);
            exit(EXIT_FAILURE);
        }
        written += result;
    }

    if (close(fd) == -1)
    {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
}

void list_files(const char *path)
{
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(path);
    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    // Read and print the directory entries
    while ((entry = readdir(dir)) != NULL)
    {
        // Skip the "." and ".." entries
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // Construct the full path to the entry
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Get file status
        struct stat statbuf;
        if (stat(full_path, &statbuf) == -1)
        {
            perror("stat");
            continue;
        }

        // Check if it's a regular file or directory
        if (S_ISREG(statbuf.st_mode))
        {
            printf("File: %s\n", entry->d_name);
        }
        else if (S_ISDIR(statbuf.st_mode))
        {
            printf("Directory: %s\n", entry->d_name);
        }
        else
        {
            printf("Other: %s\n", entry->d_name);
        }
    }

    // Close the directory
    if (closedir(dir) == -1)
    {
        perror("closedir");
        exit(EXIT_FAILURE);
    }
}

char *read_file(const char *path)
{
    FILE *file = fopen(path, "rb");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\"\n", path);
        exit(74);
    }
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(fileSize + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".", path);
        exit(74);
    }
    buffer[bytesRead] = '\0';

    fclose(file);
    printf("file content: %s\n", buffer);
    return buffer;
}