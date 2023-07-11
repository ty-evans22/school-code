#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "world.h"
#include "mapGen.h"
#include "dijkstra.h"

/* settings for map generation */
#define MIN_TREES          10
#define MIN_BOULDERS       10
#define TREE_PROB          95
#define BOULDER_PROB       95
#define MIN_TRAINERS       7   
#define ADD_TRAINER_PROB   50

typedef struct queue_node {
  int x, y;
  struct queue_node *next;
} queue_node_t;

/* comparison used to decide next trainer in priority queue */
int32_t trainer_cmp(const void *key, const void *with) {
    return ((Trainer *) key)->get_curr_time() - ((Trainer *) with)->get_curr_time();
}

void delete_trainer(void *v) {
    if (v != &world.player) {
        delete (NPC *) v;
    }
}

static int build_paths(Map *m) {
    pair_t from, to;

    if (m->get_east_exit() != -1 && m->get_west_exit() != -1) {
        from[dim_x] = 1;
        to[dim_x] = MAP_X - 2;
        from[dim_y] = m->get_west_exit();
        to[dim_y] = m->get_east_exit();

        dijkstra_path(m, from, to);
    }

    if (m->get_north_exit() != -1 && m->get_south_exit() != -1) {
        from[dim_y] = 1;
        to[dim_y] = MAP_Y - 2;
        from[dim_x] = m->get_north_exit();
        to[dim_x] = m->get_south_exit();

        dijkstra_path(m, from, to);
    }

    if (m->get_east_exit() == -1) {
        if (m->get_south_exit() == -1) {
            from[dim_x] = 1;
            from[dim_y] = m->get_west_exit();
            to[dim_x] = m->get_north_exit();
            to[dim_y] = 1;
        } else {
            from[dim_x] = 1;
            from[dim_y] = m->get_west_exit();
            to[dim_x] = m->get_south_exit();
            to[dim_y] = MAP_Y - 2;
        }

        dijkstra_path(m, from, to);
    }

    if (m->get_west_exit() == -1) {
        if (m->get_south_exit() == -1) {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->get_east_exit();
            to[dim_x] = m->get_north_exit();
            to[dim_y] = 1;
        } else {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->get_east_exit();
            to[dim_x] = m->get_south_exit();
            to[dim_y] = MAP_Y - 2;
        }

        dijkstra_path(m, from, to);
    }

    if (m->get_north_exit() == -1) {
        if (m->get_east_exit() == -1) {
            from[dim_x] = 1;
            from[dim_y] = m->get_west_exit();
            to[dim_x] = m->get_south_exit();
            to[dim_y] = MAP_Y - 2;
        } else {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->get_east_exit();
            to[dim_x] = m->get_south_exit();
            to[dim_y] = MAP_Y - 2;
        }

        dijkstra_path(m, from, to);
    }

    if (m->get_south_exit() == -1) {
        if (m->get_east_exit() == -1) {
            from[dim_x] = 1;
            from[dim_y] = m->get_west_exit();
            to[dim_x] = m->get_north_exit();
            to[dim_y] = 1;
        } else {
            from[dim_x] = MAP_X - 2;
            from[dim_y] = m->get_east_exit();
            to[dim_x] = m->get_north_exit();
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

static int smooth_height(Map *m) {
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
            head = tail = (queue_node_t*)malloc(sizeof (*tail));
        } else {
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
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
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x - 1;
            tail->y = y - 1;
        }
        if (x - 1 >= 0 && !height[y][x - 1]) {
            height[y][x - 1] = i;
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x - 1;
            tail->y = y;
        }
        if (x - 1 >= 0 && y + 1 < MAP_Y && !height[y + 1][x - 1]) {
            height[y + 1][x - 1] = i;
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x - 1;
            tail->y = y + 1;
        }
        if (y - 1 >= 0 && !height[y - 1][x]) {
            height[y - 1][x] = i;
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x;
            tail->y = y - 1;
        }
        if (y + 1 < MAP_Y && !height[y + 1][x]) {
            height[y + 1][x] = i;
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x;
            tail->y = y + 1;
        }
        if (x + 1 < MAP_X && y - 1 >= 0 && !height[y - 1][x + 1]) {
            height[y - 1][x + 1] = i;
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x + 1;
            tail->y = y - 1;
        }
        if (x + 1 < MAP_X && !height[y][x + 1]) {
            height[y][x + 1] = i;
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
            tail = tail->next;
            tail->next = NULL;
            tail->x = x + 1;
            tail->y = y;
        }
        if (x + 1 < MAP_X && y + 1 < MAP_Y && !height[y + 1][x + 1]) {
            height[y + 1][x + 1] = i;
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
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
            m->set_height(x, y, t / s);
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
            m->set_height(x, y, t / s);
        }
    }

    return 0;
}

static void find_building_location(Map *m, pair_t p) {
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

static int place_pokemart(Map *m) {
    pair_t p;

    find_building_location(m, p);

    m->set_map_val(p, ter_mart);
    m->set_map_val(p[dim_x] + 1, p[dim_y]    , ter_mart);
    m->set_map_val(p[dim_x]    , p[dim_y] + 1, ter_mart);
    m->set_map_val(p[dim_x] + 1, p[dim_y] + 1, ter_mart);

    return 0;
}

static int place_center(Map *m) {  
    pair_t p;

    find_building_location(m, p);

    m->set_map_val(p, ter_center);
    m->set_map_val(p[dim_x] + 1, p[dim_y]    , ter_center);
    m->set_map_val(p[dim_x]    , p[dim_y] + 1, ter_center);
    m->set_map_val(p[dim_x] + 1, p[dim_y] + 1, ter_center);

    return 0;
}

static int map_terrain(Map *m) {
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

    m->memset_map();

    /* Seed with some values */
    for (i = 0; i < num_total; i++) {
        do {
            x = rand() % MAP_X;
            y = rand() % MAP_Y;
        } while (m->get_map_val(x, y));
        if (i == 0) {
            type = ter_grass;
        } else if (i == num_grass) {
            type = ter_clearing;
        } else if (i == num_grass + num_clearing) {
            type = ter_mountain;
        } else if (i == num_grass + num_clearing + num_mountain) {
            type = ter_forest;
        }
        m->set_map_val(x, y, type);
        if (i == 0) {
            head = tail = (queue_node_t*)malloc(sizeof (*tail));
        } else {
            tail->next = (queue_node_t*)malloc(sizeof (*tail));
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
        i = m->get_map_val(x, y);
    
        if (x - 1 >= 0 && !m->get_map_val(x - 1, y)) {
            if ((rand() % 100) < 80) {
                m->set_map_val(x - 1, y, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x - 1;
                tail->y = y;
            } else if (!added_current) {
                added_current = 1;
                m->set_map_val(x, y, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y;
            }
        }

        if (y - 1 >= 0 && !m->get_map_val(x, y - 1)) {
            if ((rand() % 100) < 20) {
                m->set_map_val(x, y - 1, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y - 1;
            } else if (!added_current) {
                added_current = 1;
                m->set_map_val(x, y, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y;
            }
        }

        if (y + 1 < MAP_Y && !m->get_map_val(x, y + 1)) {
            if ((rand() % 100) < 20) {
                m->set_map_val(x, y + 1, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y + 1;
            } else if (!added_current) {
                added_current = 1;
                m->set_map_val(x, y, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x;
                tail->y = y;
            }
        }

        if (x + 1 < MAP_X && !m->get_map_val(x + 1, y)) {
            if ((rand() % 100) < 80) {
                m->set_map_val(x + 1, y, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
                tail = tail->next;
                tail->next = NULL;
                tail->x = x + 1;
                tail->y = y;
            } else if (!added_current) {
                added_current = 1;
                m->set_map_val(x, y, static_cast<terrain_type_t>(i));
                tail->next = (queue_node_t*)malloc(sizeof (*tail));
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
                m->set_map_val(x, y, ter_boulder);
            }
        }
    }

    if (m->get_north_exit() != -1) {
        m->set_map_val(m->get_north_exit(), 0, ter_exit);
        m->set_map_val(m->get_north_exit(), 1, ter_path);
    }
    if (m->get_south_exit() != -1) {
        m->set_map_val(m->get_south_exit(), MAP_Y - 1, ter_exit);
        m->set_map_val(m->get_south_exit(), MAP_Y - 2, ter_path);
    }
    if (m->get_west_exit() != -1) {
        m->set_map_val(0, m->get_west_exit(), ter_exit);
        m->set_map_val(1, m->get_west_exit(), ter_path);
    }
    if (m->get_east_exit() != -1) {
        m->set_map_val(MAP_X - 1, m->get_east_exit(), ter_exit);
        m->set_map_val(MAP_X - 2, m->get_east_exit(), ter_path);
    }

    return 0;
}

static int place_boulders(Map *m) {
    int i;
    int x, y;

    for (i = 0; i < MIN_BOULDERS || rand() % 100 < BOULDER_PROB; i++) {
        y = rand() % (MAP_Y - 2) + 1;
        x = rand() % (MAP_X - 2) + 1;
        if (m->get_map_val(x, y) != ter_forest && m->get_map_val(x, y) != ter_path) {
            m->set_map_val(x, y, ter_boulder);
        }
    }

    return 0;
}

static int place_trees(Map *m) {
    int i;
    int x, y;
  
    for (i = 0; i < MIN_TREES || rand() % 100 < TREE_PROB; i++) {
        y = rand() % (MAP_Y - 2) + 1;
        x = rand() % (MAP_X - 2) + 1;
        if (m->get_map_val(x, y) != ter_mountain && m->get_map_val(x, y) != ter_path) {
            m->set_map_val(x, y, ter_tree);
        }
    }

    return 0;
}

/* generates a random position on the map */
void rand_pos(pair_t pos) {
    pos[dim_x] = (rand() % (MAP_X - 2)) + 1;
    pos[dim_y] = (rand() % (MAP_Y - 2)) + 1;
}

void new_hiker() {
    pair_t pos;
    NPC *t;

    do {
        rand_pos(pos);
    } while (world.hiker_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
             world.get_cur_map()->get_trainer(pos)               ||
             pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
             pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

    t = new NPC;
    t->set_pos(pos);
    t->set_dir(0, 0);
    t->set_ttype(tr_hiker);
    t->set_mtype(move_hiker);
    t->is_defeated = false;
    t->set_symbol('h');
    t->set_curr_time(0);
    heap_insert(&world.get_cur_map()->queue, t);
    world.get_cur_map()->set_trainer(pos, t);
}

void new_rival() {
    pair_t pos;
    NPC *t;

    do {
        rand_pos(pos);
    } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
             world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
             world.get_cur_map()->get_trainer(pos)               ||
             pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
             pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);

    t = new NPC;
    t->set_pos(pos);
    t->set_dir(0, 0);
    t->set_ttype(tr_rival);
    t->set_mtype(move_rival);
    t->is_defeated = false;
    t->set_symbol('r');
    t->set_curr_time(0);
    heap_insert(&world.get_cur_map()->queue, t);
    world.get_cur_map()->set_trainer(pos, t);
}

void new_tr_other() {
    pair_t pos;
    NPC *t;

    do {
        rand_pos(pos);
    } while (world.rival_dist[pos[dim_y]][pos[dim_x]] == INT_MAX ||
             world.rival_dist[pos[dim_y]][pos[dim_x]] < 0        ||
             world.get_cur_map()->get_trainer(pos)               ||
             pos[dim_x] < 3 || pos[dim_x] > MAP_X - 4            ||
             pos[dim_y] < 3 || pos[dim_y] > MAP_Y - 4);
    
    t = new NPC;
    t->set_pos(pos);
    t->set_ttype(tr_other);

    switch (rand() % 4) {
        case 0:
            t->set_mtype(move_pace);
            t->set_symbol('p');
            break;
        case 1:
            t->set_mtype(move_wander);
            t->set_symbol('w');
            break;
        case 2:
            t->set_mtype(move_sentry);
            t->set_symbol('s');
            break;
        case 3:
            t->set_mtype(move_walk);
            t->set_symbol('n');
            break;
    }
    rand_dir(t);
    t->is_defeated = false;
    t->set_curr_time(0);
    heap_insert(&world.get_cur_map()->queue, t);
    world.get_cur_map()->set_trainer(pos, t);
}

void place_trainers() {
    world.get_cur_map()->set_num_trainers(2);

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
        world.get_cur_map()->set_num_trainers(world.get_cur_map()->get_num_trainers() + 1);
    } while (world.get_cur_map()->get_num_trainers() < MIN_TRAINERS ||
            ((rand() % 100) < ADD_TRAINER_PROB));
}

void init_pc() {
    int x, y;

    do {
        x = rand() % (MAP_X - 2) + 1;
        y = rand() % (MAP_Y - 2) + 1;
    } while (world.get_cur_map()->get_map_val(x, y) != ter_path);

    world.player.set_pos(x, y);
    world.player.set_symbol('@');
    world.player.set_curr_time(0);
    heap_insert(&world.get_cur_map()->queue, &world.player);
    world.get_cur_map()->set_trainer(x, y, &world.player);
}

void place_pc() {
    Trainer *c;

    if (world.player.get_pos_x() == 1) {
        world.player.set_pos_x(MAP_X - 2);
    } else if (world.player.get_pos_x() == MAP_X - 2) {
        world.player.set_pos_x(1);
    } else if (world.player.get_pos_y() == 1) {
        world.player.set_pos_y(MAP_Y - 2);
    } else if (world.player.get_pos_y() == MAP_Y - 2) {
        world.player.set_pos_y(1);
    }

    world.get_cur_map()->set_trainer(world.player.get_pos_x(), world.player.get_pos_y(), &world.player);

    if ((c = (Trainer *) heap_peek_min(&world.get_cur_map()->queue))) {
        world.player.set_curr_time(c->get_curr_time());
    } else {
        world.player.set_curr_time(0);
    }
}

// New map expects cur_idx to refer to the index to be generated.  If that
// map has already been generated then the only thing this does is set
// cur_map.
int new_map(int teleport) {
    int d, p;
    int e, w, n, s;
    int x, y;
    
    if (world.get_map(*world.get_cur_idx())) {
        world.set_cur_map(world.get_map(*world.get_cur_idx()));
        place_pc();

        return 0;
    }

    world.set_map(*world.get_cur_idx(), (Map *) malloc(sizeof (*world.get_cur_map())));
    world.set_cur_map(world.get_map(*world.get_cur_idx()));

    smooth_height(world.get_cur_map());

    if (!world.get_cur_idx_y()) {
        n = -1;
    } else if (world.get_map(world.get_cur_idx_x(), world.get_cur_idx_y() - 1)) {
        n = world.get_map(world.get_cur_idx_x(), world.get_cur_idx_y() - 1)->get_south_exit();
    } else {
        n = 3 + rand() % (MAP_X - 6);
    }
    if (world.get_cur_idx_y() == WORLD_SIZE - 1) {
        s = -1;
    } else if (world.get_map(world.get_cur_idx_x(), world.get_cur_idx_y() + 1)) {
        s = world.get_map(world.get_cur_idx_x(), world.get_cur_idx_y() + 1)->get_north_exit();
    } else  {
        s = 3 + rand() % (MAP_X - 6);
    }
    if (!world.get_cur_idx_x()) {
        w = -1;
    } else if (world.get_map(world.get_cur_idx_x() - 1, world.get_cur_idx_y())) {
        w = world.get_map(world.get_cur_idx_x() - 1, world.get_cur_idx_y())->get_east_exit();
    } else {
        w = 3 + rand() % (MAP_Y - 6);
    }
    if (world.get_cur_idx_x() == WORLD_SIZE - 1) {
        e = -1;
    } else if (world.get_map(world.get_cur_idx_x() + 1, world.get_cur_idx_y())) {
        e = world.get_map(world.get_cur_idx_x() + 1, world.get_cur_idx_y())->get_west_exit();
    } else {
        e = 3 + rand() % (MAP_Y - 6);
    }

    world.get_cur_map()->set_north_exit(n);
    world.get_cur_map()->set_south_exit(s);
    world.get_cur_map()->set_east_exit(e);
    world.get_cur_map()->set_west_exit(w);
    
    map_terrain(world.get_cur_map());
        
    place_boulders(world.get_cur_map());
    place_trees(world.get_cur_map());
    build_paths(world.get_cur_map());
    d = (abs(world.get_cur_idx_x() - (WORLD_SIZE / 2)) +
        abs(world.get_cur_idx_y() - (WORLD_SIZE / 2)));
    p = d > 200 ? 5 : (50 - ((45 * d) / 200));
    if ((rand() % 100) < p || !d) {
        place_pokemart(world.get_cur_map());
    }
    if ((rand() % 100) < p || !d) {
        place_center(world.get_cur_map());
    }

    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
        world.get_cur_map()->set_trainer(x, y, NULL);
        }
    }

    heap_init(&world.get_cur_map()->queue, trainer_cmp, delete_trainer);

    if ((world.get_cur_idx_x() == WORLD_SIZE / 2) &&
        (world.get_cur_idx_y() == WORLD_SIZE / 2)) {
        init_pc();
    } else {
        place_pc();
    }

    pathfind(world.get_cur_map());
    if (teleport) {
        do {
        world.get_cur_map()->set_trainer(world.player.get_pos_x(), world.player.get_pos_y(), NULL);
        world.player.set_pos(rand_range(1, MAP_X - 2), rand_range(1, MAP_Y - 2));
        } while (world.get_cur_map()->get_trainer(world.player.get_pos_x(), world.player.get_pos_y()) ||
                (move_cost[tr_pc][world.get_cur_map()->get_map_val(world.player.get_pos_x(), world.player.get_pos_y())] == INT_MAX) ||
                 world.rival_dist[world.player.get_pos_y()][world.player.get_pos_x()] < 0);
        world.get_cur_map()->set_trainer(world.player.get_pos_x(), world.player.get_pos_y(), &world.player);
        pathfind(world.get_cur_map());
    }
    
    place_trainers();

    return 0;
}