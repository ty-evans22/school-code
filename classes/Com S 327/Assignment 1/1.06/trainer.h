#ifndef TRAINER_H
#define TRAINER_H

#include <stdint.h>

#include "world.h"

typedef enum __attribute__ ((__packed__)) movement_type {
    move_hiker,
    move_rival,
    move_pace,
    move_wander,
    move_sentry,
    move_walk,
    move_pc,
    num_movement_types
} movement_type_t;

extern const char *tr_type_name[num_trainer_types]; /* makes tr_type_name array accessible throughout the program */

extern int32_t move_cost[num_trainer_types][num_terrain_types]; /* makes move_cost array accessible throughout the program */

/* Needed to slightly modify the extern declaration of move_func here in order for it to work */
/* Not 100% sure exactly what's going on here, but my understanding is that it is an array of functions */
typedef void (*move_function)(trainer_t *, pair_t);
extern move_function move_func[num_movement_types]; /* makes move_func accessible throughout the program */

/* Object representation of an NPC -- derived from the trainer_t class */
class npc_t : public trainer_t {
    private:
        movement_type_t mtype;
        pair_t dir;

    public:
        npc_t(movement_type_t m); // need to figure out how to also use base constructor (super?) - set mtype
        /* might need to make a copy constructor/assignment operator function */
        virtual ~npc_t();

        bool is_defeated;

        movement_type_t get_mtype() {return mtype;}
        int get_dir_x() {return dir[dim_x];}
        int get_dir_y() {return dir[dim_y];}

        void set_mtype(movement_type_t m) {this->mtype = m;}
        void set_dir(int x, int y) {this->dir[dim_x] = x; this->dir[dim_y] = y;}
        void set_dir(pair_t new_dir) {this->dir[dim_x] = new_dir[dim_x]; this->dir[dim_y] = new_dir[dim_y];}
};

class pc_t : public trainer_t {
    public:
        pc_t(); // need to figure out how to also use base constructor (super?) - world.pc.pc = malloc(sizeof (*world.pc.pc));
        /* might need to make a copy constructor/assignment operator function and/or a destructor */
        // include copy constructor & assignment = operator function
        virtual ~pc_t();

        void set_curr_time(int new_time) {this->curr_time = new_time;}
};

#endif