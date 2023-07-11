#include <stdint.h>
#include <stddef.h>
#include <limits.h>

#include "dijkstra.h"
#include "heap.h"
#include "mapGen.h"

#define ter_cost(x, y, c) move_cost[c][m->map[y][x]]

typedef struct path {
    heap_node_t *hn;
    uint8_t pos[2];
    uint8_t from[2];
    int32_t cost;
} path_t;

/* Array of movement costs for each of the trainer types */
static int32_t move_cost[num_trainer_types][num_terrain_types] = {
    { INT_MAX, INT_MAX, 10, 10,      10,      20, 10, INT_MAX, INT_MAX },
    { INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 15, 10, 15,      15      },
    { INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX },
    { INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX },
};

static int32_t path_cmp(const void *key, const void *with) {
    return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t hiker_cmp(const void *key, const void *with) {
    return (world.hiker_dist[((path_t *) key)->pos[dim_y]]
                            [((path_t *) key)->pos[dim_x]] -
            world.hiker_dist[((path_t *) with)->pos[dim_y]]
                            [((path_t *) with)->pos[dim_x]]);
}

static int32_t rival_cmp(const void *key, const void *with) {
    return (world.rival_dist[((path_t *) key)->pos[dim_y]]
                            [((path_t *) key)->pos[dim_x]] -
            world.rival_dist[((path_t *) with)->pos[dim_y]]
                            [((path_t *) with)->pos[dim_x]]);
}

static int32_t edge_penalty(int8_t x, int8_t y) {
    return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

/* Generate the paths connecting the exits */
void dijkstra_path(map_t *m, pair_t from, pair_t to) {
    static path_t path[MAP_Y][MAP_X], *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y;

    /* Initialize the path array */
    if (!initialized) {
        for (y = 0; y < MAP_Y; y++) {
            for (x = 0; x < MAP_X; x++) {
                path[y][x].pos[dim_y] = y;
                path[y][x].pos[dim_x] = x;
            }
        }
        initialized = 1;
    }

    /* Max out the cost of every cell */
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            path[y][x].cost = INT_MAX;
        }
    }

    /* Set the starting point's cost to 0 */
    path[from[dim_y]][from[dim_x]].cost = 0;

    /* Initialize the min-heap */
    heap_init(&h, path_cmp, NULL);

    /* Create a heap node for each point */
    for (y = 1; y < MAP_Y - 1; y++) {
        for (x = 1; x < MAP_X - 1; x++) {
            path[y][x].hn = heap_insert(&h, &path[y][x]);
        }
    }

    /* Loop through the heap until it is empty */
    while ((p = heap_remove_min(&h))) {
        p->hn = NULL;

        /* Check if the path is complete */
        if ((p->pos[dim_y] == to[dim_y]) && p->pos[dim_x] == to[dim_x]) {
            for (x = to[dim_x], y = to[dim_y];
                    (x != from[dim_x]) || (y != from[dim_y]);
                    p = &path[y][x], x = p->from[dim_x], y = p->from[dim_y]) {
                mapxy(x, y) = ter_path;
                heightxy(x, y) = 0;
            }
            heap_delete(&h);
            return;
        }

        /* Check each neighboring cell */

        if ((path[p->pos[dim_y] - 1][p->pos[dim_x]    ].hn) &&
            (path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost >
             ((p->cost + heightpair(p->pos)) *
              edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1)))) {
            path[p->pos[dim_y] - 1][p->pos[dim_x]    ].cost =
                ((p->cost + heightpair(p->pos)) *
                 edge_penalty(p->pos[dim_x], p->pos[dim_y] - 1));
            path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] - 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1]
                                                 [p->pos[dim_x]    ].hn);
        }
        if ((path[p->pos[dim_y]    ][p->pos[dim_x] - 1].hn) &&
            (path[p->pos[dim_y]    ][p->pos[dim_x] - 1].cost >
             ((p->cost + heightpair(p->pos)) *
              edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y])))) {
            path[p->pos[dim_y]][p->pos[dim_x] - 1].cost =
                ((p->cost + heightpair(p->pos)) *
                 edge_penalty(p->pos[dim_x] - 1, p->pos[dim_y]));
            path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y]    ][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                                 [p->pos[dim_x] - 1].hn);
        }
        if ((path[p->pos[dim_y]    ][p->pos[dim_x] + 1].hn) &&
            (path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost >
             ((p->cost + heightpair(p->pos)) *
              edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y])))) {
            path[p->pos[dim_y]    ][p->pos[dim_x] + 1].cost =
                ((p->cost + heightpair(p->pos)) *
                 edge_penalty(p->pos[dim_x] + 1, p->pos[dim_y]));
            path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y]    ][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]    ]
                                                 [p->pos[dim_x] + 1].hn);
        }
        if ((path[p->pos[dim_y] + 1][p->pos[dim_x]    ].hn) &&
            (path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost >
             ((p->cost + heightpair(p->pos)) *
              edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1)))) {
            path[p->pos[dim_y] + 1][p->pos[dim_x]    ].cost =
                ((p->cost + heightpair(p->pos)) *
                 edge_penalty(p->pos[dim_x], p->pos[dim_y] + 1));
            path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] + 1][p->pos[dim_x]    ].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1]
                                                 [p->pos[dim_x]    ].hn);
        }
    }
}

