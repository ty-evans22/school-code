#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <endian.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <assert.h>

#include "mapGen.h"

#define malloc(size) ({            \
    void *_tmp;                    \
    assert((_tmp = malloc(size))); \
    _tmp;                          \
})

#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95

#define mappair(pair) (m->map[pair[dim_y]][pair[dim_x]])
#define mapxy(x, y) (m->map[y][x])
#define heightpair(pair) (m->height[pair[dim_y]][pair[dim_x]])
#define heightxy(x, y) (m->height[y][x])

typedef struct queue_node {
    int x, y;
    struct queue_node *next;
} queue_node_t;

// switch back to static?
int32_t path_cmp(const void *key, const void *with) {
    return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

// returns 2 if point is on an edge (inside border) and 1 otherwise
static int32_t edge_penalty(uint8_t x, uint8_t y) {
    return (x == 1 || y == 1 || x == MAP_X - 2 || y == MAP_Y - 2) ? 2 : 1;
}

static void dijkstra_path(map_t *m, pair_t from, pair_t to) {
    static path_t path[MAP_Y][MAP_X], *p;
    static uint32_t initialized = 0;
    heap_t h;
    uint32_t x, y;

    // initialize path[][] array by setting each pos value to the proper value of x or y
    if (!initialized) {
        for (y = 0; y < MAP_Y; y++) {
            for (x = 0; x < MAP_X; x++) {
                path[y][x].pos[dim_y] = y;
                path[y][x].pos[dim_x] = x;
            }
        }
        initialized = 1;
    }

    // set the cost for each cell to the max value of integers (effectively infinity)
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            path[y][x].cost = INT_MAX;
        }
    }

    // set the starting point's cost to 0
    path[from[dim_y]][from[dim_x]].cost = 0;

    // initialize the heap h with value h->compare = path_cmp
    heap_init(&h, path_cmp, NULL);

    // for each point in the path[][] array, insert a new heap node into the heap and assign it
    // to the respective point in the path[][] array
    // i.e. path[y][x].hn = new heap_node n
    //            where n->datum = pointer to path[y][x]
    for (y = 1; y < MAP_Y - 1; y++) {
        for (x = 1; x < MAP_X - 1; x++) {
            path[y][x].hn = heap_insert(&h, &path[y][x]);
        }
    }

    // while heap h is not empty, extract the min node in the heap and assign p as a pointer to that node (node is a path_t)
    while ((p = heap_remove_min(&h))) {
        // set the value of p's heap node to null
        p->hn = NULL;

        // if (p.pos = target.pos) then path is complete
        // update the map with the path values
        // iterate through each node in the path and set the value of the corresponding position on the map to the path value
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

        // for each neighbor (north, south, east, and west) of the current node in the path:

        // if the neighboring node exists in the heap and its cost is greater than (the cost of the current node p + height of current node)
        //    multiplied by the edge penalty multiplier of the neighboring node, then set the cost of the neighbor to be equal to that calculation
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

static int build_paths(map_t *m) {
    pair_t from, to;

    from[dim_x] = 1;
    to[dim_x] = MAP_X - 2;
    from[dim_y] = m->w;
    to[dim_y] = m->e;

    dijkstra_path(m, from, to);

    from[dim_y] = 1;
    to[dim_y] = MAP_Y - 2;
    from[dim_x] = m->n;
    to[dim_x] = m->s;

    dijkstra_path(m, from, to);

    return 0;
}

// generates a random valid position for the player to spawn on the map
void place_player(map_t *m, player_t *p) {
    int x, y;
    do {
        x = (rand() % MAP_X - 2) + 1;
        y = (rand() % MAP_Y - 2) + 1;
    } while (m->map[y][x] != ter_path || x < 0 || y < 0); // idk why we get negative values sometimes but this prevents errors from it
    p->pos[dim_x] = x;
    p->pos[dim_y] = y;
}

static int gaussian[5][5] = {
    {  1,  4,  7,  4,  1 },
    {  4, 16, 26, 16,  4 },
    {  7, 26, 41, 26,  7 },
    {  4, 16, 26, 16,  4 },
    {  1,  4,  7,  4,  1 }
};

static int smooth_height(map_t *m) {
    int32_t i, x, y;
    int32_t s, t, p, q;
    queue_node_t *head, *tail, *tmp;
    uint8_t height[MAP_Y][MAP_X];

    memset(&height, 0, sizeof (height));

    /* Seed with some values */
    for (i = 1; i < 255; i += 20) {
        do {
            x = rand() % MAP_X;
            y = rand() % MAP_Y;
        } while (height[y][x]);
        height[y][x] = i;
        if (i == 1) {
            head = tail = malloc(sizeof (*tail));
        } else {
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
        }
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
    }

    /* Diffuse the values to fill the space */
    while (head) {
        x = head->x;
        y = head->y;
        i = height[y][x];

        if (x - 1 >= 0 && y - 1 >= 0 && !height[y - 1][x - 1]) {
            height[y - 1][x - 1] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x - 1;
            tail->y = y - 1;
        }
        if (x - 1 >= 0 && !height[y][x - 1]) {
            height[y][x - 1] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x - 1;
            tail->y = y;
        }
        if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
            height[y + 1][x - 1] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x - 1;
            tail->y = y + 1;
        }
        if (y - 1 >= 0 && !height[y - 1][x]) {
            height[y - 1][x] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x;
            tail->y = y - 1;
        }
        if (y + 1 < MAP_Y && !height[y + 1][x]) {
            height[y + 1][x] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x;
            tail->y = y + 1;
        }
        if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
            height[y - 1][x + 1] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x + 1;
            tail->y = y - 1;
        }
        if (x + 1 < MAP_X && !height[y][x + 1]) {
            height[y][x + 1] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x + 1;
            tail->y = y;
        }
        if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
            height[y + 1][x + 1] = i;
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x + 1;
            tail->y = y + 1;
        }

        tmp = head;
        head = head->next;
        free(tmp);
    }

    /* And smooth it a bit with a gaussian convolution */
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            for (s = t = p = 0; p < 5; p++) {
                for (q = 0; q < 5; q++) {
                    if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
                        x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
                        s += gaussian[p][q];
                        t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
                    }
                }
            }
            m->height[y][x] = t / s;
        }
    }
    /* Let's do it again, until it's smooth like Kenny G. */
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            for (s = t = p = 0; p < 5; p++) {
                for (q = 0; q < 5; q++) {
                    if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y &&
                        x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
                        s += gaussian[p][q];
                        t += height[y + (p - 2)][x + (q - 2)] * gaussian[p][q];
                    }
                }
            }
            m->height[y][x] = t / s;
        }
    }

    return 0;
}

