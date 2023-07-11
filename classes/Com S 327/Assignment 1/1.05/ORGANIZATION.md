## dijkstra.h - includes (world.h)
    void dijkstra_path(...)
    void pathfind(...)

## dijkstra.cpp - includes (dijkstra.h, heap.h, mapGen.h, trainer.h)
    #define ter_cost(...) ...                        // defines ter_cost utility function for quickly accessing move_cost array
    typedef struct path {...}
    static int32_t path_cmp(...) {...}
    static int32_t hiker_cmp(...) {...}
    static int32_t rival_cmp(...) {...}
    static int32_t edge_penalty(...) {...}
    static void dijkstra_path(...) {...}
    void pathfind(...) {...}

## mapGen.h - includes (world.h)
    #define mapxy(..), heightpair(..), heightxy(..)  // utility functions for map generation
    extern pair_t all_dirs[8]                        // makes all_dirs accessible throughout program
    void delete_character(...)                       // called delete_trainer in my code
    int new_map(int teleport)

## mapGen.cpp - includes (mapGen.h, dijkstra.h, trainer.h ** MAY NEED TO INCLUDE world.h **)
    #define MIN_TREES, ..., BOULDER_PROB             // settings for map generation
    #define MIN_TRAINERS, ADD_TRAINER_PROB
h   #define malloc(size) {...}
h   #define rand_dir(dir) {...}                      // gets a random direction
    pair_t all_dirs[8] = {...}                       // declares the global all_dirs array
    typedef struct queue_node {...}
    static int32_t cmp_char_turns(...) {...}         // comparison used to decide next trainer in priority queue - trainer_cmp in my code
    void delete_character(...) {...}                 // delete_trainer in my code
    static int build_paths(...) {...}
    static int gaussian[5][5] = {...}                //**
    static int smooth_height(...) {...}                **
    static void find_building_location(...) {...}      **
    static int place_pokemart(...) {...}               ** functions and utilities used to generate
    static int place_center(...) {...}                 ** terrain during the map generation process
    static int map_terrain(...) {...}                  **
    static int place_boulders(...) {...}               **
    static int place_trees(...) {...}                **//
    void rand_pos(...) {...}                         // generates a random position on the map
    void new_hiker() {...}
    void new_rival() {...}
    void new_char_other() {...}                      // new_tr_other in my code
    void place_characters() {...}                    // place_trainers in my code
    void init_pc() {...}
    void place_pc() {...}                            // combined with init_pc in my code - use prof's implementation
    int new_map(int teleport) {...}                  // includes teleporting in world for debugging pokemon spawns at extremities in future

## world.h - includes (heap.h)
    #define MAP_X, MAP_Y
    #define WORLD_SIZE
    #define rand_range(min, max) {...}               // returns random integer in [min, max]
    typedef enum dim {...} dim_t                     // dim_x / dim_y utility enum
    typedef int16_t pair_t[num_dims]                 // coordinate utility enum
    ***** direction_t enum?? *****
    typedef enum ... terrain_type {...}
    typedef struct character {...}                   // trainer in my code
    typedef struct map {...}
    typedef struct world {...}
    extern world_t world                             // makes world accessible throughout program
    void leave_map(...)                              // function called upon switching maps - auto updates curr world idx & calls new_map
    void init_world()
    void delete_world()

## world.cpp - includes (world.h, mapGen.h ** MAY NEED TO INCLUDE trainer.h **)
    world_t world                                    // declares the global world variable
    ****** char tr_representation[] *****
    void leave_map(...) {...}                        // function called upon switching maps - auto updates curr world idx & calls new_map
    void init_world() {...}
    void delete_world() {...}

## io.h - includes (world.h)
    void io_init_terminal(...)
    void io_reset_terminal(...)
    void io_display(...)
    void io_handle_input(...)
    void io_queue_message(...)
    void io_battle(...)

## io.cpp - includes (io.h ** MAY NEED TO INCLUDE trainer.h **)
    typedef struct io_message {...}                         // struct used to store the message displayed at the top of the screen
    static io_message_t *io_head, *io_tail                  // actual message variables
    void io_init_terminal(...) {...}                        // initializes curses
    void io_reset_terminal(...) {...}                       // ends curses and cleans up all io related stuff
    void io_queue_message(...) {...}                        // queues up a message to display after the current one
    static void io_print_message_queue(...) {...}           // displays the next message in the queue
    static int compare_trainer_distance(...) {...}          // compares trainer distance to PC according to rival distance map
    static character_t *io_nearest_visible_trainer() {...}  // returns the nearest visible trainer to the PC
    void io_display() {...}                                 // updates the display
    uint32_t io_teleport_pc(...) {...}                      // teleports the PC to a random position on the map - mostly for debugging
    static void io_scroll_trainer_list(...) {...}           // handles control while displaying the trainer list
    static void io_list_trainers_display(...) {...}         // update the display with the trainer list
    static void io_list_trainers() {...}                    // enters the trainer list display control and handles exiting
    void io_pokemart() {...}                                // temporary placeholder for pokemart display
    void io_pokemon_center() {...}                          // temporary placeholder for pokemon center display
    void io_battle(...) {...}                               // temporary placeholder for trainer battle display
    uint32_t move_pc_dir(...) {...}                         // handles movement of the PC in the proper direction based on player input
    void io_teleport_world(...) {...}                       // teleports to a specified map based on player input
    void io_handle_input(...) {...}                         // handles player input and runs the proper functions

## trainer.h - includes (world.h)
    typedef enum ... movement_type {...}
    typedef enum ... character_type {...}            // called trainer_type in my code
    extern int32_t move_cost[...][...]               // makes move_cost array available throughout program
    extern const char *char_type_name[...]           // makes char_type_name array available throughout program - tr_type_name in my code
    extern void (*move_func[...])(...)               // makes move_func function available throughout program
    typedef struct npc {...}
    typedef struct pc {...}

## trainer.cpp - includes (trainer.h)
    int32_t move_cost[...][...] = {...}              // declares global array move_cost
    const char *char_type_name[...] = {...}          // declares global array char_type_name - tr_type_name or something similar in my code
    static void move_hiker_func(...) {...}           //**
    static void move_rival_func(...) {...}             **
    static void move_pacer_func(...) {...}             **
    static void move_wanderer_func(...) {...}          ** functions for moving each type of trainer
    static void move_sentry_func(...) {...}            **
    static void move_walker_func(...) {...}            **
    static void move_pc_func(...) {...}              **//
    void (*move_func[...])(...) = {...}              // declares global function move_func

## main.cpp - includes (world.h)
    void game_loop() {...}                           // loop that runs until player quits game
    int main(...) {...}