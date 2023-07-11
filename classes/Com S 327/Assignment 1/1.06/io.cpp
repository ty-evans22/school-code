#include <stdlib.h>
#include <limits.h>
#include <ncurses.h>

#include "io.h"
#include "world.h"
#include "mapGen.h"

/* define the color pairs for various obstacles and trainers */
#define PATH_PAIR           1
#define FOREST_PAIR         2
#define MART_PAIR           3
#define CENTER_PAIR         4
#define BOULDER_PAIR        5
#define GRASS_PAIR          6
#define PLAYER_PAIR         7
#define SEEKER_PAIR         8
#define OTHER_PAIR          9
#define MESSAGE_PAIR        10
#define DEFAULT_PAIR        11

#define KEY_ESC             27

/* struct used to store the message displayed at the top of the screen */
typedef struct io_message {
    /* Will print " --more-- " at end of line when another message follows. *
     * Leave 10 extra spaces for that.                                      */
    char msg[71];
    struct io_message *next;
} io_message_t;

/* pointers to linked list containing entire current message and queued messages */
static io_message_t *io_head, *io_tail;

/* initializes curses */
void io_init_terminal(void) {
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
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
    init_pair(MESSAGE_PAIR, COLOR_CYAN, COLOR_BLACK);
    init_pair(DEFAULT_PAIR, COLOR_YELLOW, COLOR_BLACK);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
}

/* ends curses and cleans up all io related stuff */
void io_reset_terminal(void) {
    endwin();

    while (io_head) {
        io_tail = io_head;
        io_head = io_head->next;
        free(io_tail);
    }
    io_tail = NULL;
}

/* queues up a message to display after the current one */
void io_queue_message(const char *format, ...) {
    io_message_t *tmp;
    va_list ap;

    if (!(tmp = (io_message_t*)malloc(sizeof (*tmp)))) {
        perror("malloc");
        exit(1);
    }

    tmp->next = NULL;

    va_start(ap, format);

    vsnprintf(tmp->msg, sizeof (tmp->msg), format, ap);

    va_end(ap);

    if (!io_head) {
        io_head = io_tail = tmp;
    } else {
        io_tail->next = tmp;
        io_tail = tmp;
    }
}

/* displays the next message in the queue */
static void io_print_message_queue(uint32_t y, uint32_t x) {
    while (io_head) {
        io_tail = io_head;
        attron(COLOR_PAIR(MESSAGE_PAIR));
        mvprintw(y, x, "%-80s", io_head->msg);
        attroff(COLOR_PAIR(MESSAGE_PAIR));
        io_head = io_head->next;
        if (io_head) {
            attron(COLOR_PAIR(MESSAGE_PAIR));
            mvprintw(y, x + 70, "%10s", " --more-- ");
            attroff(COLOR_PAIR(MESSAGE_PAIR));
            refresh();
            getch();
        }
        free(io_tail);
    }
    io_tail = NULL;
}

/**************************************************************************
 * Compares trainer distances from the PC according to the rival distance *
 * map.  This gives the approximate distance that the PC must travel to   *
 * get to the trainer (doesn't account for crossing buildings).  This is  *
 * not the distance from the NPC to the PC unless the NPC is a rival.     *
 *                                                                        *
 * Not a bug.                                                             *
 **************************************************************************/
static int compare_trainer_distance(const void *v1, const void *v2) {
    trainer_t *c1 = (trainer_t*)v1;
    trainer_t *c2 = (trainer_t*)v2;

    return (world.rival_dist[c1->get_pos_y()][c1->get_pos_x()] -
            world.rival_dist[c2->get_pos_y()][c2->get_pos_x()]);
}

/* returns the nearest visible trainer to the PC */
static trainer_t *io_nearest_visible_trainer() {
    trainer_t **c, *n;
    uint32_t x, y, count;

    c = (trainer_t**)malloc(world.get_cur_map()->get_num_trainers() * sizeof (*c));

    /* get a linear list of trainers */
    for (count = 0, y = 1; y < MAP_Y - 1; y++) {
        for (x = 1; x < MAP_X - 1; x++) {
            if (world.get_cur_map()->get_trainer(x, y) && world.get_cur_map()->get_trainer(x, y) != &world.player) {
                c[count++] = world.get_cur_map()->get_trainer(x, y);
            }
        }
    }

    /* sort it by distance from PC */
    qsort(c, count, sizeof (*c), compare_trainer_distance);

    n = c[0];

    free(c);

    return n;
}

