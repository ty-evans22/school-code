#ifndef POKEMON_H
#define POKEMON_H

#include <string.h>

#define POKEMON_SPAWN_PROB      10
#define NUM_POKE_SPECIES        898
#define NUM_MOVES               844

typedef enum __attribute__ ((__packed__)) stats_type {
    stat_hp,
    stat_attack,
    stat_defense,
    stat_sp_attack,
    stat_sp_defense,
    stat_speed,
    // stat_accuracy,
    // stat_evasion,
    num_stat_types
} stats_type_t;

/* need to update the access modifiers and create getters/setters */
class Move {
    public:
        int move_id;
        char name[30];
        int generation_id;
        int type_id;
        int power;
        int pp;
        int accuracy;
        int priority;
        int target_id;
        int damage_class_id;
        int effect_id;
        int effect_chance;
        int level;
};

typedef struct move_node {
    struct move_node *prev;
    struct move_node *next;
    Move *move;
    int count; /* only update count in the head node */
} move_node_t;

extern Move *move_list[NUM_MOVES + 1];
extern move_node_t *pokemon_move_list[NUM_POKE_SPECIES + 1];

class Pokemon {
    private:
        int species_id;
        char name[30];
        int level;
        int stats[num_stat_types];
        int individual_vals[num_stat_types];
        Move *move_set[2];
        bool is_male;

    public:
        bool is_shiny;

        int get_species_id() {return species_id;}
        int get_level() {return level;}
        int get_stat(stats_type_t stat_type) {return stats[stat_type];}
        int get_stat(int idx) {return stats[idx];}
        int get_iv(stats_type_t stat_type) {return individual_vals[stat_type];}
        int get_iv(int idx) {return individual_vals[idx];}
        char* get_name() {return name;}
        Move** get_moves() {return move_set;}
        Move* get_move(int idx) {return move_set[idx];} /* does not error check that move_set[idx] exists */
        char get_gender() {return is_male ? 'm' : 'f';}

        void set_species_id(int id) {species_id = id;}
        void set_name(char new_name[30]) {strcpy(name, new_name);}
        void set_level(int new_level) {level = new_level;}
        void set_stats(int stat_list[num_stat_types]) {for (int i = 0; i < num_stat_types; i++) stats[i] = stat_list[i];}
        void set_stat(stats_type_t stat_type, int value) {stats[stat_type] = value;}
        void set_stat(int idx, int value) {stats[idx] = value;}
        void set_ivs(int iv_list[num_stat_types]) {for (int i = 0; i < num_stat_types; i++) individual_vals[i] = iv_list[i];}
        void set_iv(stats_type_t stat_type, int value) {individual_vals[stat_type] = value;}
        void set_iv(int idx, int value) {individual_vals[idx] = value;}
        void set_move(int idx, Move *move) {move_set[idx] = move;}
        void set_gender(char gender) {(gender == 'm' || gender == 'M') ? is_male = true : is_male = false;}
        void set_gender(bool is_male) {this->is_male = is_male;}
};

void init_poke_gen();
Pokemon *generate_random_pokemon();

#endif