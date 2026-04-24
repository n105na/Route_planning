#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create graph (CSR)
 */
Graph* create_graph(uint32_t n, uint32_t e) {
    Graph* g = malloc(sizeof(Graph));
    if (!g) return NULL;

    g->num_nodes = n;
    g->num_edges = e;

    g->offsets = calloc(n + 1, sizeof(uint32_t));
    g->edges = malloc(e * sizeof(uint32_t));
    g->weights = malloc(e * sizeof(double));
    g->node_coords = malloc(n * sizeof(NodeInfo));

    if (!g->offsets || !g->edges || !g->weights || !g->node_coords) {
        free_graph(g);
        return NULL;
    }

    return g;
}

/**
 * Free memory
 */
void free_graph(Graph* g) {
    if (!g) return;

    free(g->offsets);
    free(g->edges);
    free(g->weights);
    free(g->node_coords);
    free(g);
}

/**
 * Load graph from CSV → build correct CSR
 */
Graph* load_from_csv(const char* nodes_path, const char* edges_path) {
    FILE *f_nodes = fopen(nodes_path, "r");
    FILE *f_edges = fopen(edges_path, "r");

    if (!f_nodes || !f_edges) {
        perror("File error");
        if (f_nodes) fclose(f_nodes);
        if (f_edges) fclose(f_edges);
        return NULL;
    }

    char line[1024];
    uint32_t n = 0, e = 0;

    // 🔹 count nodes
    fgets(line, sizeof(line), f_nodes);
    while (fgets(line, sizeof(line), f_nodes)) n++;

    // 🔹 count edges
    fgets(line, sizeof(line), f_edges);
    while (fgets(line, sizeof(line), f_edges)) e++;

    Graph* g = create_graph(n, e);
    if (!g) {
        fclose(f_nodes);
        fclose(f_edges);
        return NULL;
    }

    // 🔹 load nodes
    rewind(f_nodes);
    fgets(line, sizeof(line), f_nodes);

    for (uint32_t i = 0; i < n; i++) {
        uint32_t id;
        long long osm_id;

        if (fgets(line, sizeof(line), f_nodes)) {
            sscanf(line, "%u,%lld,%lf,%lf",
                   &id, &osm_id,
                   &g->node_coords[i].lat,
                   &g->node_coords[i].lon);
        }
    }

    // 🔹 temp edge structure
    typedef struct {
        uint32_t src, dst;
        double w;
    } Edge;

    Edge* edges_tmp = malloc(e * sizeof(Edge));
    if (!edges_tmp) {
        free_graph(g);
        fclose(f_nodes);
        fclose(f_edges);
        return NULL;
    }

    // 🔹 load edges
    rewind(f_edges);
    fgets(line, sizeof(line), f_edges);

    uint32_t valid_edges = 0;

    while (fgets(line, sizeof(line), f_edges)) {
        uint32_t src, dst;
        double w;

        if (sscanf(line, "%u,%u,%lf", &src, &dst, &w) == 3) {
            if (src < n && dst < n) {
                edges_tmp[valid_edges++] = (Edge){src, dst, w};
            }
        }
    }

    // 🔹 degree array
    uint32_t* degree = calloc(n, sizeof(uint32_t));
    for (uint32_t i = 0; i < valid_edges; i++) {
        degree[edges_tmp[i].src]++;
    }

    // 🔹 build offsets (prefix sum)
    g->offsets[0] = 0;
    for (uint32_t i = 0; i < n; i++) {
        g->offsets[i + 1] = g->offsets[i] + degree[i];
    }

    // 🔹 temp positions (copy offsets)
    uint32_t* temp = malloc(n * sizeof(uint32_t));
    memcpy(temp, g->offsets, n * sizeof(uint32_t));

    // 🔹 fill edges + weights
    for (uint32_t i = 0; i < valid_edges; i++) {
        uint32_t src = edges_tmp[i].src;
        uint32_t pos = temp[src];

        g->edges[pos] = edges_tmp[i].dst;
        g->weights[pos] = edges_tmp[i].w;

        temp[src]++;
    }

    // 🔹 cleanup
    free(temp);
    free(degree);
    free(edges_tmp);

    g->num_edges = valid_edges;

    fclose(f_nodes);
    fclose(f_edges);

    return g;
}

/**
 * Utils
 */
uint32_t get_degree(const Graph* g, uint32_t u) {
    return g->offsets[u + 1] - g->offsets[u];
}

uint32_t get_offset_start(const Graph* g, uint32_t u) {
    return g->offsets[u];
}

uint32_t get_offset_end(const Graph* g, uint32_t u) {
    return g->offsets[u + 1];
}