#include "dijkstra.h"
#include "../heap/heap.h"
#include <stdlib.h>
#include <float.h> // for DBL_MAX

double* dijkstra(const Graph* g, uint32_t source) {

    uint32_t n = g->num_nodes;

    // 🔹 distances array
    double* dist = malloc(n * sizeof(double));

    // 🔹 initialize distances
    for (uint32_t i = 0; i < n; i++)
        dist[i] = DBL_MAX;

    dist[source] = 0.0;

    // 🔹 create heap
    MinHeap* heap = create_heap(n);

    // 🔹 push source
    push(heap, source, 0.0);

    while (!is_empty(heap)) {

        HeapNode top = pop(heap);
        uint32_t u = top.node;

        // 🔹 explore neighbors of u
        for (uint32_t i = g->offsets[u]; i < g->offsets[u + 1]; i++) {

            uint32_t v = g->edges[i];
            double w = g->weights[i];

            double new_dist = dist[u] + w;

            // 🔥 RELAXATION
            if (new_dist < dist[v]) {

                dist[v] = new_dist;

                // update heap
                push(heap, v, new_dist);
                // (simple version: push again instead of decrease_key)
            }
        }
    }

    free_heap(heap);
    return dist;
}