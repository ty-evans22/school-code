#include <cmath>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "pokemon.h"
#include "db_parse.h"
#include "world.h"

#define VERSION_GROUP_ID            20
#define SECONDARY_VERSION_GROUP     18
#define SHINY_PROB                  8192
#define POKEMON_OFFSET              9102
#define MOVE_OFFSET                 9174

Move *move_list[NUM_MOVES + 1];
move_node_t *pokemon_move_list[NUM_POKE_SPECIES + 1];
std::vector<Move*> poke_move_list[NUM_POKE_SPECIES + 1];

void init_poke_gen() {
    int i;

    /* create easily accessible list of moves */
    for (i = 1; i <= NUM_MOVES; i++) {
        Move *m;
        m = new Move;
        m->move_id = moves[i].id;
        strcpy(m->name, moves[i].identifier);
        m->generation_id = moves[i].generation_id;
        m->type_id = moves[i].type_id;
        m->power = moves[i].power;
        m->pp = moves[i].pp;
        m->accuracy = moves[i].accuracy;
        m->priority = moves[i].priority;
        m->target_id = moves[i].target_id;
        m->damage_class_id = moves[i].damage_class_id;
        m->effect_id = moves[i].effect_id;
        m->effect_chance = moves[i].effect_chance;
        move_list[i] = m;
    }

    /* add valid moves from the overall move list to the pokemon move list */
    for (i = 1; i < 528239; i++) {
        if (pokemon_moves[i].version_group_id == VERSION_GROUP_ID && pokemon_moves[i].pokemon_move_method_id == 1) {
            int s_offset = pokemon_moves[i].pokemon_id > 898 ? POKEMON_OFFSET : 0;
            int s_id = pokemon[pokemon_moves[i].pokemon_id - s_offset].species_id;
            int m_offset = pokemon_moves[i].move_id > 826 ? MOVE_OFFSET : 0;
            int m_id = pokemon_moves[i].move_id - m_offset;

            /* add moves to the list with the lowest level requirements listed first */
            int min_idx = 0;
            for (uint16_t g = 0; g < poke_move_list[s_id].size(); g++) {
                if (poke_move_list[s_id].at(g)->level >= pokemon_moves[i].level) {
                    break;
                }
                min_idx++;
            }
            poke_move_list[s_id].insert(poke_move_list[s_id].begin() + min_idx, move_list[m_id]);
            poke_move_list[s_id].at(min_idx)->level = pokemon_moves[i].level;
        }
    }

    /* go back through with the secondary version_group and add any moves that aren't already in the list */
    for (i = 1; i < 528239; i++) {
        if (pokemon_moves[i].version_group_id == SECONDARY_VERSION_GROUP && pokemon_moves[i].pokemon_move_method_id == 1) {
            int s_offset = pokemon_moves[i].pokemon_id > 898 ? POKEMON_OFFSET : 0;
            int s_id = pokemon[pokemon_moves[i].pokemon_id - s_offset].species_id;
            int m_offset = pokemon_moves[i].move_id > 826 ? MOVE_OFFSET : 0;
            int m_id = pokemon_moves[i].move_id - m_offset;

            /* make sure we're not adding duplicate moves */
            if (!poke_move_list[s_id].empty()) {
                bool c = false;
                for (uint16_t j = 0; j < poke_move_list[s_id].size(); j++) {
                    if (poke_move_list[s_id].at(j)->move_id == move_list[m_id]->move_id) {
                        c = true;
                    }
                }
                if (c) {
                    continue;
                }
            }

            /* add moves to the list with the lowest level requirements listed first */
            int min_idx = 0;
            for (uint16_t g = 0; g < poke_move_list[s_id].size(); g++) {
                if (poke_move_list[s_id].at(g)->level >= pokemon_moves[i].level) {
                    break;
                }
                min_idx++;
            }
            poke_move_list[s_id].insert(poke_move_list[s_id].begin() + min_idx, move_list[m_id]);
            poke_move_list[s_id].at(min_idx)->level = pokemon_moves[i].level;
        }
    }
}

Pokemon *generate_random_pokemon() {
    Pokemon *p;
    p = new Pokemon;

    /* generate the species id (1-898) */
    p->set_species_id((rand() % NUM_POKE_SPECIES) + 1);

    /* retrieve proper name */
    p->set_name(species[p->get_species_id()].identifier);

    /* generate level */
    int dist = abs(world.get_cur_idx_x() - (WORLD_SIZE / 2)) + abs(world.get_cur_idx_y() - (WORLD_SIZE / 2));
    if (dist <= 200) {
        /* min. lvl = 1 , max lvl = (dist / 2) */
        if (dist > 0) {
            p->set_level((rand() % (dist / 2)) + 1);
        } else {
            p->set_level(1);
        }
    } else {
        /* min. lvl = (dist - 200) / 2 , max lvl = 100 */
        int range = 100 - ((dist - 200) / 2) + 1;
        p->set_level((rand() % range) + 1);
    }

    /* generate ivs */
    for (int i = 0; i < num_stat_types; i++) {
        p->set_iv(i, rand() % 16);
    }

    /* generate stats - need to add a catch for species id needing to be offset */
    p->set_stat(stat_hp, floor((((pokemon_stats[((p->get_species_id() - 1) * 6) + 1].base_stat + p->get_iv(stat_hp)) * 2)
                * p->get_level()) / 100) + p->get_level() + 10);
    for (int i = 1; i < num_stat_types; i++) {
        p->set_stat(i, floor((((pokemon_stats[i + ((p->get_species_id() - 1) * 6) + 1].base_stat + p->get_iv(i)) * 2)
                * p->get_level()) / 100) + 5);
    }

    /* generate move set */
    int max_idx = 0;
    for (uint16_t i = 0; i < poke_move_list[p->get_species_id()].size(); i++) {
        if (poke_move_list[p->get_species_id()].at(i)->level > p->get_level()) {
            break;
        }
        max_idx++;
    }
    switch (max_idx) {
        case 0:
            /* no moves exist for the current pokemon - will fix in the future */
            p->set_move(0, NULL);
            p->set_move(1, NULL);
            break;
        case 1:
            /* only one move exists for the current pokemon - might fix in the future */
            p->set_move(0, poke_move_list[p->get_species_id()].at(0));
            p->set_move(1, NULL);
            break;
        default:
            int f, s;
            do {
                f = rand() % max_idx;
                s = rand() % max_idx;
            } while (f == s);
            p->set_move(0, poke_move_list[p->get_species_id()].at(f));
            p->set_move(1, poke_move_list[p->get_species_id()].at(s));
            break;
    }

    /* generate gender */
    p->set_gender(rand() % 10 < 5);

    /* generate shininess */
    if (rand() % SHINY_PROB == 0) {
        p->is_shiny = true;
    } else {
        p->is_shiny = false;
    }

    return p;
}