/* updates the display */
void io_display() {
    uint32_t y, x;
    trainer_t *c;

    clear();
    for (y = 0; y < MAP_Y; y++) {
        for (x = 0; x < MAP_X; x++) {
            if (world.get_cur_map()->get_trainer(x, y)) {
                mvaddch(y + 1, x, world.get_cur_map()->get_trainer(x, y)->get_symbol());
            } else {
                switch (world.get_cur_map()->get_map_val(x, y)) {
                    case ter_boulder:
                    case ter_mountain:
                        attron(COLOR_PAIR(BOULDER_PAIR));
                        mvaddch(y + 1, x, '%');
                        attroff(COLOR_PAIR(BOULDER_PAIR));
                        break;
                    case ter_tree:
                    case ter_forest:
                        attron(COLOR_PAIR(FOREST_PAIR));
                        mvaddch(y + 1, x, '^');
                        attroff(COLOR_PAIR(FOREST_PAIR));
                        break;
                    case ter_path:
                    case ter_exit:
                        attron(COLOR_PAIR(PATH_PAIR));
                        mvaddch(y + 1, x, '#');
                        attroff(COLOR_PAIR(PATH_PAIR));
                        break;
                    case ter_mart:
                        attron(COLOR_PAIR(MART_PAIR));
                        mvaddch(y + 1, x, 'M');
                        attroff(COLOR_PAIR(MART_PAIR));
                        break;
                    case ter_center:
                        attron(COLOR_PAIR(CENTER_PAIR));
                        mvaddch(y + 1, x, 'C');
                        attroff(COLOR_PAIR(CENTER_PAIR));
                        break;
                    case ter_grass:
                        attron(COLOR_PAIR(GRASS_PAIR));
                        mvaddch(y + 1, x, ':');
                        attroff(COLOR_PAIR(GRASS_PAIR));
                        break;
                    case ter_clearing:
                        attron(COLOR_PAIR(GRASS_PAIR));
                        mvaddch(y + 1, x, '.');
                        attroff(COLOR_PAIR(GRASS_PAIR));
                        break;
                    default:
                        /* Use zero as an error symbol, since it stands out somewhat, and it's *
                         * not otherwise used.                                                 */
                        attron(COLOR_PAIR(DEFAULT_PAIR));
                        mvaddch(y + 1, x, '0');
                        attroff(COLOR_PAIR(DEFAULT_PAIR));
                }
            }
        }
    }

    mvprintw(23, 1, "PC position is (%2d,%2d) on map %d%cx%d%c.",
             world.player.get_pos_x(),
             world.player.get_pos_y(),
             abs(world.get_cur_idx_x() - (WORLD_SIZE / 2)),
             world.get_cur_idx_x() - (WORLD_SIZE / 2) >= 0 ? 'E' : 'W',
             abs(world.get_cur_idx_y() - (WORLD_SIZE / 2)),
             world.get_cur_idx_y() - (WORLD_SIZE / 2) <= 0 ? 'N' : 'S');
    mvprintw(22, 1, "%d known %s.", world.get_cur_map()->get_num_trainers(),
             world.get_cur_map()->get_num_trainers() > 1 ? "trainers" : "trainer");
    mvprintw(22, 30, "Nearest visible trainer: ");
    if ((c = io_nearest_visible_trainer())) {
        attron(COLOR_PAIR(COLOR_RED));
        mvprintw(22, 55, "%c at %d %c by %d %c.",
                 c->get_symbol(),
                 abs(c->get_pos_y() - world.player.get_pos_y()),
                 ((c->get_pos_y() - world.player.get_pos_y()) <= 0 ?
                  'N' : 'S'),
                 abs(c->get_pos_x() - world.player.get_pos_x()),
                 ((c->get_pos_x() - world.player.get_pos_x()) <= 0 ?
                  'W' : 'E'));
        attroff(COLOR_PAIR(COLOR_RED));
    } else {
        attron(COLOR_PAIR(COLOR_BLUE));
        mvprintw(22, 55, "NONE.");
        attroff(COLOR_PAIR(COLOR_BLUE));
    }

    io_print_message_queue(0, 0);

    refresh();
}

