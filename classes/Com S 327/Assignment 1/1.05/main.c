#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <curses.h>
#include <limits.h>
#include <string.h>
#include <math.h>

/** Tyler Evans **/

#include "world.h"
#include "mapGen.h"
#include "dijkstra.h"

//*********************************************************************************//
//                                                                                 //
// did not intend to have the majority of the code in here but it's just where it  //
// ended up, and it works so I can't complain. I will end up going through all of  //
// these files and code in a few days and restructure/organize it and clean it all //
// up for next week's assignment                                                   //
//                                                                                 //
//*********************************************************************************//

#define KEY_ESC             27
#define BASE_COST           15

#define PATH_PAIR           1
#define FOREST_PAIR         2
#define MART_PAIR           3
#define CENTER_PAIR         4
#define BOULDER_PAIR        5
#define GRASS_PAIR          6
#define PLAYER_PAIR         7
#define SEEKER_PAIR         8
#define OTHER_PAIR          9

#define rand_dir(dir) {             \
    int _i = rand() & 0x7;          \
    dir[0] = direction_vals[_i][0]; \
    dir[1] = direction_vals[_i][1]; \
}

static int32_t move_cost[num_trainer_types][num_terrain_types] = {
    { INT_MAX, INT_MAX, 10, 10,      10,      20, 10, INT_MAX, INT_MAX },
    { INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 15, 10, 15,      15      },
    { INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX },
    { INT_MAX, INT_MAX, 10, INT_MAX, INT_MAX, 20, 10, INT_MAX, INT_MAX },
};

char msg[80];

/* Print the current map to the terminal */
void print_cur_map() {
    int x, y;
    int default_reached = 0;

    /* Clear the current window */
    clear();

    /* Terminal will be 21x80 */

    /* Add the message to the top line of the window */
    mvprintw(0, 0, msg);
    addch('\n');

    /* Print char representation of every cell in the map */
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            if (world.curr_map->tmap[y][x]) {
                /* Set colors for each trainer type */
                if (world.curr_map->tmap[y][x]->npc) {
                    switch (world.curr_map->tmap[y][x]->npc->ttype) {
                        case tr_hiker:
                        case tr_rival:
                            attron(COLOR_PAIR(SEEKER_PAIR));
                            break;
                        case tr_other:
                            attron(COLOR_PAIR(OTHER_PAIR));
                            break;
                        default:
                            break;
                    }
                } else {
                    attron(COLOR_PAIR(PLAYER_PAIR));
                }
                addch(world.curr_map->tmap[y][x]->symbol);
                if (world.curr_map->tmap[y][x]->npc) {
                    switch (world.curr_map->tmap[y][x]->npc->ttype) {
                        case tr_hiker:
                        case tr_rival:
                            attroff(COLOR_PAIR(SEEKER_PAIR));
                            break;
                        case tr_other:
                            attroff(COLOR_PAIR(OTHER_PAIR));
                            break;
                        default:
                            break;
                    }
                } else {
                    attroff(COLOR_PAIR(PLAYER_PAIR));
                }
            } else {
                switch (world.curr_map->map[y][x]) {
                    case ter_boulder:
                    case ter_mountain:
                        attron(COLOR_PAIR(BOULDER_PAIR));
                        addch('%');
                        attroff(COLOR_PAIR(BOULDER_PAIR));
                        break;
                    case ter_tree:
                    case ter_forest:
                        attron(COLOR_PAIR(FOREST_PAIR));
                        addch('^');
                        attroff(COLOR_PAIR(FOREST_PAIR));
                        break;
                    case ter_path:
                        attron(COLOR_PAIR(PATH_PAIR));
                        addch('#');
                        attroff(COLOR_PAIR(PATH_PAIR));
                        break;
                    case ter_mart:
                        attron(COLOR_PAIR(MART_PAIR));
                        addch('M');
                        attroff(COLOR_PAIR(MART_PAIR));
                        break;
                    case ter_center:
                        attron(COLOR_PAIR(CENTER_PAIR));
                        addch('C');
                        attroff(COLOR_PAIR(CENTER_PAIR));
                        break;
                    case ter_grass:
                        attron(COLOR_PAIR(GRASS_PAIR));
                        addch(':');
                        attroff(COLOR_PAIR(GRASS_PAIR));
                        break;
                    case ter_clearing:
                        attron(COLOR_PAIR(GRASS_PAIR));
                        addch('.');
                        attroff(COLOR_PAIR(GRASS_PAIR));
                        break;
                    default:
                        default_reached = 1;
                        break;
                }
            }
        }
        addch('\n');
    }

    if (default_reached) {
        fprintf(stderr, "Default reached in %s\n", __FUNCTION__);
    }
}

