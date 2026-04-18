#include "graph.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Alloue la mémoire nécessaire pour la structure du graphe (CSR).
 */
Graph* create_graph(uint32_t n, uint32_t e) {
    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) return NULL;

    g->num_nodes = n;
    g->num_edges = e;

    // Allocation avec calloc pour initialiser les offsets à 0
    g->offsets = (uint32_t*)calloc(n + 1, sizeof(uint32_t));
    g->edges = (uint32_t*)malloc(e * sizeof(uint32_t));
    g->weights = (double*)malloc(e * sizeof(double));
    g->node_coords = (NodeInfo*)malloc(n * sizeof(NodeInfo));

    if (!g->offsets || !g->edges || !g->weights || !g->node_coords) {
        free_graph(g);
        return NULL;
    }

    return g;
}

/**
 * Libère proprement la mémoire.
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
 * Charge les données depuis les CSV avec une sécurité renforcée.
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

    // 1. Comptage des lignes (nœuds et arêtes)
    fgets(line, sizeof(line), f_nodes);
    while (fgets(line, sizeof(line), f_nodes)) n++;

    fgets(line, sizeof(line), f_edges);
    while (fgets(line, sizeof(line), f_edges)) e++;

    Graph* g = create_graph(n, e);
    if (!g) {
        fclose(f_nodes);
        fclose(f_edges);
        return NULL;
    }

    // 2. Chargement des coordonnées
    rewind(f_nodes);
    fgets(line, sizeof(line), f_nodes); // Sauter l'en-tête
    for (uint32_t i = 0; i < n; i++) {
        uint32_t id;
        long long osm_id;
        if (fgets(line, sizeof(line), f_nodes)) {
            sscanf(line, "%u,%lld,%lf,%lf", &id, &osm_id, &g->node_coords[i].lat, &g->node_coords[i].lon);
        }
    }

    // 3. Chargement des arêtes avec SÉCURITÉ (vérification des bornes)
    rewind(f_edges);
    fgets(line, sizeof(line), f_edges); // Sauter l'en-tête
    
    uint32_t current_node = 0;
    uint32_t valid_edges_count = 0;

    for (uint32_t i = 0; i < e; i++) {
        uint32_t src, dst;
        double weight;
        if (fgets(line, sizeof(line), f_edges)) {
            if (sscanf(line, "%u,%u,%lf", &src, &dst, &weight) == 3) {
                
                // Vérification cruciale pour éviter le Segmentation Fault
                // On vérifie si src et dst sont bien dans les limites du tableau
                if (src < n) {
                    g->edges[valid_edges_count] = dst;
                    g->weights[valid_edges_count] = weight;

                    // Mise à jour des offsets pour le nœud source
                    while (current_node <= src) {
                        g->offsets[current_node + 1] = valid_edges_count + 1;
                        current_node++;
                    }
                    valid_edges_count++;
                }
            }
        }
    }
    
    // Finalisation des offsets
    while (current_node < n) {
        g->offsets[current_node + 1] = valid_edges_count;
        current_node++;
    }

    // Mise à jour du nombre réel d'arêtes valides chargées
    g->num_edges = valid_edges_count;

    fclose(f_nodes);
    fclose(f_edges);
    return g;
}