/* teleports the PC to a random valid position on the map */
uint32_t io_teleport_pc(pair_t dest) {
    /* Just for fun. And debugging. Mostly debugging. */

    do {
        dest[dim_x] = rand_range(1, MAP_X - 2);
        dest[dim_y] = rand_range(1, MAP_Y - 2);
    } while (world.get_cur_map()->get_trainer(dest)                              ||
             move_cost[tr_pc][world.get_cur_map()->get_map_val(dest)] == INT_MAX ||
             world.rival_dist[dest[dim_y]][dest[dim_x]] < 0);
    return 0;
}

/* handles control while displaying the trainer list */
static void io_scroll_trainer_list(char (*s)[40], uint32_t count) {
    uint32_t offset;
    uint32_t i;

    offset = 0;

    while (1) {
        for (i = 0; i < 13; i++) {
            mvprintw(i + 6, 19, " %-40s ", s[i + offset]);
        }
        switch (getch()) {
            case KEY_UP:
                if (offset) {
                    offset--;
                }
                break;
            case KEY_DOWN:
                if (offset < (count - 13)) {
                    offset++;
                }
                break;
            case KEY_ESC:
                return;
        }
    }
}

/* update the display with the trainer list */
static void io_list_trainers_display(trainer_t **c, uint32_t count) {
    uint32_t i;
    char (*s)[40]; /* pointer to array of 40 char */

    s = (char (*)[40])malloc(count * sizeof (*s));

    mvprintw(3, 19, " %-40s ", "");
    /* borrow the first element of our array for this string: */
    snprintf(s[0], 40, "You know of %d trainers:", count);
    mvprintw(4, 19, " %-40s ", s);
    mvprintw(5, 19, " %-40s ", "");

    for (i = 0; i < count; i++) {
        snprintf(s[i], 40, "%16s %c: %2d %s by %2d %s",
                 tr_type_name[c[i]->get_tr_type()],
                 c[i]->get_symbol(),
                 abs(c[i]->get_pos_y() - world.player.get_pos_y()),
                 ((c[i]->get_pos_y() - world.player.get_pos_y()) <= 0 ?
                  "North" : "South"),
                 abs(c[i]->get_pos_x() - world.player.get_pos_x()),
                 ((c[i]->get_pos_x() - world.player.get_pos_x()) <= 0 ?
                  "West" : "East"));
        if (count <= 13) {
            /* Handle the non-scrolling case right here. *
             * Scrolling in another function.            */
            mvprintw(i + 6, 19, " %-40s ", s[i]);
        }
    }

    if (count <= 13) {
        mvprintw(count + 6, 19, " %-40s ", "");
        mvprintw(count + 7, 19, " %-40s ", "Hit escape to continue.");
        while (getch() != KEY_ESC);
    } else {
        mvprintw(19, 19, " %-40s ", "");
        mvprintw(20, 19, " %-40s ", "Arrows to scroll, escape to continue.");
        io_scroll_trainer_list(s, count);
    }

    free(s);
}

/* enters the trainer list display control and handles exiting */
static void io_list_trainers() {
    trainer_t **c;
    uint32_t x, y, count;

    c = (trainer_t**)malloc(world.get_cur_map()->get_num_trainers() * sizeof (*c));

    /* get a linear list of trainers */
    for (count = 0, y = 1; y < MAP_Y - 1; y++) {
        for (x = 1; x < MAP_X - 1; x++) {
            if (world.get_cur_map()->get_trainer(x, y) && world.get_cur_map()->get_trainer(x, y) != &world.player) {
                c[count++] = world.get_cur_map()->get_trainer(x, y);
            }
        }
    }

    /* sort it by distance from PC */
    qsort(c, count, sizeof (*c), compare_trainer_distance);

    /* display it */
    io_list_trainers_display(c, count);
    free(c);

    /* and redraw the map */
    io_display();
}