static void find_building_location(map_t *m, pair_t p) {
    do {
        p[dim_x] = rand() % (MAP_X - 5) + 3;
        p[dim_y] = rand() % (MAP_Y - 10) + 5;

        if ((((mapxy(p[dim_x] - 1, p[dim_y]    ) == ter_path)     &&
              (mapxy(p[dim_x] - 1, p[dim_y] + 1) == ter_path))    ||
             ((mapxy(p[dim_x] + 2, p[dim_y]    ) == ter_path)     &&
              (mapxy(p[dim_x] + 2, p[dim_y] + 1) == ter_path))    ||
             ((mapxy(p[dim_x]    , p[dim_y] - 1) == ter_path)     &&
              (mapxy(p[dim_x] + 1, p[dim_y] - 1) == ter_path))    ||
             ((mapxy(p[dim_x]    , p[dim_y] + 2) == ter_path)     &&
              (mapxy(p[dim_x] + 1, p[dim_y] + 2) == ter_path)))   &&
            (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_mart)     &&
              (mapxy(p[dim_x]    , p[dim_y]    ) != ter_center)   &&
              (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_mart)     &&
              (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_center)   &&
              (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_mart)     &&
              (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_center)   &&
              (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_mart)     &&
              (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_center))) &&
            (((mapxy(p[dim_x]    , p[dim_y]    ) != ter_path)     &&
              (mapxy(p[dim_x] + 1, p[dim_y]    ) != ter_path)     &&
              (mapxy(p[dim_x]    , p[dim_y] + 1) != ter_path)     &&
              (mapxy(p[dim_x] + 1, p[dim_y] + 1) != ter_path)))) {
            break;
        }
    } while (1);
}

static int place_pokemart(map_t *m) {
    pair_t p;

    find_building_location(m, p);

    mapxy(p[dim_x]    , p[dim_y]    ) = ter_mart;
    mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_mart;
    mapxy(p[dim_x]    , p[dim_y] + 1) = ter_mart;
    mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_mart;

    return 0;
}

static int place_center(map_t *m) {
    pair_t p;

    find_building_location(m, p);

    mapxy(p[dim_x]    , p[dim_y]    ) = ter_center;
    mapxy(p[dim_x] + 1, p[dim_y]    ) = ter_center;
    mapxy(p[dim_x]    , p[dim_y] + 1) = ter_center;
    mapxy(p[dim_x] + 1, p[dim_y] + 1) = ter_center;

    return 0;
}

