#ifndef TRAINER_H
#define TRAINER_H

#include <stdint.h>

#define MAX_NUM_TRAINERS         100  // prevent overcrowding
#define DEFAULT_NUM_TRAINERS     10

/* Store the trainer types as enums to easily add more types later */
typedef enum __attribute__ ((__packed__)) trainer_type {
    tr_player,
    tr_hiker,
    tr_rival,
    tr_pacer,
    tr_wanderer,
    tr_stationary,
    tr_rand_walker,
    num_trainer_types
} trainer_type_t;

void print_hiker_dist();
void print_rival_dist();
// void init_trainer(); // initializes a trainer
uint8_t *gen_num_tr_types(uint8_t num_trainers);

#endif