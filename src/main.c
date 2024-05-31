#include "serve.h"
#include <stdio.h>
#include <unistd.h>

#include "heap.h"

// void test_heap()
// {
//     int capacity = 5;
//     float distances[] = {0.768, 0.239, 0.492, 0.857, 0.315, 0.604, 0.142, 0.981, 0.027, 0.463, 0.716, 0.201, 0.894, 0.356, 0.673, 0.028, 0.915, 0.739, 0.482, 0.206};
//     MaxHeap *heap = create_heap(capacity);
//     for (int i = 0; i < sizeof(distances) / sizeof(float); i++)
//     {
//         Data *data = (Data *)malloc(sizeof(Data));
//         if (heap->size < heap->capacity)
//         {
//             push(heap, distances[i], data);
//         }
//         else
//         {
//             HeapNode *node = peek(heap);
//             if (distances[i] < node->cosine_distance)
//             {
//                 HeapNode *top = pop(heap);
//                 free(top->data);
//                 free(top);
//                 push(heap, distances[i], data);
//             }
//         }
//     }
//     float documents[heap->capacity];
//     int index = heap->capacity - 1;
//     HeapNode *node = pop(heap);
//     while (node != NULL)
//     {
//         documents[index] = node->cosine_distance;
//         index--;
//         printf("pop: %f\n", node->cosine_distance);
//         node = pop(heap);
//     }
//     printf("cosine_distances:\n");
//     for (int i = 0; i <  sizeof(documents)/sizeof(float); i ++)
//     {
//         printf("%f\n", documents[i]);
//     }
//     free_heap(heap);
// }
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
