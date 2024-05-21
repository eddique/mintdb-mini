#include "serve.h"
#include "fs.h"
#include "emb.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int opt;
    int port = 8080;
    char *dir_path = NULL;
    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            port = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s [-p port]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc)
    {
        dir_path = argv[optind];
    }
    else
    {
        dir_path = "mint.db";
    }
    
    printf("creating directory %s\n", dir_path);
    make_dir(dir_path);
    char file_path[256];
    snprintf(file_path, sizeof(file_path),"%s/%s", dir_path, "hello_world.txt");
    printf("reading file: %s\n", file_path);
    read_file(file_path);
    run(port);
    // char data[] = "34695ed9-4cae-429b-ad5d-69460d780ed9|Hello, World!|Some content here!|https://google.com|[0.3,0.1,0.2,0.3]";
    // snprintf(file_path, sizeof(file_path),"%s/%s", dir_path, "data2.bin");
    // parse_and_write_to_file(file_path, data);
    Data *file_data = malloc(sizeof(Data));
    Data *file_data2 = malloc(sizeof(Data));
    read_document_from_file("data/tables/users/data.bin", file_data);
    if (file_data != NULL)
    {
        printf("id: %s\n", file_data->id);
        printf("title: %s\n", file_data->title);
        printf("content: %s\n", file_data->content);
        printf("url: %s\n", file_data->url);
        for (int i = 0; i < 4; i++)
        {
            printf("embedding position %d: %f\n", i, file_data->embeddings[i]);
        }
    }
    read_document_from_file("data/tables/users/data2.bin", file_data2);
    float distance = cosine_distance(file_data2->embeddings, file_data->embeddings, 4);
    printf("cosine_distance: %f\n", distance);
    free(file_data);
    free(file_data2);
    list_files(dir_path);
    return 0;
}
