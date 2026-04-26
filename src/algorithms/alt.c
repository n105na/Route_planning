#include "alt.h"
#include "dijkstra.h"
#include "../heap/heap.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <float.h>

ALT* alt_preprocess(const Graph* g, uint32_t k) {

    ALT* alt = malloc(sizeof(ALT));

    alt->num_landmarks = k;
    alt->landmarks = malloc(k * sizeof(uint32_t));
    alt->dist = malloc(k * sizeof(double*));

    printf("ALT preprocessing with %u landmarks...\n", k);

    for (uint32_t i = 0; i < k; i++) {

        uint32_t L = rand() % g->num_nodes;  // random landmark

        alt->landmarks[i] = L;

        printf("Landmark %u = %u\n", i, L);

        alt->dist[i] = dijkstra(g, L);  // store all distances
    }

    printf("ALT preprocessing done.\n");

    return alt;
}
double alt_heuristic(const ALT* alt, uint32_t v, uint32_t t) {

    double h = 0.0;

    for (uint32_t i = 0; i < alt->num_landmarks; i++) {

        double dLt = alt->dist[i][t];
        double dLv = alt->dist[i][v];

        double val = fabs(dLt - dLv);

        if (val > h)
            h = val;
    }

    return h;
}
double alt_query(const Graph* g, ALT* alt, uint32_t source, uint32_t target) {

    uint32_t n = g->num_nodes;

    double* dist = malloc(n * sizeof(double));

    for (uint32_t i = 0; i < n; i++)
        dist[i] = DBL_MAX;

    dist[source] = 0.0;

    MinHeap* heap = create_heap(n);

    // 🔥 push with ALT heuristic
    push(heap, source, alt_heuristic(alt, source, target));

    while (!is_empty(heap)) {

        HeapNode top = pop(heap);
        uint32_t u = top.node;

        if (u == target)
            break;

        for (uint32_t i = g->offsets[u]; i < g->offsets[u + 1]; i++) {

            uint32_t v = g->edges[i];
            double w = g->weights[i];

            double new_dist = dist[u] + w;

            if (new_dist < dist[v]) {

                dist[v] = new_dist;

                double priority =
                    new_dist + alt_heuristic(alt, v, target);

                push(heap, v, priority);
            }
        }
    }

    double result = dist[target];

    free(dist);
    free_heap(heap);

    return result;
}
void free_alt(ALT* alt) {

    for (uint32_t i = 0; i < alt->num_landmarks; i++) {
        free(alt->dist[i]);
    }

    free(alt->dist);
    free(alt->landmarks);
    free(alt);
}