/* temporary placeholder for Pokemart display */
void io_pokemart() {
    mvprintw(0, 0, "Welcome to the Pokemart. Could I interest you in some Pokeballs?");
    refresh();
    getch();
}

/* temporary placeholder for Pokemon Center display */
void io_pokemon_center() {
    mvprintw(0, 0, "Welcome to the Pokemon Center. How can Nurse Joy assist you?");
    refresh();
    getch();
}

/* temporary placeholder for trainer battle display */
void io_battle(trainer_t *aggressor, trainer_t *defender) {
    npc_t *npc;

    io_display();
    mvprintw(0, 0, "Aww, how'd you get so strong? You and your pokemon must share a special bond!");
    refresh();
    getch();
    if (aggressor->is_pc()) {
        /* since we know defender points to an NPC we can use static_cast instead of dynamic_cast */
        npc = static_cast<npc_t*>(defender);
    } else {
        /* same logic as above */
        npc = static_cast<npc_t*>(aggressor);
    }

    npc->is_defeated = true;
    if (npc->get_tr_type() == tr_hiker || npc->get_tr_type() == tr_rival) {
        npc->set_mtype(move_wander);
    }
}

/* handles movement of the PC in the proper direction based on player input */
uint32_t move_pc_dir(uint32_t input, pair_t dest) {
    dest[dim_y] = world.player.get_pos_y();
    dest[dim_x] = world.player.get_pos_x();

    switch (input) {
        case 1:
        case 2:
        case 3:
            dest[dim_y]++;
            break;
        case 4:
        case 5:
        case 6:
            break;
        case 7:
        case 8:
        case 9:
            dest[dim_y]--;
            break;
    }

    switch (input) {
        case 1:
        case 4:
        case 7:
            dest[dim_x]--;
            break;
        case 2:
        case 5:
        case 8:
            break;
        case 3:
        case 6:
        case 9:
            dest[dim_x]++;
            break;
        case '>':
            if (world.get_cur_map()->get_map_val(*world.player.get_pos()) == ter_mart) {
                io_pokemart();
            } else if (world.get_cur_map()->get_map_val(*world.player.get_pos()) == ter_center) {
                io_pokemon_center();
            }
            break;
    }

    if ((world.get_cur_map()->get_map_val(dest) == ter_exit) &&
        (input == 1 || input == 3 || input == 7 || input == 9)) {
        // Exiting diagonally leads to complicated entry into the new map
        // in order to avoid INT_MAX move costs in the destination.
        // Most easily solved by disallowing such entries here.
        return 1;
    }

    if (world.get_cur_map()->get_trainer(dest) && !world.get_cur_map()->get_trainer(dest)->is_pc()) {
        /* since we know the trainer at dest is an NPC we can use static_cast instead of dynamic_cast */
        npc_t *npc = static_cast<npc_t*>(world.get_cur_map()->get_trainer(dest));
        if (npc->is_defeated) {
            // some kind of greeting here would be nice
            return 1;
        } else {
            io_battle(&world.player, world.get_cur_map()->get_trainer(dest));
            // not actually moving, so set dest back to the PC position
            dest[dim_x] = world.player.get_pos_x();
            dest[dim_y] = world.player.get_pos_y();
        }
    }

    if (move_cost[tr_pc][world.get_cur_map()->get_map_val(dest)] == INT_MAX) {
        return 1;
    }

    return 0;
}

/* teleports to a specified map based on player input */
void io_teleport_world(pair_t dest) {
    int x, y;

    world.get_cur_map()->set_trainer(*world.player.get_pos(), NULL);

    mvprintw(0, 0, "Enter x [-200, 200]: ");
    refresh();
    echo();
    curs_set(1);
    mvscanw(0, 21, "%d", &x);
    mvprintw(0, 0, "Enter y [-200, 200]:          ");
    refresh();
    mvscanw(0, 21, "%d", &y);
    refresh();
    noecho();
    curs_set(0);

    if (x < -200) {
        x = -200;
    } else if (x > 200) {
        x = 200;
    }
    if (y < -200) {
        y = -200;
    } else if (y > 200) {
        y = 200;
    }

    x += 200;
    y += 200;

    world.set_cur_idx(x, y);

    new_map(1);
    io_teleport_pc(dest);
}

