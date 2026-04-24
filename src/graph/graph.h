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
    uint32_t* offsets;    // Taille: num_nodes + 1
    uint32_t* edges;      // Taille: num_edges
    double* weights;      // Taille: num_edges

    // Coordonnées géographiques (pour A*)
    NodeInfo* node_coords; // Taille: num_nodes

} Graph;

/**
 * Création d'un graphe vide avec allocation mémoire.
 */
Graph* create_graph(uint32_t n, uint32_t e);

/**
 * Libération de toute la mémoire du graphe.
 */
void free_graph(Graph* g);

/**
 * Chargement du graphe depuis des fichiers CSV.
 * nodes_path : fichier des nœuds (id, lat, lon)
 * edges_path : fichier des arêtes (src, dst, weight)
 */
Graph* load_from_csv(const char* nodes_path, const char* edges_path);

/**
 * Retourne le nombre de voisins d'un nœud u.
 */
uint32_t get_degree(const Graph* g, uint32_t u);

/**
 * Retourne l'indice de début des voisins dans edges[].
 */
uint32_t get_offset_start(const Graph* g, uint32_t u);

/**
 * Retourne l'indice de fin des voisins dans edges[].
 */
uint32_t get_offset_end(const Graph* g, uint32_t u);

#endif