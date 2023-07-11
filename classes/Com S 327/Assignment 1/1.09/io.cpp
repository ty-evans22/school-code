/* Handles I/O with the curses library */

#include <ncurses.h>
#include <chrono>
#include <thread>
#include <future>

#include "chess.h"
#include "io.h"

/* definitions for additional key codes */
#define KEY_RETURN  10
#define KEY_ESC     27

/* definitions for color pairs */
#define WHITE_CELL_PAIR               1
#define BLACK_CELL_PAIR               2
#define PLAYER_PIECE_ON_BLACK_PAIR    3
#define PLAYER_PIECE_ON_WHITE_PAIR    4
#define PLAYER_PIECE_ON_SELECTED_PAIR 5
#define OPP_PIECE_ON_BLACK_PAIR       6
#define OPP_PIECE_ON_WHITE_PAIR       7
#define OPP_PIECE_ON_SELECTED_PAIR    8
#define OPP_PIECE_ON_VALID_PAIR       9
#define SELECTED_CELL_PAIR            10
#define CONTROLS_TEXT_PAIR            11
#define VALID_MOVE_PAIR               12
#define CAPTURE_MOVE_PAIR             13
#define COORDS_PAIR                   14

/* stores the current selected cell */
int curr_x = 0, curr_y = 7;

/* initializes curses */
void io_init_terminal(void) {
    initscr();
    raw();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    set_escdelay(100); /* fixes delay that occurs when input is ESC key */
    start_color();
    init_pair(WHITE_CELL_PAIR              , COLOR_WHITE  , COLOR_WHITE );
    init_pair(BLACK_CELL_PAIR              , COLOR_BLACK  , COLOR_BLACK );
    init_pair(PLAYER_PIECE_ON_BLACK_PAIR   , COLOR_GREEN  , COLOR_BLACK );
    init_pair(PLAYER_PIECE_ON_WHITE_PAIR   , COLOR_GREEN  , COLOR_WHITE );
    init_pair(PLAYER_PIECE_ON_SELECTED_PAIR, COLOR_GREEN  , COLOR_CYAN  );
    init_pair(OPP_PIECE_ON_BLACK_PAIR      , COLOR_RED    , COLOR_BLACK );
    init_pair(OPP_PIECE_ON_WHITE_PAIR      , COLOR_RED    , COLOR_WHITE );
    init_pair(OPP_PIECE_ON_SELECTED_PAIR   , COLOR_RED    , COLOR_CYAN  );
    init_pair(OPP_PIECE_ON_VALID_PAIR      , COLOR_RED    , COLOR_YELLOW);
    init_pair(SELECTED_CELL_PAIR           , COLOR_CYAN   , COLOR_CYAN  );
    init_pair(CONTROLS_TEXT_PAIR           , COLOR_WHITE  , COLOR_BLUE  );
    init_pair(VALID_MOVE_PAIR              , COLOR_YELLOW , COLOR_YELLOW);
    init_pair(CAPTURE_MOVE_PAIR            , COLOR_RED    , COLOR_RED   );
    init_pair(COORDS_PAIR                  , COLOR_MAGENTA, COLOR_BLACK );
}

/* ends curses and cleans up all io related stuff */
void io_reset_terminal(void) {
    endwin();
}