void display_tr_list() {
    int x, y, i;
    clear();
    // not the most efficient implementation but I threw it together quickly and it works so I'll leave it for now

    /* Count the number of trainers */
    int count = 0;
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            if (world.curr_map->tmap[y][x] && world.curr_map->tmap[y][x] != &world.player) {
                count++;
            }
        }
    }

    /* Initialize trainer array */
    trainer_t *trainers[count];
    for (i = 0; i < count; i++) {
        trainers[i] = malloc(sizeof (*trainers[i]));
    }

    /* Populate trainer array */
    i = 0;
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            if (world.curr_map->tmap[y][x] && world.curr_map->tmap[y][x] != &world.player) {
                trainers[i] = world.curr_map->tmap[y][x];
                i++;
            }
        }
    }

    int curr_idx = 0;
    int exit = 0;
    strcpy(msg, "List of Trainers - Use UP and DOWN arrows to scroll - ESC to exit");
    mvprintw(0, 0, msg);
    while (!exit) {
        /* Display the current page of trainers */
        for (i = curr_idx * 21; i < count; i++) {
            /* Calculate the relative distance to the player */
            int ns = world.player.pos[dim_y] - trainers[i]->pos[dim_y];
            int ew = world.player.pos[dim_x] - trainers[i]->pos[dim_x];
            // too lazy rn to come up with something more clever than a few if statements
            // possibly add status on whether or not they're defeated - or color representation
            if (ns >= 0 && ew >= 0) {
                mvprintw(i - (curr_idx * 21) + 1, 0, "%c, %d North and %d West", trainers[i]->symbol, ns, ew);
            } else if (ns < 0 && ew < 0) {
                mvprintw(i - (curr_idx * 21) + 1, 0, "%c, %d South and %d East", trainers[i]->symbol, abs(ns), abs(ew));
            } else if (ns >= 0 && ew < 0) {
                mvprintw(i - (curr_idx * 21) + 1, 0, "%c, %d North and %d East", trainers[i]->symbol, ns, abs(ew));
            } else if (ns < 0 && ew >= 0) {
                mvprintw(i - (curr_idx * 21) + 1, 0, "%c, %d South and %d West", trainers[i]->symbol, abs(ns), ew);
            }
        }

        /* Await input */
        int b = getch();

        switch (b) {
            case KEY_UP:
                if (curr_idx > 0) {
                    curr_idx--;
                }
                break;
            case KEY_DOWN:
                if (count > 21 + (21 * curr_idx)) {
                    curr_idx++;
                }
                break;
            case KEY_ESC:
                exit = 1;
                break;
            default:
                break;
        }
    }
    strcpy(msg, "");
}

void enter_pokemon_center() {
    /* Temporary placeholder */
    clear();
    int exit = 0;
    strcpy(msg, "Temporary Pokemon Center placeholder. Press '<' to exit.");
    mvprintw(0, 0, msg);
    while (!exit) {
        int s = getch();
        if (s == '<') {
            exit = 1;
        }
    }
    strcpy(msg, "To enter Pokemon Center press '>'.");
    print_cur_map();
}

void enter_pokemart() {
    /* Temporary placeholder */
    clear();
    int exit = 0;
    strcpy(msg, "Temporary Pokemart placeholder. Press '<' to exit.");
    mvprintw(0, 0, msg);
    while (!exit) {
        int s = getch();
        if (s == '<') {
            exit = 1;
        }
    }
    strcpy(msg, "To enter Pokemart press '>'.");
    print_cur_map();
}