/* Generate the hiker and rival distance maps */
void pathfind(map_t *m) {
    heap_t h;
    uint32_t x, y;
    static path_t p[MAP_Y][MAP_X], *c;
    static uint32_t initialized = 0;

    /* Initialize the path array */
    if (!initialized) {
        initialized = 1;
        for (y = 0; y < MAP_Y; y++) {
            for (x = 0; x < MAP_X; x++) {
                p[y][x].pos[dim_y] = y;
                p[y][x].pos[dim_x] = x;
            }
        }
    }

    /* Max out the cost of every cell */
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            world.hiker_dist[y][x] = world.rival_dist[y][x] = INT_MAX;
        }
    }

    /* Set the starting point's cost to 0 */
    world.hiker_dist[world.player.pos[dim_y]][world.player.pos[dim_x]] = 
        world.rival_dist[world.player.pos[dim_y]][world.player.pos[dim_x]] = 0;

    /* Initialize the hiker min-heap */
    heap_init(&h, hiker_cmp, NULL);

    /* Create a heap node for each point */
    for (y = 1; y < MAP_Y - 1; y++) {
        for (x = 1; x < MAP_X - 1; x++) {
            if (ter_cost(x, y, tr_hiker) != INT_MAX) {
                p[y][x].hn = heap_insert(&h, &p[y][x]);
            } else {
                p[y][x].hn = NULL;
            }
        }
    }

    /* Loop through the heap until it is empty */
    while ((c = heap_remove_min(&h))) {
        c->hn = NULL;

        /* Check the cost of each neighboring node */

        if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) && (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) && (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
        }
        if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) && (world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
        }
        if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) && (world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) && (world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) && (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) && (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) && (world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
             world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker))) {
            world.hiker_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] = world.hiker_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_hiker);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
        }
    }
    heap_delete(&h);

    /* Initialize the rival min-heap */
    heap_init(&h, rival_cmp, NULL);

    /* Create a heap node for each point */
    for (y = 1; y < MAP_Y - 1; y++) {
        for (x = 1; x < MAP_X - 1; x++) {
            if (ter_cost(x, y, tr_rival) != INT_MAX) {
                p[y][x].hn = heap_insert(&h, &p[y][x]);
            } else {
                p[y][x].hn = NULL;
            }
        }
    }

    /* Loop through the heap until it is empty */
    while ((c = heap_remove_min(&h))) {
        c->hn = NULL;

        /* Check the cost of each neighboring node */

        if ((p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn) && (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] - 1] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn) && (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x]    ] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x]    ].hn);
        }
        if ((p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn) && (world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y] - 1][c->pos[dim_x] + 1] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] - 1][c->pos[dim_x] + 1].hn);
        }
        if ((p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn) && (world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] - 1] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y]    ][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn) && (world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y]    ][c->pos[dim_x] + 1] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y]    ][c->pos[dim_x] + 1].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn) && (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] - 1] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x] - 1].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn) && (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x]    ] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x]    ].hn);
        }
        if ((p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn) && (world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] >
             world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] + ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival))) {
            world.rival_dist[c->pos[dim_y] + 1][c->pos[dim_x] + 1] = world.rival_dist[c->pos[dim_y]][c->pos[dim_x]] +
                ter_cost(c->pos[dim_x], c->pos[dim_y], tr_rival);
            heap_decrease_key_no_replace(&h, p[c->pos[dim_y] + 1][c->pos[dim_x] + 1].hn);
        }
    }
    heap_delete(&h);
}