/* displays a single cell at the specified coords */
void io_display_cell(int y, int x, bool v_move) {
    attron(A_DIM);

    /* store coords for center of curr cell in tmp var for easy calculations */
    int tmp_y = cell_coords[y][x][0], tmp_x = cell_coords[y][x][1];

    if (x == 0) {
        /* handle cells on left side of board */
        mvaddch(tmp_y - 1, tmp_x - 4, '|');
        mvaddch(tmp_y    , tmp_x - 4, '|');
        mvaddch(tmp_y + 1, tmp_x - 4, '|');
    }
    if (y == 0) {
        /* handle cells on top side of board */
        mvaddch(tmp_y - 2, tmp_x - 3, '_');
        mvaddch(tmp_y - 2, tmp_x - 2, '_');
        mvaddch(tmp_y - 2, tmp_x - 1, '_');
        mvaddch(tmp_y - 2, tmp_x    , '_');
        mvaddch(tmp_y - 2, tmp_x + 1, '_');
        mvaddch(tmp_y - 2, tmp_x + 2, '_');
        mvaddch(tmp_y - 2, tmp_x + 3, '_');
    }

    /* activate the proper color */
    if (board.player_turn && y == curr_y && x == curr_x)
        attron(COLOR_PAIR(SELECTED_CELL_PAIR));
    else if (v_move)
        attron(COLOR_PAIR(VALID_MOVE_PAIR));
    else if ((y + x) % 2 == 0)
        attron(COLOR_PAIR(WHITE_CELL_PAIR));
    else
        attron(COLOR_PAIR(BLACK_CELL_PAIR));

    /* right side of cell */
    mvaddch(tmp_y - 1, tmp_x + 4, '|');
    mvaddch(tmp_y    , tmp_x + 4, '|');
    mvaddch(tmp_y + 1, tmp_x + 4, '|');

    /* bottom of cell */
    mvaddch(tmp_y + 1, tmp_x - 3, '_');
    mvaddch(tmp_y + 1, tmp_x - 2, '_');
    mvaddch(tmp_y + 1, tmp_x - 1, '_');
    mvaddch(tmp_y + 1, tmp_x    , '_');
    mvaddch(tmp_y + 1, tmp_x + 1, '_');
    mvaddch(tmp_y + 1, tmp_x + 2, '_');
    mvaddch(tmp_y + 1, tmp_x + 3, '_');

    /* fill rest with spaces so the whole cell is colored */
    for (int i = tmp_y - 1; i <= tmp_y; i++) {
        for (int j = tmp_x - 3; j <= tmp_x + 3; j++) {
            mvaddch(i, j, ' ');
        }
    }

    /* deactivate the proper color */
    if (board.player_turn && y == curr_y && x == curr_x)
        attroff(COLOR_PAIR(SELECTED_CELL_PAIR));
    else if (v_move)
        attroff(COLOR_PAIR(VALID_MOVE_PAIR));
    else if ((y + x) % 2 == 0)
        attroff(COLOR_PAIR(WHITE_CELL_PAIR));
    else
        attroff(COLOR_PAIR(BLACK_CELL_PAIR));

    attroff(A_DIM);
}

/* displays a single piece at the specified coords */
void io_display_piece(int y, int x, bool v_move) {
    attron(A_BOLD);

    /* activate the proper color */
    if (board.board[y][x]->is_player) {
        if (board.player_turn && y == curr_y && x == curr_x)
            attron(COLOR_PAIR(PLAYER_PIECE_ON_SELECTED_PAIR));
        else if ((y + x) % 2 == 0)
            attron(COLOR_PAIR(PLAYER_PIECE_ON_WHITE_PAIR));
        else
            attron(COLOR_PAIR(PLAYER_PIECE_ON_BLACK_PAIR));
    } else {
        if (board.player_turn && y == curr_y && x == curr_x)
            attron(COLOR_PAIR(OPP_PIECE_ON_SELECTED_PAIR));
        else if (v_move)
            attron(COLOR_PAIR(OPP_PIECE_ON_VALID_PAIR));
        else if ((y + x) % 2 == 0)
            attron(COLOR_PAIR(OPP_PIECE_ON_WHITE_PAIR));
        else
            attron(COLOR_PAIR(OPP_PIECE_ON_BLACK_PAIR));
    }

    mvaddch(cell_coords[y][x][0], cell_coords[y][x][1], piece_abbr[board.board[y][x]->type]);

    /* deactivate the proper color */
    if (board.board[y][x]->is_player) {
        if (board.player_turn && y == curr_y && x == curr_x)
            attroff(COLOR_PAIR(PLAYER_PIECE_ON_SELECTED_PAIR));
        else if ((y + x) % 2 == 0)
            attroff(COLOR_PAIR(PLAYER_PIECE_ON_WHITE_PAIR));
        else
            attroff(COLOR_PAIR(PLAYER_PIECE_ON_BLACK_PAIR));
    } else {
        if (board.player_turn && y == curr_y && x == curr_x)
            attroff(COLOR_PAIR(OPP_PIECE_ON_SELECTED_PAIR));
        else if (v_move)
            attroff(COLOR_PAIR(OPP_PIECE_ON_VALID_PAIR));
        else if ((y + x) % 2 == 0)
            attroff(COLOR_PAIR(OPP_PIECE_ON_WHITE_PAIR));
        else
            attroff(COLOR_PAIR(OPP_PIECE_ON_BLACK_PAIR));
    }

    attroff(A_BOLD);
}

