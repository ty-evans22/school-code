#ifndef MAPGEN_H
#define MAPGEN_H

#include <stdint.h>

#include "heap.h"

#define MAP_X              80
#define MAP_Y              21

typedef struct path {
    heap_node_t *hn;
    uint8_t pos[2];
    uint8_t from[2];
    int32_t cost;
} path_t;

typedef enum __attribute__ ((__packed__)) terrain_type {
    ter_debug,
    ter_boulder,
    ter_tree,
    ter_path,
    ter_mart,
    ter_center,
    ter_grass,
    ter_clearing,
    ter_mountain,
    ter_forest
} terrain_type_t;

typedef enum dim {
    dim_x,
    dim_y,
    num_dims
} dim_t;

typedef uint8_t pair_t[num_dims];

typedef struct player {
    uint8_t pos[2];
} player_t;

typedef struct map {
    terrain_type_t map[MAP_Y][MAP_X];
    uint8_t height[MAP_Y][MAP_X];
    uint8_t n, s, e, w;
} map_t;

int32_t path_cmp(const void *key, const void *with);
void place_player(map_t *m, player_t *p);
int new_map(map_t *m, uint8_t n, uint8_t s, uint8_t e, uint8_t w, int poke_place_prob);
void print_map(map_t *m, player_t *p);

#endif