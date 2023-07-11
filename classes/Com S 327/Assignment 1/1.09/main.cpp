#include <ncurses.h>

#include "chess.h"
#include "io.h"

void game_loop() {
    while (!board.quit_game) {
        /* player turn */
        board.player_turn = true;
        io_handle_input();
        board.player_turn = false;

        /* update the board */
        io_display();

        /* check for check/checkmate */
        if (is_checkmate(false)) {
            io_display_checkmate(false);
            getch();
            board.quit_game = true; // replace with something to signify winning
        }
        if (is_check(false))
            io_display_check(false);
        else
            io_erase_check();
        
        /* opponent turn */
        Move *m = get_opp_move();
        int m_x = m->m[dim_x], m_y = m->m[dim_y];
        if (board.board[m_y][m_x] && board.board[m_y][m_x]->is_player) {
            // handle capture of player piece
            cap_count[1][board.board[m_y][m_x]->type]++;
            board.board[m_y][m_x]->is_active = false;
        }
        board.board[m->m[dim_y]][m->m[dim_x]] = board.board[m->start[dim_y]][m->start[dim_x]];
        board.board[m->start[dim_y]][m->start[dim_x]] = NULL;
        board.board[m->m[dim_y]][m->m[dim_x]]->pos[dim_y] = m->m[dim_y];
        board.board[m->m[dim_y]][m->m[dim_x]]->pos[dim_x] = m->m[dim_x];

        /* update the board */
        io_display();

        /* check for check/checkmate */
        if (is_checkmate(true)) {
            io_display_checkmate(true);
            getch();
            board.quit_game = true; // replace with something to signify winning
        }
        if (is_check(true))
            io_display_check(true);
        else
            io_erase_check();
    }
}

int main(int argc, char *argv[]) {
    /* initialize curses */
    io_init_terminal();

    /* initialize the chess board */
    init_board();

    /* display the initial board */
    io_display();

    /* run through the game loop until the player quits */
    game_loop();

    /* deactivate curses and cleanup */
    io_reset_terminal();
}