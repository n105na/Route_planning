#include "graph/graph.h"
#include <stdio.h>

/**
 * Entry point to test graph loading + CSR correctness
 */
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

    // 🔹 TEST PRINCIPAL → node 0
    if (g->num_nodes > 0) {
        uint32_t u = 0;

        uint32_t start = get_offset_start(g, u);
        uint32_t end   = get_offset_end(g, u);
        uint32_t degree = end - start;

        printf("\nLe nœud %u a %u voisin(s).\n", u, degree);

        // 🔹 afficher voisins
        printf("Voisins de %u :\n", u);

        for (uint32_t i = start; i < end; i++) {
            uint32_t v = g->edges[i];
            double w = g->weights[i];

            printf(" -> %u (poids = %.2f)\n", v, w);
        }
    }

    // 🔹 BONUS DEBUG (important)
    printf("\n--- DEBUG (10 premiers noeuds) ---\n");
    for (uint32_t i = 0; i < 200 && i < g->num_nodes; i++) {
        printf("Node %u degree = %u\n", i, get_degree(g, i));
    }

    free_graph(g);
    return 0;
}