#ifndef WORLDGEN_H
#define WORLDGEN_H

#include <stdio.h>

#include "mapGen.h"

/* World Dimensions */
#define WORLD_Y 399
#define WORLD_X 399

typedef enum direction {
    north,
    south,
    east,
    west
} direction_t;

int fly_to_position(map_t *world[WORLD_Y][WORLD_X], int x, int y);
int move_in_direction(map_t *world[WORLD_Y][WORLD_X], direction_t direction, int curX, int curY);
void print_curr_map(map_t *world[WORLD_Y][WORLD_X], int x, int y);
int new_world(map_t *world[WORLD_Y][WORLD_X]);

#endif