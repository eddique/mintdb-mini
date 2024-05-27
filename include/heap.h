#ifndef mintdb_mini_heap_h
#define mintdb_mini_heap_h
#include "common.h"

typedef struct {
    float cosine_distance;
    Data *data;
} HeapNode;

typedef struct {
    int size;
    int capacity;
    HeapNode **nodes;
} MaxHeap;

MaxHeap *create_heap(int capacity);
void free_heap(MaxHeap *heap);
HeapNode *peek(MaxHeap *heap);
HeapNode *pop(MaxHeap *heap);
void push(MaxHeap *heap, float cosine_distance, Data *data);
#endif