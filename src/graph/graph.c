#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Alloue la mémoire nécessaire pour la structure du graphe (CSR).
 * Cette fonction prépare les tableaux pour n nœuds et e arêtes.
 */
Graph* create_graph(uint32_t n, uint32_t e) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) return NULL;

    g->num_nodes = n;
    g->num_edges = e;

    // Allocation des tableaux CSR
    g->offsets = (uint32_t*)calloc(n + 1, sizeof(uint32_t));
    g->edges = (uint32_t*)malloc(e * sizeof(uint32_t));
    g->weights = (double*)malloc(e * sizeof(double));
    
    // Allocation pour les informations géographiques (A* et ALT)
    g->node_coords = (NodeInfo*)malloc(n * sizeof(NodeInfo));

    // Vérification de l'allocation
    if (!g->offsets || !g->edges || !g->weights || !g->node_coords) {
        free_graph(g);
        return NULL;
    }

    return g;
}

/**
 * Libère proprement toute la mémoire allouée pour le graphe.
 */
void free_graph(Graph* g) {
    if (g) {
        if (g->offsets) free(g->offsets);
        if (g->edges) free(g->edges);
        if (g->weights) free(g->weights);
        if (g->node_coords) free(g->node_coords);
        free(g);
    }
}

/**
 * Charge les données depuis les CSV et construit la structure CSR.
 * Note : edges.csv doit être trié par la colonne 'src'.
 */
Graph* load_from_csv(const char* nodes_path, const char* edges_path) {
    FILE *f_nodes = fopen(nodes_path, "r");
    FILE *f_edges = fopen(edges_path, "r");

    if (!f_nodes || !f_edges) {
        perror("Erreur d'ouverture des fichiers");
        if (f_nodes) fclose(f_nodes);
        if (f_edges) fclose(f_edges);
        return NULL;
    }

    char line[1024];
    uint32_t n = 0, e = 0;

    // 1. Comptage des nœuds et des arêtes pour l'allocation
    fgets(line, sizeof(line), f_nodes); // Ignorer l'en-tête
    while (fgets(line, sizeof(line), f_nodes)) n++;

    fgets(line, sizeof(line), f_edges); // Ignorer l'en-tête
    while (fgets(line, sizeof(line), f_edges)) e++;

    // 2. Création de l'objet Graph
    Graph* g = create_graph(n, e);
    if (!g) {
        fclose(f_nodes);
        fclose(f_edges);
        return NULL;
    }

    // 3. Remplissage des coordonnées (lat/lon)
    rewind(f_nodes);
    fgets(line, sizeof(line), f_nodes); // Sauter l'en-tête
    for (uint32_t i = 0; i < n; i++) {
        uint32_t id;
        long long osm_id;
        if (fgets(line, sizeof(line), f_nodes)) {
            sscanf(line, "%u,%lld,%lf,%lf", &id, &osm_id, &g->node_coords[i].lat, &g->node_coords[i].lon);
        }
    }

    // 4. Construction de la structure CSR à partir des arêtes
    rewind(f_edges);
    fgets(line, sizeof(line), f_edges); // Sauter l'en-tête
    
    uint32_t current_node = 0;
    for (uint32_t i = 0; i < e; i++) {
        uint32_t src, dst;
        double weight;
        if (fgets(line, sizeof(line), f_edges)) {
            sscanf(line, "%u,%u,%lf", &src, &dst, &weight);
            
            g->edges[i] = dst;
            g->weights[i] = weight;

            // Gestion de l'indexation (offsets)
            while (current_node <= src) {
                g->offsets[current_node + 1] = i + 1;
                current_node++;
            }
        }
    }
    
    // Finalisation des offsets pour les nœuds isolés
    while (current_node < n) {
        g->offsets[current_node + 1] = e;
        current_node++;
    }

    fclose(f_nodes);
    fclose(f_edges);
    return g;
}
