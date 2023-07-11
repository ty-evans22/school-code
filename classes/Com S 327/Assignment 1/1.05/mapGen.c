#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "mapGen.h"
#include "dijkstra.h"
#include "trainer.h"
#include "world.h"

#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95
#define MIN_TRAINERS       7
#define ADD_TRAINER_PROB   50

#define malloc(size) ({          \
  void *_tmp;                    \
  assert((_tmp = malloc(size))); \
  _tmp;                          \
})

#define rand_dir(dir) {             \
    int _i = rand() & 0x7;          \
    dir[0] = direction_vals[_i][0]; \
    dir[1] = direction_vals[_i][1]; \
}

pair_t direction_vals[9] = {
    { -1, -1 },
    { -1,  0 },
    { -1,  1 },
    {  0, -1 },
    {  0,  1 },
    {  1, -1 },
    {  1,  0 },
    {  1,  1 },
    {  0,  0 },
};

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

static int32_t trainer_cmp(const void *key, const void *with) {
    return ((trainer_t *) key)->curr_time - ((trainer_t *) with)->curr_time;
}

void delete_trainer(void *v) {
    if (v == &world.player) {
        free(world.player.pc);
    } else {
        free(((trainer_t *) v)->npc);
        free(v);
    }
}

static int build_paths(map_t *m) {
    pair_t from, to;

    if (m->e != -1 && m->w != -1) {
        from[dim_x] = 1;
        to[dim_x] = MAP_X - 2;
        from[dim_y] = m->w;
        to[dim_y] = m->e;

        dijkstra_path(m, from, to);
    }

    if (m->n != -1 && m->s != -1) {
        from[dim_y] = 1;
        to[dim_y] = MAP_Y - 2;
        from[dim_x] = m->n;
        to[dim_x] = m->s;

        dijkstra_path(m, from, to);
    }

    if (m->e == -1) {
        if (m->s == -1) {
            from[dim_x] = 1;
            from[dim_y] = m->w;
            to[dim_x] = m->n;
            to[dim_y] = 1;
        } else {
            from[dim_x] = 1;
            from[dim_y] = m->w;
            to[dim_x] = m->s;
            to[dim_y] = MAP_Y - 2;
        }

        dijkstra_path(m, from, to);
    }

    if (m->w == -1) {
        if (m->s == -1) {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->e;
            to[dim_x] = m->n;
            to[dim_y] = 1;
        } else {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->e;
            to[dim_x] = m->s;
            to[dim_y] = MAP_Y - 2;
        }

        dijkstra_path(m, from, to);
    }

    if (m->n == -1) {
        if (m->e == -1) {
            from[dim_x] = 1;
            from[dim_y] = m->w;
            to[dim_x] = m->s;
            to[dim_y] = MAP_Y - 2;
        } else {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->e;
            to[dim_x] = m->s;
            to[dim_y] = MAP_Y - 2;
        }

        dijkstra_path(m, from, to);
    }

    if (m->s == -1) {
        if (m->e == -1) {
            from[dim_x] = 1;
            from[dim_y] = m->w;
            to[dim_x] = m->n;
            to[dim_y] = 1;
        } else {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->e;
            to[dim_x] = m->n;
            to[dim_y] = 1;
        }

        dijkstra_path(m, from, to);
    }

    return 0;
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
                    if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y && x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
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
                    if (y + (p - 2) >= 0 && y + (p - 2) < MAP_Y && x + (q - 2) >= 0 && x + (q - 2) < MAP_X) {
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

static int map_terrain(map_t *m, int8_t n, int8_t s, int8_t e, int8_t w) {
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

    /* Diffuse the vaules to fill the space */
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
            if (y == 0 || y == MAP_Y - 1 || x == 0 || x == MAP_X - 1) {
                mapxy(x, y) = ter_boulder;
            }
        }
    }

    m->n = n;
    m->s = s;
    m->e = e;
    m->w = w;

    if (n != -1) {
        mapxy(n,         0        ) = ter_path;
        mapxy(n,         1        ) = ter_path;
    }
    if (s != -1) {
        mapxy(s,         MAP_Y - 1) = ter_path;
        mapxy(s,         MAP_Y - 2) = ter_path;
    }
    if (w != -1) {
        mapxy(0,         w        ) = ter_path;
        mapxy(1,         w        ) = ter_path;
    }
    if (e != -1) {
        mapxy(MAP_X - 1, e        ) = ter_path;
        mapxy(MAP_X - 2, e        ) = ter_path;
    }

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

void rand_pos(pair_t pos) {
    pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
    pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void new_hiker() {
    pair_t pos;
    trainer_t *t;

    do {
        rand_pos(pos);
    } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
             world.curr_map->tmap[pos[dim_y]][pos[dim_x]]);

    t = malloc(sizeof (*t));
    t->npc = malloc(sizeof (*t->npc));
    t->pos[dim_y] = pos[dim_y];
    t->pos[dim_x] = pos[dim_x];
    t->npc->ttype = tr_hiker;
    t->npc->mtype = move_hiker;
    t->npc->dir[dim_x] = 0;
    t->npc->dir[dim_y] = 0;
    t->npc->defeated = 0;
    t->symbol = 'h';
    t->curr_time = 0;
    heap_insert(&world.curr_map->queue, t);
    world.curr_map->tmap[pos[dim_y]][pos[dim_x]] = t;
}

void new_rival() {
    pair_t pos;
    trainer_t *t;

    do {
        rand_pos(pos);
    } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
             world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
             world.curr_map->tmap[pos[dim_y]][pos[dim_x]]);

    t = malloc(sizeof (*t));
    t->npc = malloc(sizeof (*t->npc));
    t->pos[dim_y] = pos[dim_y];
    t->pos[dim_x] = pos[dim_x];
    t->npc->ttype = tr_rival;
    t->npc->mtype = move_rival;
    t->npc->dir[dim_x] = 0;
    t->npc->dir[dim_y] = 0;
    t->npc->defeated = 0;
    t->symbol = 'r';
    t->curr_time = 0;
    heap_insert(&world.curr_map->queue, t);
    world.curr_map->tmap[pos[dim_y]][pos[dim_x]] = t;
}

