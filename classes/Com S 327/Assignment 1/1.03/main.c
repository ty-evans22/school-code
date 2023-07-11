#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "mapGen.h"
#include "worldGen.h"
#include "heap.h"

#include <limits.h>

/* Store the trainer types as enums to easily add more types later */
typedef enum trainer_type {
    tr_hiker,
    tr_rival
} trainer_type_t;

typedef struct point {
    heap_node_t *hn;
    uint8_t pos[2];
    int32_t cost;
    int32_t dist;
} point_t;

// may need to be static
int32_t dij_cmp(const void *key, const void *with) {
    return ((point_t *) key)->dist - ((point_t *) with)->dist;
}

/* Cost arrays for each type of NPC - order is based on terrain_type enum */
static int hiker_cost[10] = {INT_MAX, INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 15, 10,      15,      15};
static int rival_cost[10] = {INT_MAX, INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX};

void dijkstra(map_t *m, point_t *cells[MAP_Y][MAP_X], player_t *pc, trainer_type_t tr) {
    uint32_t x, y;

    /* Initialize the heap */
    heap_t h;
    heap_init(&h, dij_cmp, NULL); // might need comparison based on dist instead of cost

    /* Initialize the array of cells */
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            cells[y][x] = malloc(sizeof(*cells[y][x])); // hopefully this properly allocates the memory
            // may need to put the following in separate loop for proper timing
            cells[y][x]->pos[dim_y] = y;
            cells[y][x]->pos[dim_x] = x;
            /* If current cell does not equal the player's cell */
            if (x != pc->pos[dim_x] || y != pc->pos[dim_y]) {
                cells[y][x]->dist = INT_MAX; // unkown distance from the player to this cell
                /* Determine cell cost based on trainer and terrain type */
                if (tr == tr_hiker) {
                    cells[y][x]->cost = hiker_cost[m->map[y][x]];
                } else if (tr == tr_rival) {
                    cells[y][x]->cost = rival_cost[m->map[y][x]];
                }
            } else {
                cells[y][x]->dist = 0;
                cells[y][x]->cost = 0;
            }
            /* Fill the heap */
            // cells[y][x]->hn = NULL;
            cells[y][x]->hn = heap_insert(&h, cells[y][x]);
        }
    }

    /* Main Loop */
    point_t *p;
    int count = 0;
    uint32_t alt;
    /* While heap is not empty */
    while ((p = heap_remove_min(&h))) {
        p->hn = NULL;
        count++;

        /* Go through each neighbor of p that is still in the heap */
        // top left
        if (p->pos[dim_y] > 0 && p->pos[dim_x] > 0 && (cells[p->pos[dim_y] - 1][p->pos[dim_x] - 1]->hn)) {
            alt = p->dist + cells[p->pos[dim_y] - 1][p->pos[dim_x] - 1]->cost;
            if (alt < cells[p->pos[dim_y] - 1][p->pos[dim_x] - 1]->dist) {
                cells[p->pos[dim_y] - 1][p->pos[dim_x] - 1]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y] - 1][p->pos[dim_x] - 1]->hn);
            }
        }

        // top
        if (p->pos[dim_y] > 0 && (cells[p->pos[dim_y] - 1][p->pos[dim_x]    ]->hn)) {
            alt = p->dist + cells[p->pos[dim_y] - 1][p->pos[dim_x]    ]->cost;
            if (alt < cells[p->pos[dim_y] - 1][p->pos[dim_x]    ]->dist) {
                cells[p->pos[dim_y] - 1][p->pos[dim_x]    ]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y] - 1][p->pos[dim_x]    ]->hn);
            }
        }

        // top right
        if (p->pos[dim_y] > 0 && p->pos[dim_x] < MAP_X - 1 && (cells[p->pos[dim_y] - 1][p->pos[dim_x] + 1]->hn)) {
            alt = p->dist + cells[p->pos[dim_y] - 1][p->pos[dim_x] + 1]->cost;
            if (alt < cells[p->pos[dim_y] - 1][p->pos[dim_x] + 1]->dist) {
                cells[p->pos[dim_y] - 1][p->pos[dim_x] + 1]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y] - 1][p->pos[dim_x] + 1]->hn);
            }
        }

        // left
        if (p->pos[dim_x] > 0 && (cells[p->pos[dim_y]    ][p->pos[dim_x] - 1]->hn)) {
            alt = p->dist + cells[p->pos[dim_y]    ][p->pos[dim_x] - 1]->cost;
            if (alt < cells[p->pos[dim_y]    ][p->pos[dim_x] - 1]->dist) {
                cells[p->pos[dim_y]    ][p->pos[dim_x] - 1]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y]    ][p->pos[dim_x] - 1]->hn);
            }
        }

        // right
        if (p->pos[dim_x] < MAP_X - 1 && (cells[p->pos[dim_y]    ][p->pos[dim_x] + 1]->hn)) {
            alt = p->dist + cells[p->pos[dim_y]    ][p->pos[dim_x] + 1]->cost;
            if (alt < cells[p->pos[dim_y]    ][p->pos[dim_x] + 1]->dist) {
                cells[p->pos[dim_y]    ][p->pos[dim_x] + 1]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y]    ][p->pos[dim_x] + 1]->hn);
            }
        }

        // bottom left
        if (p->pos[dim_y] < MAP_Y - 1 && p->pos[dim_x] > 0 && (cells[p->pos[dim_y] + 1][p->pos[dim_x] - 1]->hn)) {
            alt = p->dist + cells[p->pos[dim_y] + 1][p->pos[dim_x] - 1]->cost;
            if (alt < cells[p->pos[dim_y] + 1][p->pos[dim_x] - 1]->dist) {
                cells[p->pos[dim_y] + 1][p->pos[dim_x] - 1]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y] + 1][p->pos[dim_x] - 1]->hn);
            }
        }

        // bottom
        if (p->pos[dim_y] < MAP_Y - 1 && (cells[p->pos[dim_y] + 1][p->pos[dim_x]    ]->hn)) {
            alt = p->dist + cells[p->pos[dim_y] + 1][p->pos[dim_x]    ]->cost;
            if (alt < cells[p->pos[dim_y] + 1][p->pos[dim_x]    ]->dist) {
                cells[p->pos[dim_y] + 1][p->pos[dim_x]    ]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y] + 1][p->pos[dim_x]    ]->hn);
            }
        }

        // bottom right
        if (p->pos[dim_y] < MAP_Y - 1 && p->pos[dim_x] < MAP_X - 1 && (cells[p->pos[dim_y] + 1][p->pos[dim_x] + 1]->hn)) {
            alt = p->dist + cells[p->pos[dim_y] + 1][p->pos[dim_x] + 1]->cost;
            if (alt < cells[p->pos[dim_y] + 1][p->pos[dim_x] + 1]->dist) {
                cells[p->pos[dim_y] + 1][p->pos[dim_x] + 1]->dist = alt;
                // prev[^^] = p;
                heap_decrease_key_no_replace(&h, cells[p->pos[dim_y] + 1][p->pos[dim_x] + 1]->hn);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    struct timeval tv;
    uint32_t seed;

    if (argc == 2) {
        seed = atoi(argv[1]);
    } else {
        gettimeofday(&tv, NULL);
        seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
    }

    printf("Using seed: %u\n", seed);
    srand(seed);

    /* Generate the initial map (center) */
    int curX = 199;
    int curY = 199;
    map_t *w[WORLD_Y][WORLD_X];
    new_world(w);

    /* Generate the player character */
    player_t *p = malloc(sizeof (*p));
    p->pos[dim_x] = -1; 
    p->pos[dim_y] = -1; // initialize player with coordinates of (-1, -1) for error checking
    place_player(w[curY][curX], p);

    /* Print the initial map */
    print_curr_map(w, curX, curY, p);

    /* Print the hiker distance map */
    point_t *cells[MAP_Y][MAP_X];
    dijkstra(w[curY][curX], cells, p, tr_hiker);

    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            if (cells[y][x]->cost == INT_MAX) {
                printf("   ");
            } else {
                printf("%02d ", cells[y][x]->dist % 100);
            }
        }
        printf("\n");
    }

    printf("\n ");

    /* Print the rival distance map */
    point_t *cells2[MAP_Y][MAP_X];
    dijkstra(w[curY][curX], cells2, p, tr_rival);

    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            if (cells2[y][x]->cost == INT_MAX) {
                printf("   ");
            } else {
                printf("%02d ", cells2[y][x]->dist % 100);
            }
        }
        printf("\n");
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