/* adds the empty board to the display */
void io_display_empty_board() {
    /* add the coords for the board */
    attron(COLOR_PAIR(COORDS_PAIR));
    mvaddch(0,  7, 'a');
    mvaddch(0, 15, 'b');
    mvaddch(0, 23, 'c');
    mvaddch(0, 31, 'd');
    mvaddch(0, 39, 'e');
    mvaddch(0, 47, 'f');
    mvaddch(0, 55, 'g');
    mvaddch(0, 63, 'h');

    mvaddch( 3, 1, '8');
    mvaddch( 6, 1, '7');
    mvaddch( 9, 1, '6');
    mvaddch(12, 1, '5');
    mvaddch(15, 1, '4');
    mvaddch(18, 1, '3');
    mvaddch(21, 1, '2');
    mvaddch(24, 1, '1');
    attroff(COLOR_PAIR(COORDS_PAIR));

    /* add each of the cells to the board */
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            io_display_cell(y, x, false);
        }
    }
}

/* adds the opponent info to the display */
void io_display_opponent_info() {
    mvprintw( 2, 70, "OPPONENT");
    mvprintw( 4, 69, "Last Move:");
    mvprintw( 5, 70, "Qh4xe1"); // replace with variable
    mvprintw( 7, 69, "Captured:");
    mvprintw( 8, 70, "Q - %d", cap_count[1][1]);
    mvprintw( 9, 70, "R - %d", cap_count[1][2]);
    mvprintw(10, 70, "B - %d", cap_count[1][3]);
    mvprintw(11, 70, "N - %d", cap_count[1][4]);
    mvprintw(12, 70, "P - %d", cap_count[1][5]);
}

/* adds the player info to the display */
void io_display_player_info() {
    mvprintw(14, 70, "PLAYER");
    mvprintw(16, 69, "Last Move:");
    mvprintw(17, 70, "Qh4xe1"); // replace with variable
    mvprintw(19, 69, "Captured:");
    mvprintw(20, 70, "Q - %d", cap_count[0][1]);
    mvprintw(21, 70, "R - %d", cap_count[0][2]);
    mvprintw(22, 70, "B - %d", cap_count[0][3]);
    mvprintw(23, 70, "N - %d", cap_count[0][4]);
    mvprintw(24, 70, "P - %d", cap_count[0][5]);
}

/* adds the controls to the display */
void io_display_controls() {
    attron(COLOR_PAIR(CONTROLS_TEXT_PAIR));
    mvprintw(26, 0, " MOVE CURSOR - ARROWS | SELECT - ENTER | PREV. MOVES - M | RULES - R | QUIT - Q ");
    attroff(COLOR_PAIR(CONTROLS_TEXT_PAIR));
}

/* adds the pieces to the display */
void io_display_pieces() {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board.board[y][x]) {
                io_display_piece(y, x, false);
            }
        }
    }
}

/* updates the display for the main game screen */
void io_display() {
    clear();
    io_display_empty_board();
    io_display_opponent_info();
    io_display_player_info();
    io_display_controls();
    io_display_pieces();
    refresh();
}

/* adds the check flag to the display */
void io_display_check(bool player) {
    if (player)
        attron(COLOR_PAIR(PLAYER_PIECE_ON_BLACK_PAIR));
    else
        attron(COLOR_PAIR(OPP_PIECE_ON_BLACK_PAIR));
    mvprintw(0, 72, "CHECK");
    if (player)
        attroff(COLOR_PAIR(PLAYER_PIECE_ON_BLACK_PAIR));
    else
        attroff(COLOR_PAIR(OPP_PIECE_ON_BLACK_PAIR));
}

void io_display_checkmate(bool player) {
    if (player)
        attron(COLOR_PAIR(PLAYER_PIECE_ON_BLACK_PAIR));
    else
        attron(COLOR_PAIR(OPP_PIECE_ON_BLACK_PAIR));
    mvprintw(0, 69, "CHECKMATE");
    if (player)
        attroff(COLOR_PAIR(PLAYER_PIECE_ON_BLACK_PAIR));
    else
        attroff(COLOR_PAIR(OPP_PIECE_ON_BLACK_PAIR));
}

