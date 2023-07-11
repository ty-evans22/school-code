#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mapGen.h"
#include "heap.h"
#include "worldGen.h"

int manhattan_distance(int x, int y) {
    /* Calculate the distance from the center (199, 199) */
    int xVal = abs(x - 199);
    int yVal = abs(y - 199);
    return xVal + yVal;
}

/* Function to go to the specified map */
int fly_to_position(map_t *world[WORLD_Y][WORLD_X], int x, int y) {
    // error checking
    if (x < 0 || x > WORLD_X - 1 || y < 0 || y > WORLD_Y - 1) {
        printf("ERROR: Specified coordinates are not within the world bounds (399 x 399)\n");
        return 1;
    }

    // check if specified map exists
    if (world[y][x] == NULL) {
        world[y][x] = malloc(sizeof(*world[y][x]));
        // generate new map and make sure exits match up (wrap exits on edges)
        // check each map direction to see if it exists
        int n, s, e, w;
        if (y != 0 && world[y - 1][x] != NULL) {
            n = world[y - 1][x]->s;
        } else if (y == 0 && world[WORLD_Y - 1][x] != NULL) {
            n = world[WORLD_Y - 1][x]->s;
        } else {
            n = 1 + rand() % (MAP_X - 2);
        }
        if (y != WORLD_Y - 1 && world[y + 1][x] != NULL) {
            s = world[y + 1][x]->n;
        } else if (y == WORLD_Y - 1 && world[0][x] != NULL) {
            s = world[0][x]->n;
        } else {
            s = 1 + rand() % (MAP_X - 2);
        }
        if (x != 0 && world[y][x - 1] != NULL) {
            w = world[y][x - 1]->e;
        } else if (x == 0 && world[y][WORLD_X - 1] != NULL) {
            w = world[y][WORLD_X - 1]->e;
        } else {
            w = 1 + rand() % (MAP_Y - 2);
        }
        if (x != WORLD_X - 1 && world[y][x + 1] != NULL) {
            e = world[y][x + 1]->w;
        } else if (x == WORLD_X - 1 && world[y][0] != NULL) {
            e = world[y][0]->w;
        } else {
            e = 1 + rand() % (MAP_Y - 2);
        }

        int poke_place_prob = ((-45 * manhattan_distance(x, y)) / 200.0) + 50;
        // minimum probability is 5%
        if (manhattan_distance(x, y) > 200) {
            poke_place_prob = 5;
        }
        printf("\nPoke Place Prob: %d\n", poke_place_prob);
        new_map(world[y][x], n, s, e, w, poke_place_prob);
    }

    return 0;
}

// function to go to map in specified direction (just calculate x and y and use above function) - use enums to specify direction
int move_in_direction(map_t *world[WORLD_Y][WORLD_X], direction_t direction, int curX, int curY) {
    int nextX, nextY;
    // make sure edge of map is not in specified direction
    if (direction == north) {
        if (curY == 0) {
            // printf("ERROR: Cannot move north due to being at the northern edge of the world.");
            // return 1;
            nextX = curX;
            nextY = WORLD_Y - 1;
        } else {
            nextX = curX;
            nextY = curY - 1;
        }
    } else if (direction == south) {
        if (curY == WORLD_Y - 1) {
            // printf("ERROR: Cannot move south due to being at the southern edge of the world.");
            // return 1;
            nextX = curX;
            nextY = 0;
        } else {
            nextX = curX;
            nextY = curY + 1;
        }
    } else if (direction == west) {
        if (curX == 0) {
            // printf("ERROR: Cannot move west due to being at the western edge of the world.");
            // return 1;
            nextX = WORLD_X - 1;
            nextY = curY;
        } else {
            nextX = curX - 1;
            nextY = curY;
        }
    } else if (direction == east) {
        if (curX == WORLD_X - 1) {
            // printf("ERROR: Cannot move east due to being at the eastern edge of the world.");
            // return 1;
            nextX = 0;
            nextY = curY;
        } else {
            nextX = curX + 1;
            nextY = curY;
        }
    } else {
        printf("ERROR: Specified direction is invalid.\n");
        return 1;
    }

    return fly_to_position(world, nextX, nextY);
}

void print_curr_map(map_t *world[WORLD_Y][WORLD_X], int x, int y) {
    print_map(world[y][x]);
    
    // display coordinates
    printf("Current Map Coordinates: X = %d, Y = %d\n", x - 199, y - 199);
}

int new_world(map_t *world[WORLD_Y][WORLD_X]) {
    for (int y = 0; y < WORLD_Y; y++) {
        for (int x = 0; x < WORLD_X; x++) {
            if (x != 199 || y != 199) {
                // set values of all maps to NULL
                world[y][x] = NULL;
            } else {
                world[y][x] = malloc(sizeof(*world[y][x]));
                new_map(world[y][x], 1 + rand() % (MAP_X - 2), 1 + rand() % (MAP_X - 2),
                                        1 + rand() % (MAP_Y - 2), 1 + rand() % (MAP_Y - 2), 100);
            }
        }
    }
    
    return 0;
}