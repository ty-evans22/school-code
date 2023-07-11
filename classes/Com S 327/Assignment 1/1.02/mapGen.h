#ifndef MAPGEN_H
#define MAPGEN_H

#include <stdint.h>

#define MAP_X              80
#define MAP_Y              21

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

typedef struct map {
    terrain_type_t map[MAP_Y][MAP_X];
    uint8_t height[MAP_Y][MAP_X];
    uint8_t n, s, e, w;
} map_t;

int new_map(map_t *m, uint8_t n, uint8_t s, uint8_t e, uint8_t w, int poke_place_prob);
void print_map(map_t *m);

#endif