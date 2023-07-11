#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>
#include "trainer.h"
#include "heap.h"

#define MAP_X              80
#define MAP_Y              21
#define WORLD_SIZE         401

typedef enum dim {
    dim_x,
    dim_y,
    num_dims
} dim_t;

typedef uint8_t pair_t[num_dims];

typedef enum direction {
    north,
    south,
    east,
    west,
    northwest,
    northeast,
    southwest,
    southeast
} direction_t;

typedef enum __attribute__ ((__packed__)) terrain_type {
    ter_boulder,
    ter_tree,
    ter_path,
    ter_mart,
    ter_center,
    ter_grass,
    ter_clearing,
    ter_mountain,
    ter_forest,
    num_terrain_types
} terrain_type_t;

typedef struct trainer {
    uint8_t pos[2];
    trainer_type_t tr_type;
    terrain_type_t wanderer_ter_type;
    int curr_time;
    direction_t dir;
    heap_node_t *hn;
} trainer_t;

typedef struct map {
    terrain_type_t map[MAP_Y][MAP_X];
    uint8_t height[MAP_Y][MAP_X];
    uint8_t n, s, e, w;
} map_t;

typedef struct world {
    map_t *world[WORLD_SIZE][WORLD_SIZE];
    pair_t curr_index;
    map_t *curr_map;
    // trainers and distance maps will be moved into map_t once multiple maps are utilized
    uint8_t occupied[MAP_Y][MAP_X];
    uint16_t num_trainers;
    trainer_t *trainers;
    int hiker_dist[MAP_Y][MAP_X];
    int rival_dist[MAP_Y][MAP_X];
    trainer_t *player;
} world_t;

extern world_t world;

void init_world();
void init_trainers(uint16_t num_trainers);
void delete_world();
void print_curr_map();

/** Functions for moving around the world */
// int fly_to_position(pair_t pos);
// int move_in_direction(direction_t direction);

#endif