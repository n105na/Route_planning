#include "ch.h"
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <string.h>
#include "../heap/heap.h"

typedef struct {
    uint32_t src, dst;
    double w;
} EdgeTmp;

/* =========================
   WITNESS SEARCH
   ========================= */
static double witness_search(
    const Graph* g,
    uint32_t src,
    uint32_t target,
    uint32_t forbidden,
    double max_dist
) {
    uint32_t n = g->num_nodes;

    double* dist = malloc(n * sizeof(double));
    for (uint32_t i = 0; i < n; i++)
        dist[i] = DBL_MAX;

    MinHeap* heap = create_heap(n);

    dist[src] = 0;
    push(heap, src, 0);

    while (!is_empty(heap)) {

        HeapNode cur = pop(heap);
        uint32_t u = cur.node;

        if (dist[u] > max_dist) continue;
        if (u == target) break;

        for (uint32_t i = g->offsets[u]; i < g->offsets[u + 1]; i++) {

            uint32_t v = g->edges[i];

            if (v == forbidden) continue;

            double nd = dist[u] + g->weights[i];

            if (nd < dist[v] && nd <= max_dist) {
                dist[v] = nd;
                push(heap, v, nd);
            }
        }
    }

    double result = dist[target];

    free(dist);
    free_heap(heap);

    return result;
}

/* =========================
   CH PREPROCESS (FIXED)
   ========================= */
Graph* ch_preprocess(const Graph* g) {

    uint32_t n = g->num_nodes;
    uint32_t max_edges = g->num_edges * 50;

    EdgeTmp* temp = malloc(max_edges * sizeof(EdgeTmp));
    uint32_t edge_count = 0;

    // rank (simple)
    uint32_t* rank = malloc(n * sizeof(uint32_t));
    for (uint32_t i = 0; i < n; i++) {
        rank[i] = get_degree(g, i) * 1000 + i;
    }

    // 🔹 copy original edges
    for (uint32_t u = 0; u < n; u++) {
        for (uint32_t i = g->offsets[u]; i < g->offsets[u + 1]; i++) {

            if (edge_count >= max_edges) break;

            temp[edge_count++] = (EdgeTmp){
                u,
                g->edges[i],
                g->weights[i]
            };
        }
    }

    // 🔥 add shortcuts (WITH witness search)
    for (uint32_t u = 0; u < n; u++) {

        for (uint32_t i = g->offsets[u]; i < g->offsets[u + 1]; i++) {

            uint32_t v = g->edges[i];
            double w_uv = g->weights[i];

            for (uint32_t j = g->offsets[v]; j < g->offsets[v + 1]; j++) {

                uint32_t w = g->edges[j];
                double w_vw = g->weights[j];

                if (rank[u] < rank[w]) {

                    double cost = w_uv + w_vw;

                    double best = witness_search(g, u, w, v, cost);

                    if (best > cost) {

                        if (edge_count >= max_edges) continue;

                        temp[edge_count++] = (EdgeTmp){
                            u,
                            w,
                            cost
                        };
                    }
                }
            }
        }
    }

    printf("CH edges = %u\n", edge_count);

    /* =========================
       BUILD CSR CLEANLY
       ========================= */

    Graph* ch = create_graph(n, edge_count);

    uint32_t* degree = calloc(n, sizeof(uint32_t));

    for (uint32_t i = 0; i < edge_count; i++) {
        degree[temp[i].src]++;
    }

    ch->offsets[0] = 0;
    for (uint32_t i = 0; i < n; i++) {
        ch->offsets[i + 1] = ch->offsets[i] + degree[i];
    }

    uint32_t* cursor = malloc(n * sizeof(uint32_t));
    memcpy(cursor, ch->offsets, n * sizeof(uint32_t));

    for (uint32_t i = 0; i < edge_count; i++) {
        uint32_t u = temp[i].src;
        uint32_t pos = cursor[u]++;

        ch->edges[pos] = temp[i].dst;
        ch->weights[pos] = temp[i].w;
    }

    // copy coords
    for (uint32_t i = 0; i < n; i++) {
        ch->node_coords[i] = g->node_coords[i];
    }

    free(temp);
    free(degree);
    free(cursor);
    free(rank);

    return ch;
}

/* =========================
   CH QUERY (unchanged)
   ========================= */
double ch_query(const Graph* g, const Graph* g_rev,
                uint32_t source, uint32_t target) {

    uint32_t n = g->num_nodes;

    double* dist_f = malloc(n * sizeof(double));
    double* dist_b = malloc(n * sizeof(double));

    for (uint32_t i = 0; i < n; i++) {
        dist_f[i] = DBL_MAX;
        dist_b[i] = DBL_MAX;
    }

    dist_f[source] = 0;
    dist_b[target] = 0;

    MinHeap* pq_f = create_heap(n);
    MinHeap* pq_b = create_heap(n);

    push(pq_f, source, 0);
    push(pq_b, target, 0);

    double best = DBL_MAX;

    while (!is_empty(pq_f) || !is_empty(pq_b)) {

        if (!is_empty(pq_f)) {

            HeapNode cur = pop(pq_f);
            uint32_t u = cur.node;

            if (dist_f[u] > best) continue;

            for (uint32_t i = g->offsets[u]; i < g->offsets[u + 1]; i++) {

                uint32_t v = g->edges[i];
                double nd = dist_f[u] + g->weights[i];

                if (nd < dist_f[v]) {
                    dist_f[v] = nd;
                    push(pq_f, v, nd);
                }

                if (dist_b[v] != DBL_MAX) {
                    double path = nd + dist_b[v];
                    if (path < best) best = path;
                }
            }
        }

        if (!is_empty(pq_b)) {

            HeapNode cur = pop(pq_b);
            uint32_t u = cur.node;

            if (dist_b[u] > best) continue;

            for (uint32_t i = g_rev->offsets[u]; i < g_rev->offsets[u + 1]; i++) {

                uint32_t v = g_rev->edges[i];
                double nd = dist_b[u] + g_rev->weights[i];

                if (nd < dist_b[v]) {
                    dist_b[v] = nd;
                    push(pq_b, v, nd);
                }

                if (dist_f[v] != DBL_MAX) {
                    double path = nd + dist_f[v];
                    if (path < best) best = path;
                }
            }
        }
    }

    free(dist_f);
    free(dist_b);
    free_heap(pq_f);
    free_heap(pq_b);

    return best;
}