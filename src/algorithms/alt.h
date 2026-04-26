#ifndef ALT_H
#define ALT_H

#include "../graph/graph.h"

typedef struct {
    uint32_t num_landmarks;
    uint32_t* landmarks;
    double** dist;  // dist[i][v] = distance from landmark i to node v
} ALT;

// preprocessing
ALT* alt_preprocess(const Graph* g, uint32_t k);

// query
double alt_query(const Graph* g, ALT* alt, uint32_t s, uint32_t t);

// free memory
void free_alt(ALT* alt);

#endif