void pokemon_battle() {
    /* Temporary placeholder */
    clear();
    int exit = 0;
    strcpy(msg, "Temporary battle placeholder. Press ESC to exit.");
    mvprintw(0, 0, msg);
    while (!exit) {
        int s = getch();
        if (s == KEY_ESC) {
            exit = 1;
        }
    }
    strcpy(msg, "Trainer defeated!");
    print_cur_map();
}

static void move_hiker_func(trainer_t *t, pair_t dest) {
    int min, base, i;

    base = rand() & 0x7;

    dest[dim_x] = t->pos[dim_x];
    dest[dim_y] = t->pos[dim_y];
    min = INT_MAX;

    /* Check if player is nearby */
    int p_dist = sqrt(pow(world.player.pos[dim_x] - t->pos[dim_x], 2.0) + pow(world.player.pos[dim_y] - t->pos[dim_y], 2.0));
    if (p_dist <= 1) {
        pokemon_battle();
        t->npc->mtype = move_walker;
        t->npc->defeated = 1;
        return;
    }

    for (i = base; i < 8 + base; i++) {
        // check that new pos isnt out of bounds
        if (t->pos[dim_y] + direction_vals[i & 0x7][dim_y] < 1 || t->pos[dim_y] + direction_vals[i & 0x7][dim_y] > MAP_Y - 2 ||
             t->pos[dim_x] + direction_vals[i & 0x7][dim_x] < 1 || t->pos[dim_x] + direction_vals[i & 0x7][dim_x] > MAP_X - 2) {
            continue;
        }
        if ((world.hiker_dist[t->pos[dim_y] + direction_vals[i & 0x7][dim_y]]
             [t->pos[dim_x] + direction_vals[i & 0x7][dim_x]] <= min) &&
             !world.curr_map->tmap[t->pos[dim_y] + direction_vals[i & 0x7][dim_y]]
             [t->pos[dim_x] + direction_vals[i & 0x7][dim_x]]) {
            dest[dim_x] = t->pos[dim_x] + direction_vals[i & 0x7][dim_x];
            dest[dim_y] = t->pos[dim_y] + direction_vals[i & 0x7][dim_y];
            min = world.hiker_dist[dest[dim_y]][dest[dim_x]];
        }
    }
}

static void move_rival_func(trainer_t *t, pair_t dest) {
    int min, base, i;

    base = rand() & 0x7;

    dest[dim_x] = t->pos[dim_x];
    dest[dim_y] = t->pos[dim_y];
    min = INT_MAX;

    /* Check if player is nearby */
    int p_dist = sqrt(pow(world.player.pos[dim_x] - t->pos[dim_x], 2.0) + pow(world.player.pos[dim_y] - t->pos[dim_y], 2.0));
    if (p_dist <= 1) {
        pokemon_battle();
        t->npc->mtype = move_walker;
        t->npc->defeated = 1;
        return;
    }

    for (i = base; i < 8 + base; i++) {
        if (t->pos[dim_y] + direction_vals[i & 0x7][dim_y] < 1 || t->pos[dim_y] + direction_vals[i & 0x7][dim_y] > MAP_Y - 2 ||
             t->pos[dim_x] + direction_vals[i & 0x7][dim_x] < 1 || t->pos[dim_x] + direction_vals[i & 0x7][dim_x] > MAP_X - 2) {
            continue;
        }
        if ((world.rival_dist[t->pos[dim_y] + direction_vals[i & 0x7][dim_y]]
             [t->pos[dim_x] + direction_vals[i & 0x7][dim_x]] < min) &&
             !world.curr_map->tmap[t->pos[dim_y] + direction_vals[i & 0x7][dim_y]]
             [t->pos[dim_x] + direction_vals[i & 0x7][dim_x]]) {
            dest[dim_x] = t->pos[dim_x] + direction_vals[i & 0x7][dim_x];
            dest[dim_y] = t->pos[dim_y] + direction_vals[i & 0x7][dim_y];
            min = world.rival_dist[dest[dim_y]][dest[dim_x]];
        }
    }
}

