#include "graph/graph.h"
#include "algorithms/dijkstra.h"
#include "algorithms/astar.h"
#include "algorithms/alt.h"
#include "algorithms/ch.h"

#include <stdio.h>
#include <stdlib.h>

int main() {

    const char* nodes_path = "data/sample/nodes_small.csv";
    const char* edges_path = "data/sample/edges_small.csv";

    printf("Loading graph...\n");

    Graph* g = load_from_csv(nodes_path, edges_path);

    if (!g) {
        printf("Error loading graph\n");
        return 1;
    }

    printf("Nodes: %u | Edges: %u\n", g->num_nodes, g->num_edges);

    uint32_t source = 0;
    uint32_t target = 1;

    // 🔹 DIJKSTRA
    double* dist = dijkstra(g, source);
    printf("Dijkstra: %.2f\n", dist[target]);
    free(dist);

    // 🔹 A*
    double d_astar = astar(g, source, target);
    printf("A*: %.2f\n", d_astar);

    // 🔹 ALT
    ALT* alt = alt_preprocess(g, 3);
    double d_alt = alt_query(g, alt, source, target);
    printf("ALT: %.2f\n", d_alt);
    free_alt(alt);

    // 🔹 CH
    Graph* chg = ch_preprocess(g);
    Graph* rev = build_reverse_graph(chg);

    double d_ch = ch_query(chg, rev, source, target);
    printf("CH: %.2f\n", d_ch);

    free_graph(rev);
    free_graph(chg);
    free_graph(g);

    return 0;
}