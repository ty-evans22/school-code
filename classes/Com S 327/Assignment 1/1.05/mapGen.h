#ifndef MAPGEN_H
#define MAPGEN_H

#include "world.h"

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

extern pair_t direction_vals[9];

int new_map();

#endif