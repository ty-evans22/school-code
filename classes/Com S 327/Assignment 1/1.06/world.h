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
typedef int pair_t[num_dims];

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

typedef enum __attribute__ ((__packed__)) trainer_type {
    tr_pc,
    tr_hiker,
    tr_rival,
    tr_other,
    num_trainer_types
} trainer_type_t;

/* Object representation of a trainer - serves as base class for NPCs and the PC */
class trainer_t {
    protected:
        trainer_type_t ttype;
        pair_t pos;
        char symbol;
        int curr_time;

    public:
        bool is_pc();

        trainer_t();    /* default constructor */ // t = (npc_t*)malloc(sizeof (*t)); << include in constructor
        // set symbol based on tr_type
        trainer_t(const trainer_t &c); /* copy constructor */
        trainer_t &operator=(const trainer_t &c); /* assignment operator */
        virtual ~trainer_t() {/* do cleanup stuff here */}   /* destructor */

        trainer_type_t get_tr_type() {return ttype;}
        pair_t *get_pos() {return &pos;}
        int get_pos_x() {return pos[dim_x];}
        int get_pos_y() {return pos[dim_y];}
        char get_symbol() {return symbol;}
        int get_curr_time() {return curr_time;}

        void set_pos(int x, int y) {pos[dim_x] = x; pos[dim_y] = y;}
        void set_pos(pair_t new_pos) {pos[dim_x] = new_pos[dim_x]; pos[dim_y] = new_pos[dim_y];}
        void set_pos_x(int x) {pos[dim_x] = x;}
        void set_pos_y(int y) {pos[dim_y] = y;}
        void add_cost(int cost) {curr_time += cost;}
};

/* Object representation of a single MAP_Y x MAP_X map in the world */
class map_t {
    private:
        terrain_type_t map[MAP_Y][MAP_X];
        uint8_t height[MAP_Y][MAP_X];
        trainer_t *tmap[MAP_Y][MAP_X];
        uint32_t num_trainers;
        uint8_t n, s, e, w; /* coords of each exit */

    public:
        heap_t queue;   /* priority queue for trainer movement */

        map_t();    /* default constructor */
        map_t(const map_t &c); /* copy constructor */
        map_t &operator=(const map_t &c); /* assignment operator */
        ~map_t();   /* destructor */

        void memset_map(); // memset(&m->map, 0, sizeof (m->map));

        terrain_type_t get_map_val(int x, int y) {return map[y][x];}
        terrain_type_t get_map_val(pair_t pos) {return map[pos[dim_y]][pos[dim_x]];}
        uint8_t get_height(int x, int y) {return height[y][x];}
        uint8_t get_height(pair_t pos) {return height[pos[dim_y]][pos[dim_x]];}
        trainer_t *get_trainer(int x, int y) {return tmap[y][x];}
        trainer_t *get_trainer(pair_t pos) {return tmap[pos[dim_y]][pos[dim_x]];}
        uint32_t get_num_trainers() {return num_trainers;}
        uint8_t get_north_exit() {return n;}
        uint8_t get_south_exit() {return s;}
        uint8_t get_east_exit() {return e;}
        uint8_t get_west_exit() {return w;}


        void set_map_val(int x, int y, terrain_type_t t) {map[y][x] = t;}
        void set_map_val(pair_t pos, terrain_type_t t) {map[pos[dim_y]][pos[dim_x]] = t;}
        void set_trainer(int x, int y, trainer_t *t) {tmap[y][x] = t;}
        void set_trainer(pair_t pos, trainer_t *t) {tmap[pos[dim_y]][pos[dim_x]] = t;}
        void set_height(int x, int y, uint8_t height_val) {height[y][x] = height_val;}
        void set_num_trainers(uint32_t num) {num_trainers = num;}
};

/* Object representation of the world */
class world_t {
    private:
        map_t *world[WORLD_SIZE][WORLD_SIZE];
        pair_t cur_idx;
        map_t *cur_map;

    public:
        trainer_t player;
        bool quit;
        int hiker_dist[MAP_Y][MAP_X];   /* hiker distance map */
        int rival_dist[MAP_Y][MAP_X];   /* rival distance map */

        world_t();  /* default constructor */
        world_t &operator=(const world_t &c); /* assignment operator */
        world_t(const world_t &c); /* copy constructor */
        ~world_t();   /* destructor */

        void leave_map(pair_t d);

        map_t *get_map(int x, int y) {return this->world[y][x];}
        map_t *get_map(pair_t pos) {return this->world[pos[dim_y]][pos[dim_x]];}
        map_t *get_cur_map() {return cur_map;}
        pair_t *get_cur_idx() {return &cur_idx;}
        int get_cur_idx_x() {return cur_idx[dim_x];}
        int get_cur_idx_y() {return cur_idx[dim_y];}

        void set_map(pair_t pos, map_t *m) {this->world[pos[dim_y]][pos[dim_x]] = m;}
        void set_map(int x, int y, map_t *m) {this->world[y][x] = m;}
        void set_cur_idx(int x, int y) {cur_idx[dim_x] = x; cur_idx[dim_y] = y;}
        void set_cur_map(map_t *m) {cur_map = m;}
};

extern world_t world;   /* makes the world accessible throughout the program */

void init_world();
void delete_world();

#endif