static void move_pacer_func(trainer_t *t, pair_t dest) {
    dest[dim_x] = t->pos[dim_x];
    dest[dim_y] = t->pos[dim_y];

    if (!(t->pos[dim_y] + t->npc->dir[dim_y] < 0 || t->pos[dim_y] + t->npc->dir[dim_y] > MAP_Y - 1 ||
         t->pos[dim_x] + t->npc->dir[dim_x] < 0 || t->pos[dim_x] + t->npc->dir[dim_x] > MAP_X - 1)) {
        /* Check for player */
        if (!t->npc->defeated && t->pos[dim_y] + t->npc->dir[dim_y] == world.player.pos[dim_y] && t->pos[dim_x] + t->npc->dir[dim_x] == world.player.pos[dim_x]) {
            pokemon_battle();
            t->npc->defeated = 1;
            return;
        }

        if ((world.curr_map->map[t->pos[dim_y] + t->npc->dir[dim_y]][t->pos[dim_x] + t->npc->dir[dim_x]] !=
             world.curr_map->map[t->pos[dim_y]][t->pos[dim_x]]) || world.curr_map->tmap[t->pos[dim_y] + t->npc->dir[dim_y]]
             [t->pos[dim_x] + t->npc->dir[dim_x]]) {
            t->npc->dir[dim_x] *= -1;
            t->npc->dir[dim_y] *= -1;
        }
    }

    /* Check for player one last time */
    if (!t->npc->defeated && t->pos[dim_y] + t->npc->dir[dim_y] == world.player.pos[dim_y] && t->pos[dim_x] + t->npc->dir[dim_x] == world.player.pos[dim_x]) {
        pokemon_battle();
        t->npc->defeated = 1;
        return;
    }

    if (!(t->pos[dim_y] + t->npc->dir[dim_y] < 1 || t->pos[dim_y] + t->npc->dir[dim_y] > MAP_Y - 2 ||
         t->pos[dim_x] + t->npc->dir[dim_x] < 1 || t->pos[dim_x] + t->npc->dir[dim_x] > MAP_X - 2) && 
         (world.curr_map->map[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]] ==
         world.curr_map->map[t->pos[dim_y]][t->pos[dim_x]]) &&
         !world.curr_map->tmap[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]]) {
        dest[dim_x] = t->pos[dim_x] + t->npc->dir[dim_x];
        dest[dim_y] = t->pos[dim_y] + t->npc->dir[dim_y];
    }
}

static void move_wanderer_func(trainer_t *t, pair_t dest) {
    dest[dim_x] = t->pos[dim_x];
    dest[dim_y] = t->pos[dim_y];

    /* Check for player */
    if (!t->npc->defeated && t->pos[dim_y] + t->npc->dir[dim_y] == world.player.pos[dim_y] && t->pos[dim_x] + t->npc->dir[dim_x] == world.player.pos[dim_x]) {
        pokemon_battle();
        t->npc->defeated = 1;
        return;
    }

    if ((world.curr_map->map[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]] !=
         world.curr_map->map[t->pos[dim_y]][t->pos[dim_x]]) ||
         world.curr_map->tmap[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]]) {
        rand_dir(t->npc->dir);
    }

    /* Check for player one last time */
    if (!t->npc->defeated && t->pos[dim_y] + t->npc->dir[dim_y] == world.player.pos[dim_y] && t->pos[dim_x] + t->npc->dir[dim_x] == world.player.pos[dim_x]) {
        pokemon_battle();
        t->npc->defeated = 1;
        return;
    }

    if ((world.curr_map->map[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]] ==
         world.curr_map->map[t->pos[dim_y]][t->pos[dim_x]]) &&
         !world.curr_map->tmap[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]]) {
        dest[dim_x] = t->pos[dim_x] + t->npc->dir[dim_x];
        dest[dim_y] = t->pos[dim_y] + t->npc->dir[dim_y];
    }
}

static void move_stationary_func(trainer_t *t, pair_t dest) {
    dest[dim_x] = t->pos[dim_x];
    dest[dim_y] = t->pos[dim_y];
}

