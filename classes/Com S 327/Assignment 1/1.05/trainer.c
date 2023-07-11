#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "trainer.h"
#include "world.h"

uint8_t *gen_num_tr_types(uint8_t num_trainers) {
    static uint8_t num_tr_types[] = {0, 0, 0, 0, 0, 0};
    uint8_t curr_num = 0; // keep running count of number of trainers left to assign to a type

    // error check as always -- safety first :) -- code would still work if num_trainers = 0 but we always want
    // to spawn at least 1 trainer anyways
    if (num_trainers > 0) {
        /* Generate at least 1 of each trainer type as long as the total number of trainers permits */
        if (num_trainers < num_trainer_types) {
            for (int i = 0; i < num_trainers; i++) {
                num_tr_types[i] = 1;
            }
        } else {
            for (int i = tr_hiker; i < num_trainer_types; i++) {
                // in future update formula so each trainer type has at least floor(num_trainers / num_trainer_types) or
                // something along those lines to produce more even spreading of trainer types
                num_tr_types[i - 1] = (rand() % (num_trainers - ((num_trainer_types - 1) - i) - curr_num)) + 1;
                curr_num += num_tr_types[i - 1];
            }
            while (num_trainers - curr_num > 0) {
                for (int i = 0; i < num_trainers - curr_num && i < num_trainer_types - 1; i++) {
                    num_tr_types[i]++;
                    curr_num++;
                }
            }
        }
    } else {
        printf("\nERROR: num_trainers must be greater than 0. Proceeding with default number of trainers.");
        return gen_num_tr_types(DEFAULT_NUM_TRAINERS);
    }

    return num_tr_types;
}