/* gets rid of the check flag on the display */
void io_erase_check() {
    mvprintw(0, 72, "     ");
}

/* displays the game rules to the player */
void io_display_rules() {

}

/* displays a list of all of the moves made so far in the current game */
void io_display_prev_moves() {

}

/* updates the highlighted cell when the player switches the selection */
void io_update_selected_cell(int new_y, int new_x) {
    /* store the previously selected cell coords in tmp vars */
    int tmp_y = curr_y, tmp_x = curr_x;

    /* update the currently selected cell */
    curr_y = new_y;
    curr_x = new_x;

    /* update the display for the previous and current selected cell */
    io_display_cell(tmp_y, tmp_x, false);
    io_display_cell(curr_y, curr_x, false);

    /* update the display for any pieces that were on either cell */
    if (board.board[tmp_y][tmp_x])
        io_display_piece(tmp_y, tmp_x, false);
    if (board.board[curr_y][curr_x])
        io_display_piece(curr_y, curr_x, false);
}

/* variables for blinking */
bool prev_off = true;

/* makes the selected cell to move to blink */
void io_blink_cell(int y, int x) {
    if (prev_off) {
        io_display_cell(y, x, true);
        if (board.board[y][x])
            io_display_piece(y, x, true);
        prev_off = false;
    } else {
        io_display_cell(y, x, false);
        if (board.board[y][x])
            io_display_piece(y, x, false);
        prev_off = true;
    }
}

/* returns the manhattan distance between two points */
int m_dist(int y1, int x1, int y2, int x2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

/* determines the index of the next valid move to select based on user input - nxt_dir: up = 0, down = 1, left = 2, right = 3 */
int next_idx(Piece *p, int cur_idx, int nxt_dir) {
    int c_x = p->valid_moves.at(cur_idx).m[dim_x];
    int c_y = p->valid_moves.at(cur_idx).m[dim_y];
    int r_idx = -1;

    switch (nxt_dir) {
        case 0:
            /* find closest valid move up */
            for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                if (i == cur_idx)
                    continue;
                if (p->valid_moves.at(i).m[dim_y] < c_y) {
                    if (r_idx == -1)
                        r_idx = i;
                    else
                        r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) < 
                                m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                }
            }
            /* if no valid moves up then find furthest down */
            if (r_idx == -1) {
                for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                    if (i == cur_idx)
                        continue;
                    if (p->valid_moves.at(i).m[dim_y] > c_y) {
                        if (r_idx == -1)
                            r_idx = i;
                        else
                            r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) >
                                    m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                    }
                }
            }
            /* if no valid moves up or down then return cur_idx */
            r_idx = r_idx == -1 ? cur_idx : r_idx;
            break;
        case 1:
            /* find closest valid move down */
            for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                if (i == cur_idx)
                    continue;
                if (p->valid_moves.at(i).m[dim_y] > c_y) {
                    if (r_idx == -1)
                        r_idx = i;
                    else
                        r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) < 
                                m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                }
            }
            /* if no valid moves up then find furthest up */
            if (r_idx == -1) {
                for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                    if (i == cur_idx)
                        continue;
                    if (p->valid_moves.at(i).m[dim_y] < c_y) {
                        if (r_idx == -1)
                            r_idx = i;
                        else
                            r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) >
                                    m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                    }
                }
            }
            /* if no valid moves up or down then return cur_idx */
            r_idx = r_idx == -1 ? cur_idx : r_idx;
            break;
        case 2:
            /* find closest valid move left */
            for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                if (i == cur_idx)
                    continue;
                if (p->valid_moves.at(i).m[dim_x] < c_x) {
                    if (r_idx == -1)
                        r_idx = i;
                    else
                        r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) <
                                m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                }
            }
            /* if no valid moves then find furthest right */
            if (r_idx == -1) {
                for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                    if (i == cur_idx)
                        continue;
                    if (p->valid_moves.at(i).m[dim_x] > c_x) {
                        if (r_idx == -1)
                            r_idx = i;
                        else
                            r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) >
                                    m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                    }
                }
            }
            /* if no valid moves left or right then return cur_idx */
            r_idx = r_idx == -1 ? cur_idx : r_idx;
            break;
        case 3:
            /* find closest valid move right */
            for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                if (i == cur_idx)
                    continue;
                if (p->valid_moves.at(i).m[dim_x] > c_x) {
                    if (r_idx == -1)
                        r_idx = i;
                    else
                        r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) <
                                m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                }
            }
            /* if no valid moves then find furthest left */
            if (r_idx == -1) {
                for (int i = 0; i < (int)p->valid_moves.size(); i++) {
                    if (i == cur_idx)
                        continue;
                    if (p->valid_moves.at(i).m[dim_x] < c_x) {
                        if (r_idx == -1)
                            r_idx = i;
                        else
                            r_idx = m_dist(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], c_y, c_x) >
                                    m_dist(p->valid_moves.at(r_idx).m[dim_y], p->valid_moves.at(r_idx).m[dim_x], c_y, c_x) ? i : r_idx;
                    }
                }
            }
            /* if no valid moves left or right then return cur_idx */
            r_idx = r_idx == -1 ? cur_idx : r_idx;
            break;
        default:
            return cur_idx;
    }

    return r_idx;
}