static void move_walker_func(trainer_t *t, pair_t dest) {
    dest[dim_x] = t->pos[dim_x];
    dest[dim_y] = t->pos[dim_y];

    /* Check for player */
    if (!t->npc->defeated && t->pos[dim_y] + t->npc->dir[dim_y] == world.player.pos[dim_y] && t->pos[dim_x] + t->npc->dir[dim_x] == world.player.pos[dim_x]) {
        pokemon_battle();
        t->npc->defeated = 1;
        return;
    }

    if ((move_cost[tr_other][world.curr_map->map[t->pos[dim_y] +
         t->npc->dir[dim_y]][t->pos[dim_x] + t->npc->dir[dim_x]]] ==
         INT_MAX) || world.curr_map->tmap[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]]) {
        t->npc->dir[dim_x] *= -1;
        t->npc->dir[dim_y] *= -1;
    }

    /* Check for player one last time */
    if (!t->npc->defeated && t->pos[dim_y] + t->npc->dir[dim_y] == world.player.pos[dim_y] && t->pos[dim_x] + t->npc->dir[dim_x] == world.player.pos[dim_x]) {
        pokemon_battle();
        t->npc->defeated = 1;
        return;
    }

    if ((move_cost[tr_other][world.curr_map->map[t->pos[dim_y] +
         t->npc->dir[dim_y]][t->pos[dim_x] + t->npc->dir[dim_x]]] !=
         INT_MAX) && !world.curr_map->tmap[t->pos[dim_y] + t->npc->dir[dim_y]]
         [t->pos[dim_x] + t->npc->dir[dim_x]]) {
        dest[dim_x] = t->pos[dim_x] + t->npc->dir[dim_x];
        dest[dim_y] = t->pos[dim_y] + t->npc->dir[dim_y];
    }
}

static void move_pc_func(trainer_t *t, pair_t dest) {
    dest[dim_x] = t->pos[dim_x] + direction_vals[t->pc->dir][dim_x];
    dest[dim_y] = t->pos[dim_y] + direction_vals[t->pc->dir][dim_y];
}

void (*move_func[num_movement_types])(trainer_t *, pair_t) = {
    move_hiker_func,
    move_rival_func,
    move_pacer_func,
    move_wanderer_func,
    move_stationary_func,
    move_walker_func,
    move_pc_func,
};

