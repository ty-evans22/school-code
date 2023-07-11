#include <stdlib.h>
#include <string.h>

#include "world.h"
#include "trainer.h"
#include "mapGen.h"

world_t world;  /* declares the global world variable */

/*** DEFINITIONS FOR trainer_t CLASS ***/

/* trainer_t constructor */
trainer_t::trainer_t() {
    curr_time = 0;
}

/* copy constructor */
trainer_t::trainer_t(const trainer_t &c) {
    this->ttype = c.ttype;
    this->pos[dim_x] = c.pos[dim_x];
    this->pos[dim_y] = c.pos[dim_y];
    this->symbol = c.symbol;
    this->curr_time = c.curr_time;
}

/* assignment operator = overload */
trainer_t &trainer_t::operator=(const trainer_t &c) {
    /* guard against self-assignment */
    if (this == &c) {
        return *this;
    }

    this->ttype = c.ttype;
    this->pos[dim_x] = c.pos[dim_x];
    this->pos[dim_y] = c.pos[dim_y];
    this->symbol = c.symbol;
    this->curr_time = c.curr_time;

    return *this;
}

// /* destructor */
// trainer_t::~trainer_t() {
    
// }

bool trainer_t::is_pc() {
    return ttype == tr_pc ? true : false;
}

/*** DEFINITIONS FOR map_t CLASS ***/

/* map_t constructor */
map_t::map_t() {
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
}

/* copy constructor */
map_t::map_t(const map_t &c) {
    this->num_trainers = c.num_trainers;
    this->n = c.n;
    this->s = c.s;
    this->e = c.e;
    this->w = c.w;

    /* copy the heap and arrays */
    heap_init(&this->queue, trainer_cmp, delete_trainer);
    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_Y; x++) {
            this->map[y][x] = c.map[y][x];
            this->height[y][x] = c.map[y][x];
            if (c.tmap[y][x] == NULL) {
                this->tmap[y][x] = NULL;
            } else {
                this->tmap[y][x] = c.tmap[y][x];
                heap_insert(&this->queue, this->tmap[y][x]);
            }
        }
    }
}

/* assignment operator = overload */
map_t &map_t::operator=(const map_t &c) {
    /* guard against self-assignment */
    if (this == &c) {
        return *this;
    }

    this->num_trainers = c.num_trainers;
    this->n = c.n;
    this->s = c.s;
    this->e = c.e;
    this->w = c.w;

    /* copy the heap and arrays */
    heap_init(&this->queue, trainer_cmp, delete_trainer);
    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_Y; x++) {
            this->map[y][x] = c.map[y][x];
            this->height[y][x] = c.map[y][x];
            if (c.tmap[y][x] == NULL) {
                this->tmap[y][x] = NULL;
            } else {
                this->tmap[y][x] = c.tmap[y][x];
                heap_insert(&this->queue, this->tmap[y][x]);
            }
        }
    }

    return *this;
}

/* destructor */
map_t::~map_t() {
    heap_delete(&queue);

    /* free trainers */
    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            delete(this->tmap[y][x]);
        }
    }

    free(this);
}

void map_t::memset_map() {
    memset(&this->map, 0, sizeof(this->map));
}

/*** DEFINITIONS FOR world_t CLASS ***/

/* world_t constructor */
world_t::world_t() {
    cur_idx[dim_x] = cur_idx[dim_y] = WORLD_SIZE / 2;
    quit = false;
    player = *new pc_t();

    /* initialize all maps other than starting map to NULL */
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            if (x != cur_idx[dim_x] || y != cur_idx[dim_y]) {
                world[y][x] = NULL;
            } else {
                new_map(0);
            }
        }
    }
}

/* copy constructor */
world_t::world_t(const world_t& c) {
    this->cur_idx[dim_x] = c.cur_idx[dim_x];
    this->cur_idx[dim_y] = c.cur_idx[dim_y];
    this->quit = c.quit;
    this->player = c.player;
    this->cur_map = c.cur_map;

    /* perform a deep copy of the distance maps */
    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            this->hiker_dist[y][x] = c.hiker_dist[y][x];
            this->rival_dist[y][x] = c.rival_dist[y][x];
        }
    }

    /* perform a deep copy of the world array */
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            this->world[y][x] = c.world[y][x];
        }
    }
}

/* assignment operator = overload */
world_t &world_t::operator=(const world_t &c) {
    /* guard against self-assignment */
    if (this == &c) {
        return *this;
    }

    this->cur_idx[dim_x] = c.cur_idx[dim_x];
    this->cur_idx[dim_y] = c.cur_idx[dim_y];
    this->quit = c.quit;
    this->player = c.player;
    this->cur_map = c.cur_map;

    /* perform a deep copy of the distance maps */
    for (int y = 0; y < MAP_Y; y++) {
        for (int x = 0; x < MAP_X; x++) {
            this->hiker_dist[y][x] = c.hiker_dist[y][x];
            this->rival_dist[y][x] = c.rival_dist[y][x];
        }
    }

    /* perform a deep copy of the world array */
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            this->world[y][x] = c.world[y][x];
        }
    }

    return *this;
}

/* destructor */
world_t::~world_t() {
    /* free world array */
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            if (this->world[y][x]) {
                delete(this->world[y][x]);
                this->world[y][x] = NULL;
            }
        }
    }
}

/* function called upon switching maps -- auto updates cur_idx and calls new_map */
void world_t::leave_map(pair_t d) {
    if (d[dim_x] == 0) {
        cur_idx[dim_x]--;
    } else if (d[dim_y] == 0) {
        cur_idx[dim_y]--;
    } else if (d[dim_x] == MAP_X - 1) {
        cur_idx[dim_x]++;
    } else {
        cur_idx[dim_y]++;
    }
    new_map(0);
}

void init_world() {
    /* initialize tmp world and copy all values to actual world */
    world_t *tmp = new world_t(); // need to delete tmp once finished
    world = *tmp;
    delete(tmp);
}

void delete_world() {
    for (int y = 0; y < WORLD_SIZE; y++) {
        for (int x = 0; x < WORLD_SIZE; x++) {
            if (world.get_map(x, y)) {
                delete(world.get_map(x, y));
                world.set_map(x, y, NULL);
            }
        }
    }
}