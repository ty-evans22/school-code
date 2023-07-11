#include <stdlib.h>
#include <limits.h>

#include "trainer.h"
#include "mapGen.h"
#include "io.h"

/***********************************************************************
 * Hack: Avoid the "path to a building" issue by making building cells *
 * have a large but not infinite movement cost.  This allows the       *
 * pathfinding algorithm to run without overflowing, but still makes   *
 * NPCs avoid buildings most of the time since the cost to move on to  *
 * the building is greater than navigating the building's perimeter.   *
 ***********************************************************************/
int32_t move_cost[num_trainer_types][num_terrain_types] = {
    { INT_MAX, INT_MAX, 10, 10, 10, 20, 10, INT_MAX, INT_MAX, 10      },
    { INT_MAX, INT_MAX, 10, 50, 50, 15, 10, 15,      15,      INT_MAX },
    { INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX },
    { INT_MAX, INT_MAX, 10, 50, 50, 20, 10, INT_MAX, INT_MAX, INT_MAX },
};

/* declares global array tr_type_name */
const char *tr_type_name[num_trainer_types] = {
    "PC",
    "Hiker",
    "Rival",
    "Trainer",
};

static void move_hiker_func(trainer_t *c, pair_t dest) {
    int min;
    int base;
    int i;

    base = rand() & 0x7;

    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();
    min = INT_MAX;
    
    for (i = base; i < 8 + base; i++) {
        if ((world.hiker_dist[c->get_pos_y() + all_dirs[i & 0x7][dim_y]][c->get_pos_x() + all_dirs[i & 0x7][dim_x]] <= min) &&
             !world.get_cur_map()->get_trainer(c->get_pos_x() + all_dirs[i & 0x7][dim_x], c->get_pos_y() + all_dirs[i & 0x7][dim_y])) {
            dest[dim_x] = c->get_pos_x() + all_dirs[i & 0x7][dim_x];
            dest[dim_y] = c->get_pos_y() + all_dirs[i & 0x7][dim_y];
            min = world.hiker_dist[dest[dim_y]][dest[dim_x]];
        }
        if (world.hiker_dist[c->get_pos_y() + all_dirs[i & 0x7][dim_y]][c->get_pos_x() + all_dirs[i & 0x7][dim_x]] == 0) {
            io_battle(c, &world.player);
            break;
        }
    }
}

static void move_rival_func(trainer_t *c, pair_t dest) {
    int min;
    int base;
    int i;
    
    base = rand() & 0x7;

    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();
    min = INT_MAX;
    
    for (i = base; i < 8 + base; i++) {
        if ((world.rival_dist[c->get_pos_y() + all_dirs[i & 0x7][dim_y]][c->get_pos_x() + all_dirs[i & 0x7][dim_x]] < min) &&
             !world.get_cur_map()->get_trainer(c->get_pos_x() + all_dirs[i & 0x7][dim_x], c->get_pos_y() + all_dirs[i & 0x7][dim_y])) {
            dest[dim_x] = c->get_pos_x() + all_dirs[i & 0x7][dim_x];
            dest[dim_y] = c->get_pos_y() + all_dirs[i & 0x7][dim_y];
            min = world.rival_dist[dest[dim_y]][dest[dim_x]];
        }
        if (world.rival_dist[c->get_pos_y() + all_dirs[i & 0x7][dim_y]][c->get_pos_x() + all_dirs[i & 0x7][dim_x]] == 0) {
            io_battle(c, &world.player);
            break;
        }
    }
}

static void move_pacer_func(trainer_t *c, pair_t dest) {
    npc_t *t = static_cast<npc_t*>(c);
    
    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();

    if (!t->is_defeated && world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y()) 
         == &world.player) {
        io_battle(c, &world.player);
        return;
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y()) !=
         world.get_cur_map()->get_map_val(*c->get_pos())) || world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(),
         c->get_pos_y() + t->get_dir_y())) {
        t->set_dir(t->get_dir_x() * -1, t->get_dir_y() * -1);
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y()) ==
         world.get_cur_map()->get_map_val(*c->get_pos())) && !world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(),
         c->get_pos_y() + t->get_dir_y())) {
        dest[dim_x] = c->get_pos_x() + t->get_dir_x();
        dest[dim_y] = c->get_pos_y() + t->get_dir_y();
    }
}

static void move_wanderer_func(trainer_t *c, pair_t dest) {
    npc_t *t = static_cast<npc_t*>(c);

    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();

    if (!t->is_defeated && world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(), 
         c->get_pos_y() + t->get_dir_y()) == &world.player) {
        io_battle(c, &world.player);
        return;
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y()) !=
         world.get_cur_map()->get_map_val(*c->get_pos())) || world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(),
         c->get_pos_y() + t->get_dir_y())) {
        rand_dir(t);
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y()) ==
         world.get_cur_map()->get_map_val(*c->get_pos())) && !world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(),
         c->get_pos_y() + t->get_dir_y())) {
        dest[dim_x] = c->get_pos_x() + t->get_dir_x();
        dest[dim_y] = c->get_pos_y() + t->get_dir_y();
    }
}

static void move_sentry_func(trainer_t *c, pair_t dest) {
    // Not a bug.  Sentries are non-aggro.
    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();
}

static void move_walker_func(trainer_t *c, pair_t dest) {
    npc_t *t = static_cast<npc_t*>(c);

    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();

    if (!t->is_defeated && world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(),
         c->get_pos_y() + t->get_dir_y()) == &world.player) {
        io_battle(c, &world.player);
        return;
    }

    if ((move_cost[tr_other][world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y())] == INT_MAX)
         || world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y())) {
        t->set_dir(t->get_dir_x() * -1, t->get_dir_y() * -1);
    }

    if ((move_cost[tr_other][world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y())] != INT_MAX)
         && !world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y())) {
        dest[dim_x] = c->get_pos_x() + t->get_dir_x();
        dest[dim_y] = c->get_pos_y() + t->get_dir_y();
    }
}

static void move_pc_func(trainer_t *c, pair_t dest) {
    io_display();
    io_handle_input(dest);
}

void (*move_func[num_movement_types])(trainer_t *, pair_t) = {
    move_hiker_func,
    move_rival_func,
    move_pacer_func,
    move_wanderer_func,
    move_sentry_func,
    move_walker_func,
    move_pc_func,
};

/*** DEFINITIONS FOR npc_t CLASS ***/

/* constructor */
npc_t::npc_t(movement_type m) {
    is_defeated = false;
    mtype = m;

    switch(m) {
        case move_hiker:
            ttype = tr_hiker;
            symbol = 'h';
            break;
        case move_rival:
            ttype = tr_rival;
            symbol = 'r';
            break;
        case move_pace:
            ttype = tr_other;
            symbol = 'p';
            break;
        case move_wander:
            ttype = tr_other;
            symbol = 'w';
            break;
        case move_sentry:
            ttype = tr_other;
            symbol = 's';
            break;
        case move_walk:
            ttype = tr_other;
            symbol = 'n';
            break;
        default:
            break;
    }
}

/* destructor */
npc_t::~npc_t() {

}

/*** DEFINITIONS FOR pc_t CLASS ***/

/* constructor */
pc_t::pc_t() {
    ttype = tr_pc;
    symbol = '@';
}

/* destructor */
pc_t::~pc_t() {
    
}