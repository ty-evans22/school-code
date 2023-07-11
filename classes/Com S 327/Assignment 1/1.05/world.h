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

typedef int pair_t[num_dims];

typedef enum direction {
    northwest,
    west,
    southwest,
    north,
    south,
    northeast,
    east,
    southeast,
    center
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

typedef struct pc {
    direction_t dir;
    uint8_t poke_mart;
    uint8_t poke_center;
} pc_t;

typedef struct npc {
    trainer_type_t ttype;
    movement_type_t mtype;
    uint8_t defeated;
    pair_t dir;
} npc_t;

typedef struct trainer {
    npc_t *npc;
    pc_t *pc;
    pair_t pos;
    char symbol;
    int curr_time;
} trainer_t;

typedef struct map {
    terrain_type_t map[MAP_Y][MAP_X];
    uint8_t height[MAP_Y][MAP_X];
    trainer_t *tmap[MAP_Y][MAP_X];
    heap_t queue;                       /* priority queue for NPC movement */
    uint8_t n, s, e, w;                 /* coords of each exit */
} map_t;

typedef struct world {
    map_t *world[WORLD_SIZE][WORLD_SIZE];
    pair_t curr_index;
    map_t *curr_map;
    int hiker_dist[MAP_Y][MAP_X];       /* hiker distance map */
    int rival_dist[MAP_Y][MAP_X];       /* rival distance map */
    trainer_t player;
} world_t;

extern world_t world;

void init_world();
void delete_world();

/** Functions for moving around the world */
// int fly_to_position(pair_t pos);
// int move_in_direction(direction_t direction);

#endif