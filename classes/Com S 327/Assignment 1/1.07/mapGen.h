#ifndef MAPGEN_H
#define MAPGEN_H

#include <assert.h>

#include "world.h"

/* might be able to move this to mapGen.cpp */
#define malloc(size) ({            \
    void *_tmp;                    \
    assert((_tmp = malloc(size))); \
    _tmp;                          \
})

int32_t trainer_cmp(const void *key, const void *with);

int new_map(int teleport);

#endif