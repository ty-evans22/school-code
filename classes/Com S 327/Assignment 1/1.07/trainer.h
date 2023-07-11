#ifndef TRAINER_H
#define TRAINER_H

#include <stdint.h>

#include "world.h"

class Map;

typedef int16_t pair_t[2];

class Trainer;

/* Trainer is defined in world.h to allow an instance of character
 * in world without including trainer.h in world.h                 */

extern void (*move_func[num_movement_types])(Trainer *, pair_t);

int32_t cmp_tr_turns(const void *key, const void *with);
void delete_trainer(void *v);
void pathfind(Map *m);

int player_move(char);

#endif