#include "astar.h"
#include "../heap/heap.h"
#include <stdlib.h>
#include <float.h>
#include <math.h>

double heuristic(const Graph* g, uint32_t u, uint32_t t) {
    double dx = g->node_coords[u].lat - g->node_coords[t].lat;
    double dy = g->node_coords[u].lon - g->node_coords[t].lon;
    return sqrt(dx*dx + dy*dy);
}

double astar(const Graph* g, uint32_t source, uint32_t target) {

    uint32_t n = g->num_nodes;

    double* dist = malloc(n * sizeof(double));

    for (uint32_t i = 0; i < n; i++)
        dist[i] = DBL_MAX;

    dist[source] = 0.0;

    MinHeap* heap = create_heap(n);

    // 🔥 key difference: use f = g + h
    push(heap, source, heuristic(g, source, target));

    while (!is_empty(heap)) {

        HeapNode top = pop(heap);
        uint32_t u = top.node;

        // 🔥 stop early when we reach target
        if (u == target)
            break;

        for (uint32_t i = g->offsets[u]; i < g->offsets[u + 1]; i++) {

            uint32_t v = g->edges[i];
            double w = g->weights[i];

            double new_dist = dist[u] + w;

            if (new_dist < dist[v]) {
                dist[v] = new_dist;

                double priority = new_dist + heuristic(g, v, target);

                push(heap, v, priority);
            }
        }
    }

    double result = dist[target];

    free(dist);
    free_heap(heap);

    return result;
}