/* handles player input and runs the proper functions */
void io_handle_input(pair_t dest) {
    uint32_t turn_not_consumed;
    int key;

    do {
        switch (key = getch()) {
            case '7':
            case 'y':
            case KEY_HOME:
                turn_not_consumed = move_pc_dir(7, dest);
                break;
            case '8':
            case 'k':
            case KEY_UP:
                turn_not_consumed = move_pc_dir(8, dest);
                break;
            case '9':
            case 'u':
            case KEY_PPAGE:
                turn_not_consumed = move_pc_dir(9, dest);
                break;
            case '6':
            case 'l':
            case KEY_RIGHT:
                turn_not_consumed = move_pc_dir(6, dest);
                break;
            case '3':
            case 'n':
            case KEY_NPAGE:
                turn_not_consumed = move_pc_dir(3, dest);
                break;
            case '2':
            case 'j':
            case KEY_DOWN:
                turn_not_consumed = move_pc_dir(2, dest);
                break;
            case '1':
            case 'b':
            case KEY_END:
                turn_not_consumed = move_pc_dir(1, dest);
                break;
            case '4':
            case 'h':
            case KEY_LEFT:
                turn_not_consumed = move_pc_dir(4, dest);
                break;
            case '5':
            case ' ':
            case '.':
            case KEY_B2:
                dest = *world.player.get_pos();
                turn_not_consumed = 0;
                break;
            case '>':
                turn_not_consumed = move_pc_dir('>', dest);
                break;
            case 'Q':
                dest = *world.player.get_pos();
                world.quit = true;
                turn_not_consumed = 0;
                break;
                // second break statement?
            case 't':
                /* teleport the PC to a random place in the map */
                io_teleport_pc(dest);
                turn_not_consumed = 0;
                break;
            case 'T':
                /* teleport the PC to any map in the world */
                io_teleport_world(dest);
                turn_not_consumed = 0;
                break;
            case 'm':
                io_list_trainers();
                turn_not_consumed = 1;
                break;
            case 'q':
                /* Demonstrate use of the message queue. You can use this for *
                 * printf()-style debugging (though gdb is probably a better  *
                 * option.) Not that it matters, but using this command will  *
                 * waste a turn. Set turn_not_consumed to 1 and you should be *
                 * able to figure out why I did it that way.                  */
                io_queue_message("This is the first message.");
                io_queue_message("Since there are multiple messages, "
                                 "you will see \"more\" prompts.");
                io_queue_message("You can use any key to advance through messages.");
                io_queue_message("Normal gameplay will not resume until the queue "
                                 "is empty.");
                io_queue_message("Long lines will be truncated, not wrapped.");
                io_queue_message("io_queue_message() is variadic and handles "
                                 "all printf() conversion specifiers.");
                io_queue_message("Did you see %s?", "what I did there");
                io_queue_message("When the last message is displayed, there will "
                                 "be no \"more\" prompt.");
                io_queue_message("Have fun!  And happy printing!");
                io_queue_message("Oh!  And use 'Q' to quit!");

                dest = *world.player.get_pos();
                turn_not_consumed = 0;
                break;
            default:
                /* Also not in the spec.  It's not always easy to figure out what *
                 * key code corresponds with a given keystroke.  Print out any    *
                 * unhandled key here.  Not only does it give a visual error      *
                 * indicator, but it also gives an integer value that can be used *
                 * for that key in this (or other) switch statements.  Printed in *
                 * octal, with the leading zero, because ncurses.h lists codes in *
                 * octal, thus allowing us to do reverse lookups.  If a key has a *
                 * name defined in the header, you can use the name here, else    *
                 * you can directly use the octal value.                          */
                mvprintw(0, 0, "Unbound key: %#o ", key);
                turn_not_consumed = 1;
        }
        refresh();
    } while (turn_not_consumed);
}