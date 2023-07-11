#include <stdlib.h>
#include <string.h>

#include "world.h"
#include "mapGen.h"

World world;

/* declare the global all_dirs array */
pair_t all_dirs[8] = {
  { -1, -1 },
  { -1,  0 },
  { -1,  1 },
  {  0, -1 },
  {  0,  1 },
  {  1, -1 },
  {  1,  0 },
  {  1,  1 },
};

void Map::memset_map() {
    memset(&this->map, 0, sizeof(this->map));
}

void World::leave_map(pair_t d) {
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

void rand_dir(NPC *n) {
    int _i = rand() & 0x7;
    n->set_dir(all_dirs[_i][dim_x], all_dirs[_i][dim_y]);
}

void init_world() {
    world.quit = false;
    world.set_cur_idx(WORLD_SIZE / 2, WORLD_SIZE / 2);
    new_map(0);
}

void delete_world() {
    int x, y;

    for (y = 0; y < WORLD_SIZE; y++) {
        for (x = 0; x < WORLD_SIZE; x++) {
            if (world.get_map(x, y)) {
                heap_delete(&world.get_map(x, y)->queue);
                free(world.get_map(x, y));
                world.set_map(x, y, NULL);
            }
        }
    }
}