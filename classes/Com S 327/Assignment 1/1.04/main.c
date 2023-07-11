#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#include "world.h"
#include "trainer.h"
#include "dijkstra.h"
#include "heap.h"

#define BASE_COST 15

int direction_vals[8][2] = {
    { 0, -1},
    { 0,  1},
    { 1,  0},
    {-1,  0},
    {-1, -1},
    { 1, -1},
    {-1,  1},
    { 1,  1}
};

/* Used for easy representation of the opposite direction */
int opp_direction_vals[] = {1, 0, 3, 2, 7, 6, 5, 4};

/* Array of movement costs for each of the trainer types */
static int32_t move_cost[num_trainer_types][num_terrain_types] = {
    { INT_MAX, INT_MAX, 10     , 10,      10,      20, 10, INT_MAX, INT_MAX }, // player
    { INT_MAX, INT_MAX, 10     , INT_MAX, INT_MAX, 15, 10, 15,      15      }, // hiker
    { INT_MAX, INT_MAX, 10     , INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX }, // rival
    { INT_MAX, INT_MAX, 10     , INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX }, // pacer
    { INT_MAX, INT_MAX, INT_MAX, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX }, // wanderer
    { INT_MAX, INT_MAX, 10     , INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX }, // stationary
    { INT_MAX, INT_MAX, 10     , INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX }  // random walker
};

static int32_t trainer_cmp(const void *key, const void *with) {
    return ((trainer_t *) key)->curr_time - ((trainer_t *) with)->curr_time;
}

