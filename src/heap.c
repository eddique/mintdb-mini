#include "heap.h"

MaxHeap *create_heap(int capacity)
{
    MaxHeap *heap = (MaxHeap *)malloc(sizeof(MaxHeap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->nodes = (HeapNode **)malloc(capacity * sizeof(HeapNode *));
    return heap;
}

// TODO: Fix for data pointers, should it free here?
void free_heap(MaxHeap *heap)
{
    for (int i = 0; i < heap->size; i++)
    {
        free(heap->nodes[i]->data);
        heap->nodes[i]->data = NULL;
        free(heap->nodes[i]);
        heap->nodes[i] = NULL;
    }
    free(heap->nodes);
    heap->nodes = NULL;
    free(heap);
    heap = NULL;
}


HeapNode *peek(MaxHeap *heap)
{
    if (heap->size == 0)
        return NULL;
    return heap->nodes[0];
}

void swap(HeapNode **x, HeapNode **y)
{
    HeapNode *temp = *x;
    *x = *y;
    *y = temp;
}

void heapify(MaxHeap *heap, int idx)
{
    int largest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->nodes[left]->cosine_distance > heap->nodes[largest]->cosine_distance)
        largest = left;

    if (right < heap->size && heap->nodes[right]->cosine_distance > heap->nodes[largest]->cosine_distance)
        largest = right;

    if (largest != idx)
    {
        swap(&(heap->nodes[largest]), &(heap->nodes[idx]));
        heapify(heap, largest);
    }
}

void push(MaxHeap *heap, float cosine_distance, Data *data)
{
    if (heap->size == heap->capacity)
    {
        printf("size: %d - capacity: %d", heap->size, heap->capacity);
        printf("\nOverflow: Could not insert Key\n");
        return;
    }
    HeapNode *node = (HeapNode *)malloc(sizeof(HeapNode));
    node->cosine_distance = cosine_distance;
    node->data = data;

    heap->nodes[heap->size] = node;
    int i = heap->size++;
    while (i != 0 && heap->nodes[(i - 1) / 2]->cosine_distance < heap->nodes[i]->cosine_distance)
    {
        swap(&heap->nodes[i], &heap->nodes[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

HeapNode *pop(MaxHeap *heap)
{
    if (heap->size <= 0)
        return NULL;

    if (heap->size == 1)
    {
        heap->size--;
        return heap->nodes[0];
    }

    HeapNode *root = heap->nodes[0];
    heap->nodes[0] = heap->nodes[heap->size - 1];
    heap->size--;
    heapify(heap, 0);

    return root;
}