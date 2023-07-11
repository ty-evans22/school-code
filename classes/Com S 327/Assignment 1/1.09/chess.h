#ifndef CHESS_H
#define CHESS_H

#include <cstdint>
#include <vector>
#include <string>

using std::vector; using std::string;

#define BOARD_SIZE 8

typedef enum dim {
    dim_y,
    dim_x,
    num_dims
} dim_t;

// typedef enum color {
//     white,
//     black
// } color_t;

/* utility type used to store coordinates*/
typedef int pair_t[num_dims];

/* object representation of a move */
class Move {
    public:
        pair_t m;
        double eval;
        pair_t start;
};

typedef enum __attribute__ ((__packed__)) piece_type {
    piece_king,
    piece_queen,
    piece_rook,
    piece_bishop,
    piece_knight,
    piece_pawn,
    piece_none
} piece_type_t;

// /* new implementation of piece */
// class Piece_2 {
//     public:
//         piece_type_t piece_type;
//         color_t color;
//         char symbol();
// };

class Piece {
    public:
        piece_type_t type;
        bool is_player;
        bool is_active;
        pair_t pos;
        vector<Move> valid_moves;
        vector<Move> tmp_moves;

        void update_valid_moves();
        void store_moves();
        void restore_moves();
};

// /* new implementation of move */
// class Move_2 {
//     public:
//         piece_type_t moved_piece;
//         pair_t from;
//         pair_t to;
//         piece_type_t promotion;
//         piece_type_t drop;
//         string uci();
// };

// class Board_2 {
//     public:
//         Piece_2 board[BOARD_SIZE][BOARD_SIZE];
//         color turn; // color who's turn it currently is */
//         int fullmove_number; // counts move pairs - starts at 1 and is incremented after every black move
//         int halfmove_clock; // num of half moves since last capture or pawn move
//         vector<Move_2> move_stack;
//         Piece_2 remove_piece_at(pair_t pos);
//         void set_piece_at(pair_t pos, Piece_2 p, bool promotion = false);
//         void push(Move_2 move);
//         Move_2 pop();
//         Move_2 peek();
//         Piece_2 piece_at(pair_t pos);
//         piece_type_t piece_type_at(pair_t pos);
//         color_t color_at(pair_t pos);
//         pair_t* king (color_t color);
// };

class Board {
    public:
        Piece *board[BOARD_SIZE][BOARD_SIZE];
        Piece *pieces[32];
        bool quit_game;
        bool player_turn;
};

/* declare any extern vars needed globally */
extern Board board;
extern char piece_abbr[7];
extern pair_t cell_coords[8][8];
extern pair_t cap_count[6];
// extern Piece_2 empty_piece;

Move *get_opp_move();
bool is_check(bool player);
bool is_checkmate(bool player);
void init_board();

#endif