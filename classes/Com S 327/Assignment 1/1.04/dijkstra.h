#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <stdint.h>
#include <limits.h>

#include "heap.h"
#include "world.h"
#include "trainer.h"

typedef struct path {
    heap_node_t *hn;
    uint8_t pos[2];
    uint8_t from[2];
    int32_t cost;
} path_t;

void dijkstra_path(map_t *m, pair_t from, pair_t to);
void pathfind(map_t *m);

#endif