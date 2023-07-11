#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "trainer.h"
#include "world.h"
#include "io.h"
#include "dijkstra.h"

/* main game loop that runs until the player quits the game */
void game_loop() {
    trainer_t *c;
    pair_t d;

    while(!world.quit) {
        c = (trainer_t*)heap_remove_min(&world.get_cur_map()->queue);

        if (c->is_pc()) {
            move_func[move_pc](c, d);
        } else {
            npc_t *t = static_cast<npc_t*>(c);
            move_func[t->get_mtype()](c, d);
        }

        world.get_cur_map()->set_trainer(*c->get_pos(), NULL);
        if (c->is_pc() && (d[dim_x] == 0 || d[dim_x] == MAP_X - 1 || d[dim_y] == 0 || d[dim_y] == MAP_Y - 1)) {
            world.leave_map(d);
            d[dim_x] = c->get_pos_x();
            d[dim_y] = c->get_pos_y();
        }
        world.get_cur_map()->set_trainer(d, c);

        if (c->is_pc()) {
            // Performance bug - pathfinding runs twice after generating a new map
            pathfind(world.get_cur_map());
        }
        
        c->add_cost(move_cost[c->get_tr_type()][world.get_cur_map()->get_map_val(d)]);
        c->set_pos(d);

        heap_insert(&world.get_cur_map()->queue, c);
    }
}

int main(int argc, char *argv[]) {
    struct timeval tv;
    uint32_t seed;

    if (argc == 2) {
        seed = atoi(argv[1]);
    } else {
        gettimeofday(&tv, NULL);
        seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
    }

    printf("Using seed: %u\n", seed);
    srand(seed);

    io_init_terminal();
  
    init_world();

    game_loop();

    delete_world();

    io_reset_terminal();

    return 0;
}