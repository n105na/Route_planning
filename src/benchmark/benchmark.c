#include "../graph/graph.h"
#include "../algorithms/dijkstra.h"
#include "../algorithms/astar.h"
#include "../algorithms/alt.h"
#include "../algorithms/ch.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>

#define NUM_QUERIES 500

double now_ms() {
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec * 1000.0 + t.tv_nsec / 1e6;
}

int cmp_double(const void* a, const void* b) {
    double x = *(double*)a;
    double y = *(double*)b;
    return (x > y) - (x < y);
}

/* =========================
   MEMORY ESTIMATION
   ========================= */
double estimate_graph_memory(const Graph* g) {
    size_t mem = 0;

    mem += (g->num_nodes + 1) * sizeof(uint32_t);
    mem += g->num_edges * sizeof(uint32_t);
    mem += g->num_edges * sizeof(double);
    mem += g->num_nodes * sizeof(NodeInfo);
    mem += g->num_nodes * sizeof(uint32_t);

    return mem / (1024.0 * 1024.0);
}

/* =========================
   DISTANCE
   ========================= */
double geo_dist(uint32_t a, uint32_t b, Graph* g) {
    double dx = g->node_coords[a].lat - g->node_coords[b].lat;
    double dy = g->node_coords[a].lon - g->node_coords[b].lon;
    return sqrt(dx*dx + dy*dy) * 100000;
}

int main() {

    const char* nodes_path = "data/sample/nodes_small.csv";
    const char* edges_path = "data/sample/edges_small.csv";

    Graph* g = load_from_csv(nodes_path, edges_path);

    if (!g) {
        printf("Error loading graph\n");
        return 1;
    }

    printf("Nodes: %u | Edges: %u\n", g->num_nodes, g->num_edges);
    printf("Graph memory: %.2f MB\n", estimate_graph_memory(g));

    srand(42);

    /* =========================
       OUTPUT FILES
       ========================= */
    FILE* f = fopen("results/csv/results.csv", "w");
    fprintf(f, "query,algo,time_ms,distance,type\n");

    /* =========================
       PREPROCESS
       ========================= */
    double t0 = now_ms();
    ALT* alt = alt_preprocess(g, 3);
    double alt_pre = now_ms() - t0;

    t0 = now_ms();
    Graph* chg = ch_preprocess(g);
    Graph* rev = build_reverse_graph(chg);
    double ch_pre = now_ms() - t0;

    printf("CH memory: %.2f MB\n", estimate_graph_memory(chg));

    double t_dij[NUM_QUERIES];
    double t_astar[NUM_QUERIES];
    double t_alt[NUM_QUERIES];
    double t_ch[NUM_QUERIES];

    double sum_dij = 0, sum_astar = 0, sum_alt = 0, sum_ch = 0;

    int short_q = 0, medium_q = 0, long_q = 0;

    int valid = 0;

    /* =========================
       QUERIES
       ========================= */
    for (int i = 0; i < NUM_QUERIES; i++) {

        uint32_t s = rand() % g->num_nodes;
        uint32_t t = rand() % g->num_nodes;
        if (s == t) continue;

        double d_geo = geo_dist(s, t, g);

        const char* type;
        if (d_geo < 500) { short_q++; type = "short"; }
        else if (d_geo < 2000) { medium_q++; type = "medium"; }
        else { long_q++; type = "long"; }

        // Dijkstra
        double start = now_ms();
        double* dist = dijkstra(g, s);
        double d1 = dist[t];
        t_dij[valid] = now_ms() - start;
        sum_dij += t_dij[valid];
        fprintf(f, "%d,dijkstra,%.6f,%.6f,%s\n", valid, t_dij[valid], d1, type);
        free(dist);

        // A*
        start = now_ms();
        double d2 = astar(g, s, t);
        t_astar[valid] = now_ms() - start;
        sum_astar += t_astar[valid];
        fprintf(f, "%d,astar,%.6f,%.6f,%s\n", valid, t_astar[valid], d2, type);

        // ALT
        start = now_ms();
        double d3 = alt_query(g, alt, s, t);
        t_alt[valid] = now_ms() - start;
        sum_alt += t_alt[valid];
        fprintf(f, "%d,alt,%.6f,%.6f,%s\n", valid, t_alt[valid], d3, type);

        // CH
        start = now_ms();
        double d4 = ch_query(chg, rev, s, t);
        t_ch[valid] = now_ms() - start;
        sum_ch += t_ch[valid];
        fprintf(f, "%d,ch,%.6f,%.6f,%s\n", valid, t_ch[valid], d4, type);

        // correctness
        if (fabs(d1 - d2) > 1e-6 ||
            fabs(d1 - d3) > 1e-6 ||
            fabs(d1 - d4) > 1e-6) {
            printf("Mismatch at query %d\n", i);
        }

        valid++;
    }

    fclose(f);

    /* =========================
       SORT
       ========================= */
    qsort(t_dij, valid, sizeof(double), cmp_double);
    qsort(t_astar, valid, sizeof(double), cmp_double);
    qsort(t_alt, valid, sizeof(double), cmp_double);
    qsort(t_ch, valid, sizeof(double), cmp_double);

    printf("\n===== BENCHMARK =====\n");

    printf("Dijkstra: avg=%.3f p50=%.3f p95=%.3f\n",
        sum_dij/valid, t_dij[valid/2], t_dij[(int)(0.95*valid)]);

    printf("A*:       avg=%.3f p50=%.3f p95=%.3f\n",
        sum_astar/valid, t_astar[valid/2], t_astar[(int)(0.95*valid)]);

    printf("ALT:      avg=%.3f p50=%.3f p95=%.3f\n",
        sum_alt/valid, t_alt[valid/2], t_alt[(int)(0.95*valid)]);

    printf("CH:       avg=%.3f p50=%.3f p95=%.3f\n",
        sum_ch/valid, t_ch[valid/2], t_ch[(int)(0.95*valid)]);

    printf("\nPreprocess ALT: %.3f ms\n", alt_pre);
    printf("Preprocess CH:  %.3f ms\n", ch_pre);

    free_alt(alt);
    free_graph(rev);
    free_graph(chg);
    free_graph(g);

    return 0;
}