void move_trainers() {
    trainer_t *t;
    pair_t d;

    do {
        t = heap_remove_min(&world.curr_map->queue);
        if (t == &world.player) {
            move_func[move_player](t, d);
            /* Check one last time for NPCs */
            if (world.curr_map->tmap[d[dim_y]][d[dim_x]] && world.curr_map->tmap[d[dim_y]][d[dim_x]]->npc
                 && !world.curr_map->tmap[d[dim_y]][d[dim_x]]->npc->defeated) {
                if (!world.curr_map->tmap[d[dim_y]][d[dim_x]]->npc->defeated) {
                    pokemon_battle();
                    world.curr_map->tmap[d[dim_y]][d[dim_x]]->npc->defeated = 1;
                    switch (world.curr_map->tmap[d[dim_y]][d[dim_x]]->npc->ttype) {
                        case tr_hiker:
                        case tr_rival:
                            world.curr_map->tmap[d[dim_y]][d[dim_x]]->npc->mtype = move_walker;
                        default:
                            break;
                    }
                }
                heap_insert(&world.curr_map->queue, t);
                continue;
            }

            /* Check if moving onto Pokemart or Pokemon Center */
            if (world.curr_map->map[d[dim_y]][d[dim_x]] == ter_mart) {
                strcpy(msg, "To enter Pokemart press '>'.");
                t->pc->poke_mart = 1;
                heap_insert(&world.curr_map->queue, t);
                continue;
            } else if (world.curr_map->map[d[dim_y]][d[dim_x]] == ter_center) {
                strcpy(msg, "To enter Pokemon Center press '>'.");
                t->pc->poke_center = 1;
                heap_insert(&world.curr_map->queue, t);
                continue;
            } else {
                t->pc->poke_mart = 0;
                t->pc->poke_center = 0;
            }

            /* Check if exit */
            if (d[dim_x] == world.curr_map->n && d[dim_y] == 0) {
                world.player.pc->dir = north;
                world.curr_map->tmap[t->pos[dim_y]][t->pos[dim_x]] = NULL;
                if (world.curr_index[dim_y] == 0) {
                    // wrap to other side of world
                    world.curr_index[dim_y] = WORLD_SIZE - 1;
                } else {
                    world.curr_index[dim_y]--;
                }
                new_map();
                return;
            } else if (d[dim_x] == 0 && d[dim_y] == world.curr_map->w) {
                world.player.pc->dir = west;
                world.curr_map->tmap[t->pos[dim_y]][t->pos[dim_x]] = NULL;
                if (world.curr_index[dim_x] == 0) {
                    // wrap to other side of world
                    world.curr_index[dim_x] = WORLD_SIZE - 1;
                } else {
                    world.curr_index[dim_x]--;
                }
                new_map();
                return;
            } else if (d[dim_x] == MAP_X - 1 && d[dim_y] == world.curr_map->e) {
                world.player.pc->dir = east;
                world.curr_map->tmap[t->pos[dim_y]][t->pos[dim_x]] = NULL;
                if (world.curr_index[dim_x] == WORLD_SIZE - 1) {
                    // wrap to other side of world
                    world.curr_index[dim_x] = 0;
                } else {
                    world.curr_index[dim_x]++;
                }
                new_map();
                return;
            } else if (d[dim_x] == world.curr_map->s && d[dim_y] == MAP_Y - 1) {
                world.player.pc->dir = south;
                world.curr_map->tmap[t->pos[dim_y]][t->pos[dim_x]] = NULL;
                if (world.curr_index[dim_y] == WORLD_SIZE - 1) {
                    // wrap to other side of world
                    world.curr_index[dim_y] = 0;
                } else {
                    world.curr_index[dim_y]++;
                }
                new_map();
                return;
            }

            world.curr_map->tmap[t->pos[dim_y]][t->pos[dim_x]] = NULL;
            world.curr_map->tmap[d[dim_y]][d[dim_x]] = t;
            t->curr_time += move_cost[tr_player][world.curr_map->map[t->pos[dim_y]][t->pos[dim_x]]];
            t->pos[dim_x] = d[dim_x];
            t->pos[dim_y] = d[dim_y];
            world.player.pos[dim_x] = d[dim_x];
            world.player.pos[dim_y] = d[dim_y];
        } else {
            move_func[t->npc->mtype](t, d);
            world.curr_map->tmap[t->pos[dim_y]][t->pos[dim_x]] = NULL;
            world.curr_map->tmap[d[dim_y]][d[dim_x]] = t;
            t->curr_time += move_cost[t->npc->ttype][world.curr_map->map[d[dim_y]][d[dim_x]]];
            t->pos[dim_y] = d[dim_y];
            t->pos[dim_x] = d[dim_x];
        }
        heap_insert(&world.curr_map->queue, t);
    } while (t != &world.player);

    pathfind(world.curr_map);
}

/* Return 1 if movement in specified direction is valid and 0 otherwise */
int validate_movement(direction_t d) {
    int x = world.player.pos[dim_x] + direction_vals[d][dim_x];
    int y = world.player.pos[dim_y] + direction_vals[d][dim_y];
    if (((x == world.curr_map->n && y == 0) || (x == 0 && y == world.curr_map->w) || (x == MAP_X - 1 && y == world.curr_map->e) ||
         (x == world.curr_map->s && y == MAP_Y - 1)) || (x > 0 && x < MAP_X - 1 && y > 0 && y < MAP_Y - 1 && 
         move_cost[tr_player][world.curr_map->map[y][x]] < INT_MAX)) {
        if (!world.curr_map->tmap[y][x]) {
            return 1;
        } else {
            if (world.curr_map->tmap[y][x]->npc->defeated == 0) {
                pokemon_battle();
                world.curr_map->tmap[y][x]->npc->defeated = 1;
                switch (world.curr_map->tmap[y][x]->npc->ttype) {
                    case tr_hiker:
                    case tr_rival:
                        world.curr_map->tmap[y][x]->npc->mtype = move_walker;
                    default:
                        break;
                }
                return 2;
            }
        }
    }
    return 0;
}

