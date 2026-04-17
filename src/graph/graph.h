#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

/**
 * Structure stockant les coordonnées géographiques d'un nœud.
 */
typedef struct {
    double lat;
    double lon;
} NodeInfo;

/**
 * Représentation du graphe en format CSR (Compressed Sparse Row).
 */
typedef struct {
    uint32_t num_nodes;   // Nombre total de sommets (V)
    uint32_t num_edges;   // Nombre total d'arêtes (E)

    // Structure CSR
    uint32_t* offsets;    // Tableau d'indexation des voisins (taille: num_nodes + 1)
    uint32_t* edges;      // Tableau des nœuds destinataires (taille: num_edges)
    double* weights;      // Tableau des distances/poids (taille: num_edges)

    NodeInfo* node_coords; // Tableau des coordonnées GPS de chaque nœud
} Graph;

// Fonctions de gestion du graphe
Graph* create_graph(uint32_t n, uint32_t e);
void free_graph(Graph* g);
Graph* load_from_csv(const char* nodes_path, const char* edges_path);

#endif