int main(int argc, char *argv[]) {

    struct timeval tv;
    uint32_t seed = -1;
    uint16_t num_trainers = DEFAULT_NUM_TRAINERS;

    /* Process command line arguments */
    if (argc > 1) {
        // error checking
        if (argc <= 4) {
            // argv[1] is either seed or --numtrainers
            for (int i = 1; i < argc; i++) {
                int result = strcmp(argv[i], "--numtrainers");
                if (result == 0) {
                    // next arg is numtrainers - make sure it is valid
                    if (argc == i + 1) {
                        printf("\nERROR: --numtrainers switch must be followed by an integer value for the number of trainers to spawn.");
                    } else {
                        num_trainers = atoi(argv[i + 1]);
                        // error checking
                        if (num_trainers == 0) {
                            printf("\nERROR: --numtrainers switch value invalid. Proceeding with the default value.");
                            num_trainers = DEFAULT_NUM_TRAINERS;
                        } else if (num_trainers > MAX_NUM_TRAINERS) {
                            printf("\nERROR: --numtrainers switch value too large. Must be less than %d. Proceeding with max allowed value.", MAX_NUM_TRAINERS);
                            num_trainers = MAX_NUM_TRAINERS;
                        }
                        break;
                    }
                } else {
                    seed = atoi(argv[i]);
                    // error checking
                    if (seed == 0) {
                        printf("\nERROR: Value of seed must be an integer. Proceeding with random seed.");
                        seed = -1;
                    }
                }
            }
        } else {
            printf("\nERROR: Too many command line arguments. Max number is 3 (seed --numtrainers num_trainers). Proceeding with random seed and default number of trainers.");
        }
    }

    /* Make sure seed is set */
    if (seed == -1) {
        gettimeofday(&tv, NULL);
        seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
    }

    printf("\nUsing seed: %u\n", seed);
    srand(seed);

    /* Initialize the world */
    init_world();
    init_trainers(num_trainers);

    /* Generate the distance maps */
    pathfind(world.curr_map);

    /* Print the initial map and distance maps */
    print_curr_map();
    // print_hiker_dist();
    // print_rival_dist();

    /* Start priority queue */
    heap_t h;
    trainer_t *t;
    int redraw_flag = 0;
    heap_init(&h, trainer_cmp, NULL); // initializes the min-heap

    /* Create a heap node for every trainer */
    for (int i = 0; i < world.num_trainers; i++) {
        world.trainers[i].hn = heap_insert(&h, &world.trainers[i]);
    }

    /* Add the player to the heap */
    world.player->hn = heap_insert(&h, world.player);

    /* Keep looping through the heap until the user quits with CTRL-C */
    while (1) {
        /* Get the next trainer to move */
        t = heap_remove_min(&h);
        t->hn = NULL;

        /* Determine the trainer's next move based on their trainer type */

        pair_t new_pos = {t->pos[dim_x], t->pos[dim_y]};
        int cost = INT_MAX;

        if (t->tr_type == tr_hiker || t->tr_type == tr_rival) {
            int lowest_distance = INT_MAX;
            for (int y = -1; y <= 1; y++) {
                for (int x = -1; x <= 1; x++) {
                    // skip if current position
                    if (x == 0 && y == 0) {
                        continue;
                    }
                    if (t->pos[dim_x] + x > 0 && t->pos[dim_x] + x < MAP_X - 1 && t->pos[dim_y] + y > 0 && t->pos[dim_y] + y < MAP_Y - 1
                         && world.occupied[t->pos[dim_y] + y][t->pos[dim_x] + x] == 0) {
                        if (t->tr_type == tr_hiker && world.hiker_dist[t->pos[dim_y] + y][t->pos[dim_x] + x] < lowest_distance) {
                            lowest_distance = world.hiker_dist[t->pos[dim_y] + y][t->pos[dim_x] + x];
                            cost = move_cost[tr_hiker][world.curr_map->map[t->pos[dim_y] + y][t->pos[dim_x] + x]];
                            new_pos[dim_x] = t->pos[dim_x] + x;
                            new_pos[dim_y] = t->pos[dim_y] + y;
                        } else if (t->tr_type == tr_rival && world.rival_dist[t->pos[dim_y] + y][t->pos[dim_x] + x] < lowest_distance) {
                            lowest_distance = world.rival_dist[t->pos[dim_y] + y][t->pos[dim_x] + x];
                            cost = move_cost[tr_rival][world.curr_map->map[t->pos[dim_y] + y][t->pos[dim_x] + x]];
                            new_pos[dim_x] = t->pos[dim_x] + x;
                            new_pos[dim_y] = t->pos[dim_y] + y;
                        }
                    }
                }
            }
        } else if (t->tr_type == tr_pacer) {
            int tmp_x = t->pos[dim_x] + direction_vals[t->dir][dim_x];
            int tmp_y = t->pos[dim_y] + direction_vals[t->dir][dim_y];
            // check if the given position is valid
            if (tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && world.occupied[tmp_y][tmp_x] == 0 &&
                 move_cost[tr_pacer][world.curr_map->map[tmp_y][tmp_x]] < cost) {
                cost = move_cost[tr_pacer][world.curr_map->map[tmp_y][tmp_x]];
                new_pos[dim_x] = tmp_x;
                new_pos[dim_y] = tmp_y;
            } else {
                // change directions
                t->dir = (direction_t)(opp_direction_vals[t->dir]);
                // make sure next cell in new direction is valid
                tmp_x = t->pos[dim_x] + direction_vals[t->dir][dim_x];
                tmp_y = t->pos[dim_y] + direction_vals[t->dir][dim_y];
                if (tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && world.occupied[tmp_y][tmp_x] == 0 &&
                     move_cost[tr_pacer][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX) {
                    cost = move_cost[tr_pacer][world.curr_map->map[tmp_y][tmp_x]];
                    new_pos[dim_x] = tmp_x;
                    new_pos[dim_y] = tmp_y;
                }
            }
        } else if (t->tr_type == tr_stationary) {
            cost = BASE_COST;
        } else if (t->tr_type == tr_wanderer) {
            // check next cell in headed direction and move there if valid, otherwise move in random different direction
            int tmp_x = t->pos[dim_x] + direction_vals[t->dir][dim_x];
            int tmp_y = t->pos[dim_y] + direction_vals[t->dir][dim_y];
            // check if given position is valid
            if (tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && world.occupied[tmp_y][tmp_x] == 0 &&
                 move_cost[tr_wanderer][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX && world.curr_map->map[tmp_y][tmp_x] == t->wanderer_ter_type) {
                cost = move_cost[tr_wanderer][world.curr_map->map[tmp_y][tmp_x]];
                new_pos[dim_x] = tmp_x;
                new_pos[dim_y] = tmp_y;
            } else {
                // change direction randomly
                // very messy implementation here but its temporary
                int visited[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                visited[t->dir] = 1;
                int count = 1;
                do {
                    t->dir = (direction_t)(rand() % 8);
                    if (visited[t->dir] == 0) {
                        visited[t->dir] = 1;
                        count++;
                        tmp_x = t->pos[dim_x] + direction_vals[t->dir][dim_x];
                        tmp_y = t->pos[dim_y] + direction_vals[t->dir][dim_y];
                    }
                } while (count < 8 && !(tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && 
                    world.occupied[tmp_y][tmp_x] == 0 && move_cost[tr_wanderer][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX
                    && world.curr_map->map[tmp_y][tmp_x] == t->wanderer_ter_type));

                // test one last time
                if (tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && 
                     world.occupied[tmp_y][tmp_x] == 0 && move_cost[tr_wanderer][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX
                     && world.curr_map->map[tmp_y][tmp_x] == t->wanderer_ter_type) {
                    cost = move_cost[tr_wanderer][world.curr_map->map[tmp_y][tmp_x]];
                    new_pos[dim_x] = tmp_x;
                    new_pos[dim_y] = tmp_y;
                }
            }
        } else if (t->tr_type == tr_rand_walker) {
            // check next cell in headed direction and move there if valid, otherwise move in random different direction
            int tmp_x = t->pos[dim_x] + direction_vals[t->dir][dim_x];
            int tmp_y = t->pos[dim_y] + direction_vals[t->dir][dim_y];
            // check if given position is valid
            if (tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && world.occupied[tmp_y][tmp_x] == 0 &&
                 move_cost[tr_rand_walker][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX) {
                cost = move_cost[tr_rand_walker][world.curr_map->map[tmp_y][tmp_x]];
                new_pos[dim_x] = tmp_x;
                new_pos[dim_y] = tmp_y;
            } else {
                // change direction randomly
                // very messy implementation here but its temporary
                int visited[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                visited[t->dir] = 1;
                int count = 1;
                do {
                    t->dir = (direction_t)(rand() % 8);
                    if (visited[t->dir] == 0) {
                        visited[t->dir] = 1;
                        count++;
                        tmp_x = t->pos[dim_x] + direction_vals[t->dir][dim_x];
                        tmp_y = t->pos[dim_y] + direction_vals[t->dir][dim_y];
                    }
                } while (count < 8 && !(tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && 
                    world.occupied[tmp_y][tmp_x] == 0 && move_cost[tr_rand_walker][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX));
                // test one last time
                if (tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && 
                     world.occupied[tmp_y][tmp_x] == 0 && move_cost[tr_rand_walker][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX) {
                    cost = move_cost[tr_rand_walker][world.curr_map->map[tmp_y][tmp_x]];
                    new_pos[dim_x] = tmp_x;
                    new_pos[dim_y] = tmp_y;
                }
            }
        } else {
            // player so pick a random adjacent valid cell and move there - also set redraw_flag
            redraw_flag = 1;
            int tmp_x, tmp_y;
            int visited[8] = {0, 0, 0, 0, 0, 0, 0, 0};
            int count = 0;
            do {
                t->dir = (direction_t)(rand() % 8);
                if (!visited[t->dir]) {
                    visited[t->dir] = 1;
                    count++;
                    tmp_x = t->pos[dim_x] + direction_vals[t->dir][dim_x];
                    tmp_y = t->pos[dim_y] + direction_vals[t->dir][dim_y];
                }
            } while (count < 8 && !(tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && 
                world.occupied[tmp_y][tmp_x] == 0 && move_cost[tr_player][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX));
            // test one last time
            if (tmp_x > 0 && tmp_x < MAP_X - 1 && tmp_y > 0 && tmp_y < MAP_Y - 1 && 
                world.occupied[tmp_y][tmp_x] == 0 && move_cost[tr_player][world.curr_map->map[tmp_y][tmp_x]] < INT_MAX) {
                cost = move_cost[tr_player][world.curr_map->map[tmp_y][tmp_x]];
                new_pos[dim_x] = tmp_x;
                new_pos[dim_y] = tmp_y;
            }
        }

        /* If no valid movements were found then stay at the same position and add the base cost */
        if (cost == INT_MAX) {
            cost = BASE_COST;
            new_pos[dim_x] = t->pos[dim_x];
            new_pos[dim_y] = t->pos[dim_y];
        }

        /* Update curr_time and position */
        t->curr_time += cost;
        world.occupied[t->pos[dim_y]][t->pos[dim_x]] = 0;
        t->pos[dim_y] = new_pos[dim_y];
        t->pos[dim_x] = new_pos[dim_x];
        world.occupied[t->pos[dim_y]][t->pos[dim_x]] = 1;

        /* Add the trainer back to the heap */
        t->hn = heap_insert(&h, t);

        /* Redraw the map if necessary */
        if (redraw_flag == 1) {
            print_curr_map();
            pathfind(world.curr_map);
            /* Pause so the user can see the changes */
            usleep(250000);
            redraw_flag = 0;
        }
    }

    // /* Keeps taking input from the user until the user quits */
    // int quit = 0;
    // while (quit == 0) {
    //     char input[9];
    //     scanf("%s", input);
    //     char c = input[0];
    //     int x, y;
    //     switch (c) {
    //         case 'n':
    //             if (move_in_direction(w, north, curX, curY) == 0) {
    //                 curY--;
    //                 print_curr_map(w, curX, curY);
    //             }
    //             break;
    //         case 's':
    //             if (move_in_direction(w, south, curX, curY) == 0) {
    //                 curY++;
    //                 print_curr_map(w, curX, curY);
    //             }
    //             break;
    //         case 'e':
    //             if (move_in_direction(w, east, curX, curY) == 0) {
    //                 curX++;
    //                 print_curr_map(w, curX, curY);
    //             }
    //             break;
    //         case 'w':
    //             if (move_in_direction(w, west, curX, curY) == 0) {
    //                 curX--;
    //                 print_curr_map(w, curX, curY);
    //             }
    //             break;
    //         case 'q':
    //             quit = 1;
    //             break;
    //         case 'f':
    //             scanf("%d", &x);
    //             scanf("%d", &y);
    //             x += 199;
    //             y += 199;
    //             if (fly_to_position(w, x, y) == 0) {
    //                 curX = x;
    //                 curY = y;
    //                 print_curr_map(w, curX, curY);
    //             }
    //             break;
    //         default:
    //             printf("ERROR: Invalid input.");
    //     }
    // }

    return 0;
}