/* handles player move selection */
int io_select_cell() {
    Piece *p = board.board[curr_y][curr_x];
    p->update_valid_moves();
    
    /* display valid moves */
    for (int i = 0; i < (int)p->valid_moves.size(); i++) {
        io_display_cell(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], true);
        if (board.board[p->valid_moves.at(i).m[dim_y]][p->valid_moves.at(i).m[dim_x]])
            io_display_piece(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], true);
    }

    /* update controls */
    attron(COLOR_PAIR(CONTROLS_TEXT_PAIR));
    mvprintw(26, 0, "  MOVE CURSOR - ARROWS | SELECT MOVE - ENTER | RETURN TO PIECE SELECTION - ESC  ");
    attroff(COLOR_PAIR(CONTROLS_TEXT_PAIR));

    /* get user input */
    bool selected;
    int key, idx = 0;
    halfdelay(5);
    do {
        switch (key = getch()) {
            case KEY_UP:
                if (p->valid_moves.size() > 1) {
                    idx = next_idx(p, idx, 0);
                }
                selected = false;
                break;
            case KEY_DOWN:
                if (p->valid_moves.size() > 1) {
                    idx = next_idx(p, idx, 1);
                }
                selected = false;
                break;
            case KEY_LEFT:
                if (p->valid_moves.size() > 1) {
                    idx = next_idx(p, idx, 2);
                }
                selected = false;
                break;
            case KEY_RIGHT:
                if (p->valid_moves.size() > 1) {
                    idx = next_idx(p, idx, 3);
                }
                selected = false;
                break;
            case KEY_RETURN:
                if (idx < (int)p->valid_moves.size()) {
                    p->pos[dim_y] = p->valid_moves.at(idx).m[dim_y];
                    p->pos[dim_x] = p->valid_moves.at(idx).m[dim_x];
                    if (board.board[p->pos[dim_y]][p->pos[dim_x]]) {
                        // handle capture of opponent's piece
                        cap_count[0][board.board[p->pos[dim_y]][p->pos[dim_x]]->type]++;
                        board.board[p->pos[dim_y]][p->pos[dim_x]]->is_active = false;
                    }
                    board.board[p->pos[dim_y]][p->pos[dim_x]] = p;
                    board.board[curr_y][curr_x] = NULL;
                }
                selected = true;
                break;
            case KEY_ESC:
                selected = true;
                break;
            default:
                selected = false;
        }

        /* update cells so they display correctly */
        for (int i = 0; i < (int)p->valid_moves.size(); i++) {
            if (i != idx) {
                io_display_cell(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], true);
                if (board.board[p->valid_moves.at(i).m[dim_y]][p->valid_moves.at(i).m[dim_x]])
                    io_display_piece(p->valid_moves.at(i).m[dim_y], p->valid_moves.at(i).m[dim_x], true);
            } else
                io_blink_cell(p->valid_moves.at(idx).m[dim_y], p->valid_moves.at(idx).m[dim_x]);
        }
    } while (!selected);

    /* reset display */
    raw();

    if (p->pos[dim_y] == curr_y && p->pos[dim_x] == curr_x) {
        io_display();
        return 1;
    }

    return 0;
}

