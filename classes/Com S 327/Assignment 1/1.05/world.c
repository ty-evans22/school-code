#include <stdio.h>
#include <stdlib.h>
#include <curses.h>

#include "world.h"
#include "mapGen.h"

world_t world;

char tr_representation[] = {'h', 'r', 'p', 'w', 's', 'n'};

void update_curr_map() {
    world.curr_map = world.world[world.curr_index[dim_y]][world.curr_index[dim_x]];
}

/* Initialize the world to the default starting values */
void init_world() {
    world.curr_index[dim_x] = world.curr_index[dim_y] = WORLD_SIZE / 2;

    /* Initialize all maps other than starting map to NULL */
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            if (x != world.curr_index[dim_x] || y != world.curr_index[dim_y]) {
                world.world[y][x] = NULL;
            } else {
                new_map();
            }
        }
    }
}

/* Run at the end of the program to free the memory that was allocated for the world */
void delete_world() {
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            if (world.world[y][x]) {
                free(world.world[y][x]);
                world.world[y][x] = NULL;
            }
        }
    }
}

/* Functions for moving around the world -- more efficient version exists in professor's code */
// int manhattan_distance(pair_t pos) {
//     /* Calculate the distance from the center (200, 200) */
//     return abs(pos[dim_x] - (WORLD_SIZE / 2)) + abs(pos[dim_y] - (WORLD_SIZE / 2));
// }

// /* Function to go to the specified map */
// int fly_to_position(pair_t pos) {
//     // error checking
//     if (pos[dim_x] < 0 || pos[dim_x] > WORLD_SIZE - 1 || pos[dim_y] < 0 || pos[dim_y] > WORLD_SIZE - 1) {
//         printf("ERROR: Specified coordinates are not within the world bounds (%d x %d)\n", WORLD_SIZE, WORLD_SIZE);
//         return 1;
//     }

//     // check if the specified map exists
//     if (world.world[pos[dim_y]][pos[dim_x]] == NULL) {
//         world.world[pos[dim_y]][pos[dim_x]] = malloc(sizeof(*world.world[pos[dim_y]][pos[dim_x]]));
//         // generate new map and make sure exits match up (wrap exits on edges)
//         // check each map direction to see if it exists
//         int n, s, e, w;

//         //********* STILL NEEDS TO BE REFORMATTED TO NEW CODE *********//

//         if (y != 0 && world[y - 1][x] != NULL) {
//             n = world[y - 1][x]->s;
//         } else if (y == 0 && world[WORLD_Y - 1][x] != NULL) {
//             n = world[WORLD_Y - 1][x]->s;
//         } else {
//             n = 1 + rand() % (MAP_X - 2);
//         }
//         if (y != WORLD_Y - 1 && world[y + 1][x] != NULL) {
//             s = world[y + 1][x]->n;
//         } else if (y == WORLD_Y - 1 && world[0][x] != NULL) {
//             s = world[0][x]->n;
//         } else {
//             s = 1 + rand() % (MAP_X - 2);
//         }
//         if (x != 0 && world[y][x - 1] != NULL) {
//             w = world[y][x - 1]->e;
//         } else if (x == 0 && world[y][WORLD_X - 1] != NULL) {
//             w = world[y][WORLD_X - 1]->e;
//         } else {
//             w = 1 + rand() % (MAP_Y - 2);
//         }
//         if (x != WORLD_X - 1 && world[y][x + 1] != NULL) {
//             e = world[y][x + 1]->w;
//         } else if (x == WORLD_X - 1 && world[y][0] != NULL) {
//             e = world[y][0]->w;
//         } else {
//             e = 1 + rand() % (MAP_Y - 2);
//         }

//         int poke_place_prob = ((-45 * manhattan_distance(x, y)) / 200.0) + 50;
//         // minimum probability is 5%
//         if (manhattan_distance(x, y) > 200) {
//             poke_place_prob = 5;
//         }
//         printf("\nPoke Place Prob: %d\n", poke_place_prob);
//         new_map(world[y][x], n, s, e, w, poke_place_prob);
//     }

//     return 0;
// }

// //********* STILL NEEDS TO BE REFORMATTED TO NEW CODE *********//

// // function to go to map in specified direction (just calculate x and y and use above function) - use enums to specify direction
// int move_in_direction(map_t *world[WORLD_Y][WORLD_X], direction_t direction, int curX, int curY) {
//     int nextX, nextY;
//     // make sure edge of map is not in specified direction
//     if (direction == north) {
//         if (curY == 0) {
//             // printf("ERROR: Cannot move north due to being at the northern edge of the world.");
//             // return 1;
//             nextX = curX;
//             nextY = WORLD_Y - 1;
//         } else {
//             nextX = curX;
//             nextY = curY - 1;
//         }
//     } else if (direction == south) {
//         if (curY == WORLD_Y - 1) {
//             // printf("ERROR: Cannot move south due to being at the southern edge of the world.");
//             // return 1;
//             nextX = curX;
//             nextY = 0;
//         } else {
//             nextX = curX;
//             nextY = curY + 1;
//         }
//     } else if (direction == west) {
//         if (curX == 0) {
//             // printf("ERROR: Cannot move west due to being at the western edge of the world.");
//             // return 1;
//             nextX = WORLD_X - 1;
//             nextY = curY;
//         } else {
//             nextX = curX - 1;
//             nextY = curY;
//         }
//     } else if (direction == east) {
//         if (curX == WORLD_X - 1) {
//             // printf("ERROR: Cannot move east due to being at the eastern edge of the world.");
//             // return 1;
//             nextX = 0;
//             nextY = curY;
//         } else {
//             nextX = curX + 1;
//             nextY = curY;
//         }
//     } else {
//         printf("ERROR: Specified direction is invalid.\n");
//         return 1;
//     }

//     return fly_to_position(world, nextX, nextY);
// }