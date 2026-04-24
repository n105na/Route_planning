#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>

typedef struct {
    uint32_t node;
    double dist;
} HeapNode;

typedef struct {
    HeapNode* data;
    uint32_t* pos;     // position of each node in heap
    uint32_t size;
    uint32_t capacity;
} MinHeap;

// core functions
MinHeap* create_heap(uint32_t capacity);
void free_heap(MinHeap* h);

void push(MinHeap* h, uint32_t node, double dist);
HeapNode pop(MinHeap* h);

void decrease_key(MinHeap* h, uint32_t node, double new_dist);

int is_empty(MinHeap* h);

#endif