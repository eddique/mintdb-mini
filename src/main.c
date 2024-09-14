#include <stdio.h>
#include <unistd.h>
#include "heap.h"
#include "serve.h"


void cli(int argc, char *argv[], int *port, char **dir_path)
{
    int opt;

    while ((opt = getopt(argc, argv, "p:")) != -1)
    {
        switch (opt)
        {
        case 'p':
            *port = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Usage: %s [-p port] data_path\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (optind < argc)
    {
        *dir_path = argv[optind];
    }
    else
    {
        *dir_path = "data";
    }
}

int main(int argc, char *argv[])
{
    int port = 8080;
    char *dir_path = NULL;
    cli(argc, argv, &port, &dir_path);
    printf("\n\x1b[38;5;50mmintdb\x1b[0mmini\n\n");
    init_fs(dir_path);
    run(port);
    return 0;
}
