#include <stdlib.h>
#include <assert.h>
#include "heap.h"

void test_heap_initialization()
{
    // Fixture:
    int capacity = 5;
    MaxHeap *heap = create_heap(capacity);

    // Test:
    assert(heap->capacity == capacity);
    assert(heap->size == 0);

    // Cleanup:
    free_heap(heap);
}

void test_heap_insertion()
{
    // Fixture:
    int capacity = 5;
    float distances[] = {0.768, 0.239, 0.492, 0.857, 0.315, 0.604, 0.142, 0.981, 0.027, 0.463, 0.716, 0.201, 0.894, 0.356, 0.673, 0.028, 0.915, 0.739, 0.482, 0.206};
    size_t distances_len = sizeof(distances) / sizeof(float);
    MaxHeap *heap = create_heap(capacity);
    for (int i = 0; i < distances_len; i++)
    {
        Data *data = (Data *)malloc(sizeof(Data));
        if (heap->size < heap->capacity)
        {
            push(heap, distances[i], data);
        }
        else
        {
            HeapNode *node = peek(heap);
            if (distances[i] < node->cosine_distance)
            {
                HeapNode *top = pop(heap);
                free(top->data);
                free(top);
                push(heap, distances[i], data);
            }
        }
    }

    // Test:
    assert(heap->size == capacity);
    
    free_heap(heap);
}

void test_heap_order()
{
    // Fixture:
    int capacity = 5;
    float distances[] = {0.768, 0.239, 0.492, 0.857, 0.315, 0.604, 0.142, 0.981, 0.027, 0.463, 0.716, 0.201, 0.894, 0.356, 0.673, 0.028, 0.915, 0.739, 0.482, 0.206};
    size_t distances_len = sizeof(distances) / sizeof(float);
    MaxHeap *heap = create_heap(capacity);
    for (int i = 0; i < distances_len; i++)
    {
        Data *data = (Data *)malloc(sizeof(Data));
        if (heap->size < heap->capacity)
        {
            push(heap, distances[i], data);
        }
        else
        {
            HeapNode *node = peek(heap);
            if (distances[i] < node->cosine_distance)
            {
                HeapNode *top = pop(heap);
                free(top->data);
                free(top);
                push(heap, distances[i], data);
            }
        }
    }

    // Test:
    assert(heap->size == capacity);
    float last_distance = 1.0;
    HeapNode *node = pop(heap);
    while (node != NULL)
    {
        assert(node->cosine_distance <= last_distance);
        last_distance = node->cosine_distance;
        free(node);
        node = NULL;
        node = pop(heap);
    }

    // Cleanup:

    // Cleanup:
    free_heap(heap);
}

int main() {
    test_heap_initialization();
    test_heap_insertion();
    test_heap_order();
    printf("All unit tests passed.\n");
    return 0;
}
