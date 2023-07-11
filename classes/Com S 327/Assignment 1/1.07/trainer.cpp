#include <stdlib.h>
#include <limits.h>

#include "trainer.h"
#include "world.h"
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

const char *tr_type_name[num_trainer_types] = {
    "PC",
    "Hiker",
    "Rival",
    "Trainer",
};

static void move_hiker_func(Trainer *c, pair_t dest) {
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
        /* check that the destination isn't a Pokemon Center or PokeMart */
        if (world.get_cur_map()->get_map_val(dest) == ter_center || world.get_cur_map()->get_map_val(dest) == ter_mart) {
            dest[dim_x] = c->get_pos_x();
            dest[dim_y] = c->get_pos_y();
            min = world.hiker_dist[dest[dim_y]][dest[dim_x]];
        } else if (world.hiker_dist[c->get_pos_y() + all_dirs[i & 0x7][dim_y]][c->get_pos_x() + all_dirs[i & 0x7][dim_x]] == 0) {
            io_battle(c, &world.player);
            break;
        }
    }
}

static void move_rival_func(Trainer *c, pair_t dest) {
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
        /* check that the destination isn't a Pokemon Center or PokeMart */
        if (world.get_cur_map()->get_map_val(dest) == ter_center || world.get_cur_map()->get_map_val(dest) == ter_mart) {
            dest[dim_x] = c->get_pos_x();
            dest[dim_y] = c->get_pos_y();
            min = world.rival_dist[dest[dim_y]][dest[dim_x]];
        } else if (world.rival_dist[c->get_pos_y() + all_dirs[i & 0x7][dim_y]][c->get_pos_x() + all_dirs[i & 0x7][dim_x]] == 0) {
            io_battle(c, &world.player);
            break;
        }
    }
}

static void move_pacer_func(Trainer *c, pair_t dest) {
    NPC *n = dynamic_cast<NPC *>(c);
    
    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();

    if (!n->is_defeated && world.get_cur_map()->get_trainer(c->get_pos_x() + n->get_dir_x(), c->get_pos_y() + n->get_dir_y()) 
         == &world.player) {
        io_battle(c, &world.player);
        return;
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + n->get_dir_x(), c->get_pos_y() + n->get_dir_y()) !=
         world.get_cur_map()->get_map_val(c->get_pos_x(), c->get_pos_y())) || world.get_cur_map()->get_trainer(c->get_pos_x() + n->get_dir_x(),
         c->get_pos_y() + n->get_dir_y())) {
        n->set_dir(n->get_dir_x() * -1, n->get_dir_y() * -1);
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + n->get_dir_x(), c->get_pos_y() + n->get_dir_y()) ==
         world.get_cur_map()->get_map_val(c->get_pos_x(), c->get_pos_y())) && !world.get_cur_map()->get_trainer(c->get_pos_x() + n->get_dir_x(),
         c->get_pos_y() + n->get_dir_y())) {
        dest[dim_x] = c->get_pos_x() + n->get_dir_x();
        dest[dim_y] = c->get_pos_y() + n->get_dir_y();
    }
}

static void move_wanderer_func(Trainer *c, pair_t dest) {
    NPC *t = dynamic_cast<NPC *>(c);

    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();

    if (!t->is_defeated && world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(), 
         c->get_pos_y() + t->get_dir_y()) == &world.player) {
        io_battle(c, &world.player);
        return;
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y()) !=
         world.get_cur_map()->get_map_val(c->get_pos_x(), c->get_pos_y())) || world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(),
         c->get_pos_y() + t->get_dir_y())) {
        rand_dir(t);
    }

    if ((world.get_cur_map()->get_map_val(c->get_pos_x() + t->get_dir_x(), c->get_pos_y() + t->get_dir_y()) ==
         world.get_cur_map()->get_map_val(c->get_pos_x(), c->get_pos_y())) && !world.get_cur_map()->get_trainer(c->get_pos_x() + t->get_dir_x(),
         c->get_pos_y() + t->get_dir_y())) {
        dest[dim_x] = c->get_pos_x() + t->get_dir_x();
        dest[dim_y] = c->get_pos_y() + t->get_dir_y();
    }
}

static void move_sentry_func(Trainer *c, pair_t dest) {
    // Not a bug.  Sentries are non-aggro.
    dest[dim_x] = c->get_pos_x();
    dest[dim_y] = c->get_pos_y();
}

static void move_walker_func(Trainer *c, pair_t dest) {
    NPC *t = dynamic_cast<NPC *>(c);

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

static void move_pc_func(Trainer *c, pair_t dest) {
    io_display();
    io_handle_input(dest);
}

void (*move_func[num_movement_types])(Trainer *, pair_t) = {
    move_hiker_func,
    move_rival_func,
    move_pacer_func,
    move_wanderer_func,
    move_sentry_func,
    move_walker_func,
    move_pc_func,
};