void new_tr_other() {
    pair_t pos;
    trainer_t *t;

    do {
        rand_pos(pos);
    } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
             world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
             world.curr_map->tmap[pos[dim_y]][pos[dim_x]]);
    
    t = malloc(sizeof (*t));
    t->npc = malloc(sizeof (*t->npc));
    t->pos[dim_y] = pos[dim_y];
    t->pos[dim_x] = pos[dim_x];
    t->npc->ttype = tr_other;
    t->npc->defeated = 0;
    switch (rand() % 4) {
        case 0:
            t->npc->mtype = move_pacer;
            t->symbol = 'p';
            break;
        case 1:
            t->npc->mtype = move_wanderer;
            t->symbol = 'w';
            break;
        case 2:
            t->npc->mtype = move_stationary;
            t->symbol = 's';
            break;
        case 3:
            t->npc->mtype = move_walker;
            t->symbol = 'n';
            break;
    }
    rand_dir(t->npc->dir);
    t->curr_time = 0;
    heap_insert(&world.curr_map->queue, t);
    world.curr_map->tmap[pos[dim_y]][pos[dim_x]] = t;
}

void place_trainers() {
    int num_trainers = 2;

    /* Always place a hiker and a rival, then place a random amount of others */
    new_hiker();
    new_rival();
    do {
        //higher probability of non-hikers and rivals
        switch(rand() % 10) {
            case 0:
                new_hiker();
                break;
            case 1:
                new_rival();
                break;
            default:
                new_tr_other();
                break;
        }
    } while (++num_trainers < MIN_TRAINERS ||
            ((rand() % 100) < ADD_TRAINER_PROB));
}

/* Place the player into a valid location on the map */
void init_pc() {
    int x, y;
    do {
        x = (rand() % (MAP_X - 2)) + 1;
        y = (rand() % (MAP_Y - 2)) + 1;
    } while (world.curr_map->map[y][x] != ter_path);
    world.player.pos[dim_x] = x;
    world.player.pos[dim_y] = y;
    world.player.symbol = '@';
    world.player.pc = malloc(sizeof (*world.player.pc));
    world.player.pc->poke_mart = 0;
    world.player.pc->poke_center = 0;

    world.curr_map->tmap[y][x] = &world.player;
    world.player.curr_time = 0;

    heap_insert(&world.curr_map->queue, &world.player);
}

void init_pc_traverse_old() {
    if (world.player.pc->dir == north) {
        world.player.pos[dim_x] = world.curr_map->s;
        world.player.pos[dim_y] = MAP_Y - 2;
    } else if (world.player.pc->dir == west) {
        world.player.pos[dim_x] = MAP_X - 2;
        world.player.pos[dim_y] = world.curr_map->e;
    } else if (world.player.pc->dir == south) {
        world.player.pos[dim_x] = world.curr_map->n;
        world.player.pos[dim_y] = 1;
    } else if (world.player.pc->dir == east) {
        world.player.pos[dim_x] = 1;
        world.player.pos[dim_y] = world.curr_map->w;
    }

    world.curr_map->tmap[world.player.pos[dim_y]][world.player.pos[dim_x]] = &world.player;

    /* Get lowest curr_time from heap */
    trainer_t *t;
    t = heap_peek_min(&world.curr_map->queue);
    world.player.curr_time = t->curr_time;
    // heap_insert(&world.curr_map->queue, t);
    heap_insert(&world.curr_map->queue, &world.player);
}

