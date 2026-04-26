#ifndef CH_H
#define CH_H

#include "../graph/graph.h"

// build CH graph (adds shortcuts)
Graph* ch_preprocess(const Graph* g);

// shortest path with CH (bidirectional)
double ch_query(const Graph* g, const Graph* g_rev,
                uint32_t source, uint32_t target);

#endif