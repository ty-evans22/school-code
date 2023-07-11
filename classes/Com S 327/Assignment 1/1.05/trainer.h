#ifndef TRAINER_H
#define TRAINER_H

#include <stdint.h>

#define MAX_NUM_TRAINERS         100  // prevent overcrowding
#define DEFAULT_NUM_TRAINERS     10

typedef enum __attribute__ ((__packed__)) movement_type {
    move_hiker,
    move_rival,
    move_pacer,
    move_wanderer,
    move_stationary,
    move_walker,
    move_player,
    num_movement_types
} movement_type_t;

typedef enum __attribute__ ((__packed__)) trainer_type {
    tr_player,
    tr_hiker,
    tr_rival,
    tr_other,
    num_trainer_types
} trainer_type_t;

uint8_t *gen_num_tr_types(uint8_t num_trainers);

#endif