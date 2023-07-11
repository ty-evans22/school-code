#ifndef MAPGEN_H
#define MAPGEN_H

#include <assert.h>

#include "world.h"
#include "trainer.h"

/* Utility functions for map generation */
#define mappair(pair) (m->get_map_val(pair))
#define mapxy(x, y) (m->get_map_val(x, y))
#define heightpair(pair) (m->get_height(pair))
#define heightxy(x, y) (m->get_height(x, y))

/* might be able to move this to mapGen.cpp */
#define malloc(size) ({            \
    void *_tmp;                    \
    assert((_tmp = malloc(size))); \
    _tmp;                          \
})

extern pair_t all_dirs[8];  /* makes the all_dirs array accessible throughout the program */

int32_t trainer_cmp(const void *key, const void *with);

void delete_trainer(void *v);
void rand_dir(npc_t *t);
int new_map(int teleport);

#endif