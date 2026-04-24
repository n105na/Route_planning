#include "heap.h"
#include <stdlib.h>

static void swap(HeapNode* a, HeapNode* b) {
    HeapNode tmp = *a;
    *a = *b;
    *b = tmp;
}

MinHeap* create_heap(uint32_t capacity) {
    MinHeap* h = malloc(sizeof(MinHeap));

    h->data = malloc(capacity * sizeof(HeapNode));
    h->pos = malloc(capacity * sizeof(uint32_t));
    h->size = 0;
    h->capacity = capacity;

    return h;
}

void free_heap(MinHeap* h) {
    free(h->data);
    free(h->pos);
    free(h);
}

static void heapify_up(MinHeap* h, int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;

        if (h->data[parent].dist <= h->data[i].dist)
            break;

        // swap
        swap(&h->data[parent], &h->data[i]);

        h->pos[h->data[parent].node] = parent;
        h->pos[h->data[i].node] = i;

        i = parent;
    }
}

static void heapify_down(MinHeap* h, int i) {
    while (1) {
        int left = 2*i + 1;
        int right = 2*i + 2;
        int smallest = i;

        if (left < h->size &&
            h->data[left].dist < h->data[smallest].dist)
            smallest = left;

        if (right < h->size &&
            h->data[right].dist < h->data[smallest].dist)
            smallest = right;

        if (smallest == i)
            break;

        swap(&h->data[i], &h->data[smallest]);

        h->pos[h->data[i].node] = i;
        h->pos[h->data[smallest].node] = smallest;

        i = smallest;
    }
}

void push(MinHeap* h, uint32_t node, double dist) {
    int i = h->size++;

    h->data[i].node = node;
    h->data[i].dist = dist;
    h->pos[node] = i;

    heapify_up(h, i);
}

HeapNode pop(MinHeap* h) {
    HeapNode root = h->data[0];

    h->data[0] = h->data[h->size - 1];
    h->pos[h->data[0].node] = 0;

    h->size--;

    heapify_down(h, 0);

    return root;
}

void decrease_key(MinHeap* h, uint32_t node, double new_dist) {
    int i = h->pos[node];

    h->data[i].dist = new_dist;

    heapify_up(h, i);
}

int is_empty(MinHeap* h) {
    return h->size == 0;
}