static int map_terrain(map_t *m, uint8_t n, uint8_t s, uint8_t e, uint8_t w) {
    int32_t i, x, y;
    queue_node_t *head, *tail, *tmp;
    int num_grass, num_clearing, num_mountain, num_forest, num_total;
    terrain_type_t type;
    int added_current = 0;

    num_grass = rand() % 4 + 2;
    num_clearing = rand() % 4 + 2;
    num_mountain = rand() % 2 + 1;
    num_forest = rand() % 2 + 1;
    num_total = num_grass + num_clearing + num_mountain + num_forest;

    memset(&m->map, 0, sizeof (m->map));

    /* Seed with some values */
    for (i = 0; i < num_total; i++) {
        do {
            x = rand() % MAP_X;
            y = rand() % MAP_Y;
        } while (m->map[y][x]);
        if (i == 0) {
            type = ter_grass;
        } else if (i == num_grass) {
            type = ter_clearing;
        } else if (i == num_grass + num_clearing) {
            type = ter_mountain;
        } else if (i == num_grass + num_clearing + num_mountain) {
            type = ter_forest;
        }
        m->map[y][x] = type;
        if (i == 0) {
            head = tail = malloc(sizeof (*tail));
        } else {
            tail->next = malloc(sizeof (*tail));
            tail = tail->next;
        }
        tail->next = NULL;
        tail->x = x;
        tail->y = y;
    }

    /* Diffuse the values to fill the space */
    while (head) {
        x = head->x;
        y = head->y;
        i = m->map[y][x];

        if (x - 1 >= 0 && !m->map[y][x - 1]) {
            if ((rand() % 100) < 80) {
                m->map[y][x - 1] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x - 1;
                tail->y = y;
            } else if (!added_current) {
                added_current = 1;
                m->map[y][x] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y;
            }
        }

        if (y - 1 >= 0 && !m->map[y - 1][x]) {
            if ((rand() % 100) < 20) {
                m->map[y - 1][x] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y - 1;
            } else if (!added_current) {
                added_current = 1;
                m->map[y][x] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y;
            }
        }

        if (y + 1 < MAP_Y && !m->map[y + 1][x]) {
            if ((rand() % 100) < 20) {
                m->map[y + 1][x] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y + 1;
            } else if (!added_current) {
                added_current = 1;
                m->map[y][x] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y;
            }
        }

        if (x + 1 < MAP_X && !m->map[y][x + 1]) {
            if ((rand() % 100) < 80) {
                m->map[y][x + 1] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x + 1;
                tail->y = y;
            } else if (!added_current) {
                added_current = 1;
                m->map[y][x] = i;
                tail->next = malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y;
            }
        }

        added_current = 0;
        tmp = head;
        head = head->next;
        free(tmp);
    }

    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            if (y == 0 || y == MAP_Y - 1 ||
                x == 0 || x == MAP_X - 1) {
                mapxy(x, y) = ter_boulder;
            }
        }
    }

    m->n = n;
    m->s = s;
    m->e = e;
    m->w = w;

    mapxy(n,         0        ) = ter_path;
    mapxy(n,         1        ) = ter_path;
    mapxy(s,         MAP_Y - 1) = ter_path;
    mapxy(s,         MAP_Y - 2) = ter_path;
    mapxy(0,         w        ) = ter_path;
    mapxy(1,         w        ) = ter_path;
    mapxy(MAP_X - 1, e        ) = ter_path;
    mapxy(MAP_X - 2, e        ) = ter_path;

    return 0;
}

static int place_boulders(map_t *m) {
    int i;
    int x, y;

    for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
        y = rand() % (MAP_Y - 2) + 1;
        x = rand() % (MAP_X - 2) + 1;
        if (m->map[y][x] != ter_forest && m->map[y][x] != ter_path) {
            m->map[y][x] = ter_boulder;
        }
    }

    return 0;
}

static int place_trees(map_t *m) {
    int i;
    int x, y;

    for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
        y = rand() % (MAP_Y - 2) + 1;
        x = rand() % (MAP_X - 2) + 1;
        if (m->map[y][x] != ter_mountain && m->map[y][x] != ter_path) {
            m->map[y][x] = ter_tree;
        }
    }

    return 0;
}

int new_map(map_t *m, uint8_t n, uint8_t s, uint8_t e, uint8_t w, int poke_place_prob) {
    smooth_height(m);
    map_terrain(m, n, s, e, w);
    build_paths(m);
    place_boulders(m);
    place_trees(m);

    int martProb = rand() % 100;
    if (martProb < poke_place_prob) {
        place_pokemart(m);
    }
    int centerProb = rand() % 100;
    if (centerProb < poke_place_prob) {
        place_center(m);
    }

    return 0;
}

void print_map(map_t *m, player_t *p) {
    int x, y;
    int default_reached = 0;

    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            /* Check if the player character is at these coordinates */
            if (x == p->pos[dim_x] && y == p->pos[dim_y]) {
                putchar('@');
                continue;
            }

            switch (m->map[y][x]) {
                case ter_boulder:
                case ter_mountain:
                    putchar('%');
                    break;
                case ter_tree:
                case ter_forest:
                    putchar('^');
                    break;
                case ter_path:
                    putchar('#');
                    break;
                case ter_mart:
                    putchar('M');
                    break;
                case ter_center:
                    putchar('C');
                    break;
                case ter_grass:
                    putchar(':');
                    break;
                case ter_clearing:
                    putchar('.');
                    break;
                default:
                    default_reached = 1;
                    break;
            }
        }
        putchar('\n');
    }

    if (default_reached) {
        fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
    }
}