#ifndef WORLD_H
#define WORLD_H

#include <stdint.h>

#include "heap.h"

#define MAP_X              80
#define MAP_Y              21
#define WORLD_SIZE         401

/* Returns random integer in [min, max]. */
#define rand_range(min, max) ((rand() % (((max) + 1) - (min))) + (min))

/* Utility enum used for quick and easy access to specific dimensions in coordinates */
typedef enum dim {
    dim_x,
    dim_y,
    num_dims
} dim_t;

/* Utility type used to store coordinates - could possibly be replaced w/ a class */
typedef int16_t pair_t[2];

typedef enum __attribute__ ((__packed__)) terrain_type {
    ter_boulder,
    ter_tree,
    ter_path,
    ter_mart,
    ter_center,
    ter_grass,
    ter_clearing,
    ter_mountain,
    ter_forest,
    ter_exit,
    num_terrain_types
} terrain_type_t;

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

typedef enum __attribute__ ((__packed__)) trainer_type {
    tr_pc,
    tr_hiker,
    tr_rival,
    tr_other,
    num_trainer_types
} trainer_type_t;

class Trainer;

class Map {
    private:
        terrain_type_t map[MAP_Y][MAP_X];
        uint8_t height[MAP_Y][MAP_X];
        Trainer *tmap[MAP_Y][MAP_X];
        uint32_t num_trainers;
        uint8_t n, s, e, w; /* coords of each exit */

    public:
        heap_t queue;   /* priority queue for trainer movement */

        void memset_map();

        terrain_type_t get_map_val(int x, int y) {return map[y][x];}
        terrain_type_t get_map_val(pair_t pos) {return map[pos[dim_y]][pos[dim_x]];}
        uint8_t get_height(int x, int y) {return height[y][x];}
        uint8_t get_height(pair_t pos) {return height[pos[dim_y]][pos[dim_x]];}
        Trainer *get_trainer(int x, int y) {return tmap[y][x];}
        Trainer *get_trainer(pair_t pos) {return tmap[pos[dim_y]][pos[dim_x]];}
        uint32_t get_num_trainers() {return num_trainers;}
        uint8_t get_north_exit() {return n;}
        uint8_t get_south_exit() {return s;}
        uint8_t get_east_exit() {return e;}
        uint8_t get_west_exit() {return w;}


        void set_map_val(int x, int y, terrain_type_t t) {map[y][x] = t;}
        void set_map_val(pair_t pos, terrain_type_t t) {map[pos[dim_y]][pos[dim_x]] = t;}
        void set_trainer(int x, int y, Trainer *t) {tmap[y][x] = t;}
        void set_trainer(pair_t pos, Trainer *t) {tmap[pos[dim_y]][pos[dim_x]] = t;}
        void set_height(int x, int y, uint8_t height_val) {height[y][x] = height_val;}
        void set_num_trainers(uint32_t num) {num_trainers = num;}
        void set_north_exit(uint8_t n) {this->n = n;}
        void set_south_exit(uint8_t s) {this->s = s;}
        void set_west_exit(uint8_t w) {this->w = w;}
        void set_east_exit(uint8_t e) {this->e = e;}
};

/* Here instead of trainer.h to avoid including trainer.h */
class Trainer {
    protected:
        pair_t pos;
        char symbol;
        int curr_time;

    public:
        virtual ~Trainer() {}

        const int get_pos_x() {return pos[dim_x];}
        const int get_pos_y() {return pos[dim_y];}
        char get_symbol() {return symbol;}
        int get_curr_time() {return curr_time;}

        void set_pos(int x, int y) {pos[dim_x] = x; pos[dim_y] = y;}
        void set_pos(pair_t new_pos) {pos[dim_x] = new_pos[dim_x]; pos[dim_y] = new_pos[dim_y];}
        void set_pos_x(int x) {pos[dim_x] = x;}
        void set_pos_y(int y) {pos[dim_y] = y;}
        void set_symbol(char s) {symbol = s;}
        void set_curr_time(int new_time) {curr_time = new_time;}
        void add_cost(int cost) {curr_time += cost;}
};

class PC : public Trainer {
    public:
};

class NPC : public Trainer {
    private:
        trainer_type_t ttype;
        movement_type_t mtype;
        pair_t dir;

    public:
        bool is_defeated;

        trainer_type_t get_ttype() {return ttype;}
        movement_type_t get_mtype() {return mtype;}
        int get_dir_x() {return dir[dim_x];}
        int get_dir_y() {return dir[dim_y];}

        void set_ttype(trainer_type_t t) {this->ttype = t;}
        void set_mtype(movement_type_t m) {this->mtype = m;}
        void set_dir(int x, int y) {this->dir[dim_x] = x; this->dir[dim_y] = y;}
        void set_dir(pair_t new_dir) {this->dir[dim_x] = new_dir[dim_x]; this->dir[dim_y] = new_dir[dim_y];}
};

class World {
    private:
        Map *world[WORLD_SIZE][WORLD_SIZE];
        pair_t cur_idx;
        Map *cur_map;

    public:
        PC player;
        bool quit;
        /* Place distance maps in world, not map, since *
         * we only need one pair at any given time.     */
        int hiker_dist[MAP_Y][MAP_X];   /* hiker distance map */
        int rival_dist[MAP_Y][MAP_X];   /* rival distance map */

        void leave_map(pair_t d);

        Map *get_map(int x, int y) {return this->world[y][x];}
        Map *get_map(pair_t pos) {return this->world[pos[dim_y]][pos[dim_x]];}
        Map *get_cur_map() {return cur_map;}
        pair_t *get_cur_idx() {return &cur_idx;}
        int get_cur_idx_x() {return cur_idx[dim_x];}
        int get_cur_idx_y() {return cur_idx[dim_y];}

        void set_map(pair_t pos, Map *m) {this->world[pos[dim_y]][pos[dim_x]] = m;}
        void set_map(int x, int y, Map *m) {this->world[y][x] = m;}
        void set_cur_idx(int x, int y) {cur_idx[dim_x] = x; cur_idx[dim_y] = y;}
        void set_cur_map(Map *m) {cur_map = m;}
};

#define mapxy(x, y) (m->get_map_val(x, y))
#define heightpair(pair) (m->get_height(pair))
#define heightxy(x, y) (m->get_height(x, y))

extern const char *tr_type_name[num_trainer_types];
extern int32_t move_cost[num_trainer_types][num_terrain_types];

/* Even unallocated, a WORLD_SIZE x WORLD_SIZE array of pointers is a very *
 * large thing to put on the stack.  To avoid that, world is global.     */
extern World world;

extern pair_t all_dirs[8];

void rand_dir(NPC *n);
void init_world();
void delete_world();

#endif