void init_pc_traverse_new() {
    if (world.player.pc->dir == north) {
        world.player.pos[dim_x] = world.curr_map->s;
        world.player.pos[dim_y] = MAP_Y - 2;
    } else if (world.player.pc->dir == west) {
        world.player.pos[dim_x] = MAP_X - 2;
        world.player.pos[dim_y] = world.curr_map->e;
    } else if (world.player.pc->dir == south) {
        world.player.pos[dim_x] = world.curr_map->n;
        world.player.pos[dim_y] = 1;
    } else if (world.player.pc->dir == east) {
        world.player.pos[dim_x] = 1;
        world.player.pos[dim_y] = world.curr_map->w;
    }

    world.curr_map->tmap[world.player.pos[dim_y]][world.player.pos[dim_x]] = &world.player;
    world.player.curr_time = 0;

    heap_insert(&world.curr_map->queue, &world.player);
}

/* Sets the curr_map pointer in world to the map at curr_index. Generates a new map there if neeeded. */
int new_map() {
    int d, p;
    int e, w, n, s;

    /* Check if the map at the new index has already been generated */
    if (world.world[world.curr_index[dim_y]][world.curr_index[dim_x]]) {
        world.curr_map = world.world[world.curr_index[dim_y]][world.curr_index[dim_x]];
        init_pc_traverse_old();
        return 0;
    }

    world.curr_map                                                    =
        world.world[world.curr_index[dim_y]][world.curr_index[dim_x]] =
        malloc(sizeof (*world.curr_map));

    smooth_height(world.curr_map);
    
    if (!world.curr_index[dim_y]) {
        n = -1;
    } else if (world.world[world.curr_index[dim_y] - 1][world.curr_index[dim_x]]) {
        n = world.world[world.curr_index[dim_y] - 1][world.curr_index[dim_x]]->s;
    } else {
        n = 1 + rand() % (MAP_X - 2);
    }
    if (world.curr_index[dim_y] == WORLD_SIZE - 1) {
        s = -1;
    } else if (world.world[world.curr_index[dim_y] + 1][world.curr_index[dim_x]]) {
        s = world.world[world.curr_index[dim_y] + 1][world.curr_index[dim_x]]->n;
    } else  {
        s = 1 + rand() % (MAP_X - 2);
    }
    if (!world.curr_index[dim_x]) {
        w = -1;
    } else if (world.world[world.curr_index[dim_y]][world.curr_index[dim_x] - 1]) {
        w = world.world[world.curr_index[dim_y]][world.curr_index[dim_x] - 1]->e;
    } else {
        w = 1 + rand() % (MAP_Y - 2);
    }
    if (world.curr_index[dim_x] == WORLD_SIZE - 1) {
        e = -1;
    } else if (world.world[world.curr_index[dim_y]][world.curr_index[dim_x] + 1]) {
        e = world.world[world.curr_index[dim_y]][world.curr_index[dim_x] + 1]->w;
    } else {
        e = 1 + rand() % (MAP_Y - 2);
    }
    
    map_terrain(world.curr_map, n, s, e, w);
        
    place_boulders(world.curr_map);
    place_trees(world.curr_map);
    build_paths(world.curr_map);
    d = (abs(world.curr_index[dim_x] - (WORLD_SIZE / 2)) +
        abs(world.curr_index[dim_y] - (WORLD_SIZE / 2)));
    p = d > 200 ? 5 : (50 - ((45 * d) / 200));
    if ((rand() % 100) < p || !d) {
        place_pokemart(world.curr_map);
    }
    if ((rand() % 100) < p || !d) {
        place_center(world.curr_map);
    }

    /* Initialize player and trainers */
    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            world.curr_map->tmap[y][x] = NULL;
        }
    }

    heap_init(&world.curr_map->queue, trainer_cmp, delete_trainer);

    if (world.curr_index[dim_y] == WORLD_SIZE / 2 && world.curr_index[dim_x] == WORLD_SIZE / 2) {
        init_pc();
    } else {
        init_pc_traverse_new();
    }

    pathfind(world.curr_map);
    place_trainers();

    return 0;
}