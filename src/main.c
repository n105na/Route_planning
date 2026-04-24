#include "graph/graph.h"
#include "heap/heap.h"
#include "algorithms/dijkstra.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "algorithms/astar.h"

int main() {

    const char* nodes_path = "data/sample/nodes_small.csv";
    const char* edges_path = "data/sample/edges_small.csv";

    printf("Chargement du graphe de test...\n");

    Graph* g = load_from_csv(nodes_path, edges_path);

    if (g == NULL) {
        printf("Erreur : Impossible de charger le graphe.\n");
        return 1;
    }

    printf("Succès !\n");
    printf("Nombre de nœuds : %u\n", g->num_nodes);
    printf("Nombre d'arêtes : %u\n", g->num_edges);


    // 🔹 TEST CSR (node 0)
    if (g->num_nodes > 0) {
        uint32_t u = 0;

        uint32_t start = get_offset_start(g, u);
        uint32_t end   = get_offset_end(g, u);

        printf("\nLe nœud %u a %u voisin(s).\n", u, end - start);

        for (uint32_t i = start; i < end; i++) {
            printf(" -> %u (poids = %.2f)\n",
                   g->edges[i],
                   g->weights[i]);
        }
    }


    // 🔥 TEST HEAP
    printf("\n--- TEST HEAP ---\n");

    MinHeap* h = create_heap(10);

    push(h, 1, 5.0);
    push(h, 2, 3.0);
    push(h, 3, 8.0);

    HeapNode n = pop(h);
    printf("min = %u (%.2f)\n", n.node, n.dist);

    free_heap(h);


    // 🔥 TEST DIJKSTRA
    printf("\n--- TEST DIJKSTRA ---\n");

    double* dist = dijkstra(g, 0);

    for (int i = 0; i < 10 && i < g->num_nodes; i++) {
        if (dist[i] == DBL_MAX)
    printf("0 -> %d = unreachable\n", i);
else
    printf("0 -> %d = %.2f\n", i, dist[i]);
    }

    free(dist);
    // 🔥 TEST A*
printf("\n--- TEST A* ---\n");

uint32_t source = 0;
uint32_t target = 3;   // change if needed

double d = astar(g, source, target);

if (d == DBL_MAX)
    printf("%u -> %u = unreachable\n", source, target);
else
    printf("%u -> %u = %.2f\n", source, target, d);

    // 🔹 DEBUG GRAPH
    printf("\n--- DEBUG (20 premiers noeuds) ---\n");

    for (uint32_t i = 0; i < 20 && i < g->num_nodes; i++) {
        printf("Node %u degree = %u\n", i, get_degree(g, i));
    }


    free_graph(g);
    return 0;
}