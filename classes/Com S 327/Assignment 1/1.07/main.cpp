#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>

#include "db_parse.h"
#include "io.h"
#include "heap.h"
#include "world.h"
#include "dijkstra.h"
#include "trainer.h"
#include "pokemon.h"

/* main game loop that runs until the player quits the game */
void game_loop() {
    Trainer *c;
    NPC *n;
    PC *p;
    pair_t d;
    
    while (!world.quit) {
        c = (Trainer *) heap_remove_min(&world.get_cur_map()->queue);
        n = dynamic_cast<NPC *> (c);
        p = dynamic_cast<PC *> (c);

        move_func[n ? n->get_mtype() : move_pc](c, d);

        world.get_cur_map()->set_trainer(c->get_pos_x(), c->get_pos_y(), NULL);
        if (p && (d[dim_x] == 0 || d[dim_x] == MAP_X - 1 || d[dim_y] == 0 || d[dim_y] == MAP_Y - 1)) {
            world.leave_map(d);
            d[dim_x] = c->get_pos_x();
            d[dim_y] = c->get_pos_y();
        }
        world.get_cur_map()->set_trainer(d, c);

        if (p) {
            // Performance bug - pathfinding runs twice after generating a new map
            pathfind(world.get_cur_map());
        }

        c->add_cost(move_cost[n ? n->get_ttype() : tr_pc][world.get_cur_map()->get_map_val(d)]);
        c->set_pos(d);
        heap_insert(&world.get_cur_map()->queue, c);
    }
}

// int main(int argc, char *argv[]) {
//     struct timeval tv;
//     uint32_t seed;

//     /* parse and load in data from files */
//     db_parse(false); /* value of true to print all file info, false otherwise */

//     if (argc == 2) {
//         seed = atoi(argv[1]);
//     } else {
//         gettimeofday(&tv, NULL);
//         seed = (tv.tv_usec ^ (tv.tv_sec << 20)) & 0xffffffff;
//     }

//     printf("Using seed: %u\n", seed);
//     srand(seed);

//     init_world();

//     char input;
//     do {
//         input = scanf("%c", &input);
//         Pokemon *p = generate_random_pokemon();

//         printf("\n");
//         printf("%s, %c - Lvl: %d\n", p->get_name(), p->get_gender(), p->get_level());
//         for (int i = 0; i < num_stat_types; i++) {
//             printf("%s: %d", stats[i + 1].identifier, p->get_stat(i));
//             if (i != num_stat_types - 1) {
//                 printf(", ");
//             } else {
//                 printf("\n");
//             }
//         }
//         if (p->get_move(0)) {
//             printf("Move 1: %s", p->get_move(0)->name);
//             if (p->get_move(1)) {
//                 printf(", Move 2: %s", p->get_move(1)->name);
//             }
//         }
//     } while (input != 'q');

//     delete_world();

//     return 0;
// }

int main(int argc, char *argv[]) {
    struct timeval tv;
    uint32_t seed;

    /* parse and load in data from files */
    db_parse(false); /* value of true to print all file info, false otherwise */

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

    init_poke_gen();

    game_loop();

    delete_world();

    io_reset_terminal();

    return 0;
}