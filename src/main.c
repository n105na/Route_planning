#include "graph/graph.h"
#include <stdio.h>

/**
 * Point d'entrée principal pour tester le chargement du graphe.
 */
int main() {
    // Chemins vers les fichiers de test (jeu de données réduit)
    const char* nodes_path = "data/sample/nodes_small.csv";
    const char* edges_path = "data/sample/edges_small.csv";

    printf("Chargement du graphe de test...\n");
    
    // Tentative de chargement du graphe via la structure CSR
    Graph* g = load_from_csv(nodes_path, edges_path);

    // Vérification si le chargement a réussi
    if (g == NULL) {
        printf("Erreur : Impossible de charger le graphe.\n");
        return 1;
    }

    // Affichage des informations pour validation
    printf("Succès !\n");
    printf("Nombre de nœuds chargés : %u\n", g->num_nodes);
    printf("Nombre d'arêtes chargées : %u\n", g->num_edges);

    // Test de cohérence : affichage des voisins du premier nœud (ID 0)
    if (g->num_nodes > 0) {
        // Dans une structure CSR, les voisins du nœud i sont entre offsets[i] et offsets[i+1]
        uint32_t start = g->offsets[0];
        uint32_t end = g->offsets[1];
        printf("Le nœud 0 a %u voisin(s).\n", end - start);
    }

    // Libération de la mémoire allouée pour éviter les fuites (memory leaks)
    free_graph(g);
    
    return 0;
}