void game_loop() {
    uint8_t quit_game = 0;
    do {
        print_cur_map();
        int input = getch();
        int v;
        switch (input) {
            case '7':
            case 'y':
            case 'w':
                /* Attempt to move PC one cell to upper left */
                v = validate_movement(northwest);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = northwest;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '8':
            case 'k':
            case 'e':
                /* Attempt to move PC one cell up */
                v = validate_movement(north);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = north;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '9':
            case 'u':
            case 'r':
                /* Attempt to move PC one cell to upper right */
                v = validate_movement(northeast);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = northeast;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '6':
            case 'l':
            case 'f':
                /* Attempt to move PC one cell right */
                v = validate_movement(east);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = east;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '3':
            case 'n':
            case 'v':
                /* Attempt to move PC one cell to lower right */
                v = validate_movement(southeast);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = southeast;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '2':
            case 'j':
            case 'c':
                /* Attempt to move PC one cell down */
                v = validate_movement(south);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = south;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '1':
            case 'b':
            case 'x':
                /* Attempt to move PC one cell to lower left */
                v = validate_movement(southwest);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = southwest;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '4':
            case 'h':
            case 's':
                /* Attempt to move PC one cell to left */
                v = validate_movement(west);
                if (v == 1) {
                    strcpy(msg, "");
                    world.player.pc->dir = west;
                    move_trainers();
                    print_cur_map();
                } else if (v == 0) {
                    strcpy(msg, "Cannot move there!");
                }
                break;
            case '>':
                /* Attempt to enter a Pokemart or Pokemon Center */
                // should update in the future to be more streamlined
                if (world.player.pc->poke_mart) {
                    enter_pokemart();
                } else if (world.player.pc->poke_center) {
                    enter_pokemon_center();
                } else {
                    strcpy(msg, "Must move towards a PokeMart or PokeCenter first before you can enter one!");
                }
                break;
            case '5':
            case '.':
            case ' ':
            case 'd':
                /* Rest for a turn */
                strcpy(msg, "");
                world.player.pc->dir = center;
                move_trainers();
                print_cur_map();
                break;
            case 't':
                /* Display a list of trainers on the map */
                display_tr_list();

                break;
            case 'Q':
            case 'q':
                /* Quit the game */
                quit_game = 1;
                break;
            default:
                /* No-op */
        }

        refresh();
    } while (!quit_game);
}

/* Initialize the terminal with all the proper settings */
void init_terminal() {
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    set_escdelay(100);   // uncomment this if long delay when pressing escape key

    start_color();
    init_pair(PATH_PAIR, COLOR_BLACK, COLOR_CYAN);
    init_pair(FOREST_PAIR, COLOR_GREEN, COLOR_BLACK);
    init_pair(MART_PAIR, COLOR_WHITE, COLOR_BLUE);
    init_pair(CENTER_PAIR, COLOR_WHITE, COLOR_RED);
    init_pair(BOULDER_PAIR, COLOR_WHITE, COLOR_BLACK);
    init_pair(GRASS_PAIR, COLOR_YELLOW, COLOR_GREEN);
    init_pair(PLAYER_PAIR, COLOR_WHITE, COLOR_RED);
    init_pair(SEEKER_PAIR, COLOR_BLUE, COLOR_WHITE);
    init_pair(OTHER_PAIR, COLOR_BLACK, COLOR_WHITE);
}

int main(int argc, char *argv[]) {
    struct timeval tv;
    uint32_t seed;

    /* Process command line arguments */
    if (argc == 2) {
        seed = atoi(argv[1]);
    } else {
        gettimeofday(&tv, NULL);
        seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
    }

    // printf("Using seed: %u\n", seed);
    srand(seed);

    /* Initialize the world */
    init_world();

    /* Run the UI with curses */
    init_terminal();

    /* Start the game loop */
    game_loop();

    /* Handle cleanup before program termination */
    delete_world();
    endwin();           /* End curses mode */
    
    return 0;
}