/* receives input from the user and handles execution of the corresponding action */
void io_handle_input() {
    bool turn_not_consumed;
    int key;

    /* make sure cursor is displayed initially */
    io_update_selected_cell(curr_y, curr_x);

    do {
        switch (key = getch()) {
            case KEY_UP:
                io_update_selected_cell(curr_y == 0 ? 7 : curr_y - 1, curr_x);
                turn_not_consumed = true;
                break;
            case KEY_DOWN:
                io_update_selected_cell(curr_y == 7 ? 0 : curr_y + 1, curr_x);
                turn_not_consumed = true;
                break;
            case KEY_LEFT:
                io_update_selected_cell(curr_y, curr_x == 0 ? 7 : curr_x - 1);
                turn_not_consumed = true;
                break;
            case KEY_RIGHT:
                io_update_selected_cell(curr_y, curr_x == 7 ? 0 : curr_x + 1);
                turn_not_consumed = true;
                break;
            case KEY_RETURN:
                if (board.board[curr_y][curr_x] && board.board[curr_y][curr_x]->is_player) {
                    if (io_select_cell() == 0)
                        turn_not_consumed = false;
                    else
                        turn_not_consumed = true;
                }
                break;
            case 'M':
            case 'm':
                io_display_prev_moves();
                io_display();
                turn_not_consumed = true;
                break;
            case 'R':
            case 'r':
                io_display_rules();
                io_display();
                turn_not_consumed = true;
                break;
            case 'Q':
            case 'q':
                board.quit_game = true;
                turn_not_consumed = false;
                break;
            default:
                /* any unhandled input is ignored */
                turn_not_consumed = true;
        }
    } while (turn_not_consumed);
}

// new window size: 80 columns x 27 rows

/**********************************************************************************/
/*       a       b       c       d       e       f       g       h     CHECKMATE  */
/*    _______ _______ _______ _______ _______ _______ _______ _______             */
/*   |       |       |       |       |       |       |       |       |  OPPONENT  */
/* 8 |   R   |   N   |   B   |   Q   |   K   |   B   |   N   |   R   |            */
/*   |_______|_______|_______|_______|_______|_______|_______|_______| Last Move: */
/*   |       |       |       |       |       |       |       |       |  Qh4xe1    */
/* 7 |   P   |   P   |   P   |   P   |   P   |   P   |   P   |   P   |            */
/*   |_______|_______|_______|_______|_______|_______|_______|_______| Captured:  */
/*   |       |       |       |       |       |       |       |       |  Q - 0     */
/* 6 |       |       |       |       |       |       |       |       |  R - 0     */
/*   |_______|_______|_______|_______|_______|_______|_______|_______|  B - 0     */
/*   |       |       |       |       |       |       |       |       |  N - 0     */
/* 5 |       |       |       |       |       |       |       |       |  P - 0     */
/*   |_______|_______|_______|_______|_______|_______|_______|_______|            */
/*   |       |       |       |       |       |       |       |       |   PLAYER   */
/* 4 |       |       |       |       |       |       |       |       |            */
/*   |_______|_______|_______|_______|_______|_______|_______|_______| Last Move: */
/*   |       |       |       |       |       |       |       |       |  Qh4xe1    */
/* 3 |       |       |       |       |       |       |       |       |            */
/*   |_______|_______|_______|_______|_______|_______|_______|_______| Captured:  */
/*   |       |       |       |       |       |       |       |       |  Q - 0     */
/* 2 |   P   |   P   |   P   |   P   |   P   |   P   |   P   |   P   |  R - 0     */
/*   |_______|_______|_______|_______|_______|_______|_______|_______|  B - 0     */
/*   |       |       |       |       |       |       |       |       |  N - 0     */
/* 1 |   R   |   N   |   B   |   Q   |   K   |   B   |   N   |   R   |  P - 0     */
/*   |_______|_______|_______|_______|_______|_______|_______|_______|            */
/* MOVE CURSOR - ARROWS | SELECT - ENTER | PREV. MOVES - M | RULES - R | QUIT - Q */
/**********************************************************************************/

/*  MOVE CURSOR - ARROWS | SELECT MOVE - ENTER | RETURN TO PIECE SELECTION - ESC  */