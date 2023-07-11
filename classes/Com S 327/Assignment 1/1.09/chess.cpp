#include <iostream>
#include <vector>
#include <string>

#include "chess.h"

using std::vector; using std::string;

/* define the weights for opponent move calculations */
#define KING_WEIGHT         200
#define QUEEN_WEIGHT        9
#define ROOK_WEIGHT         5
#define BISHOP_WEIGHT       3
#define KNIGHT_WEIGHT       3
#define PAWN_WEIGHT         1
#define DSI_WEIGHT          0.5
#define MOBILITY_WEIGHT     0.1

/* define the extern vars declared in the header */
Board board;

/* contains the shorthand abbreviation for each piece type */
char piece_abbr[7] = {'K', 'Q', 'R', 'B', 'N', 'P', ' '};

// Piece_2 empty_piece;

pair_t cap_count[6] = {
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0},
    {0, 0}
};

/* contains the coords to the center of each cell */
pair_t cell_coords[8][8] = {
    {{ 3,  7}, { 3, 15}, { 3, 23}, { 3, 31}, { 3, 39}, { 3, 47}, { 3, 55}, { 3, 63}},
    {{ 6,  7}, { 6, 15}, { 6, 23}, { 6, 31}, { 6, 39}, { 6, 47}, { 6, 55}, { 6, 63}},
    {{ 9,  7}, { 9, 15}, { 9, 23}, { 9, 31}, { 9, 39}, { 9, 47}, { 9, 55}, { 9, 63}},
    {{12,  7}, {12, 15}, {12, 23}, {12, 31}, {12, 39}, {12, 47}, {12, 55}, {12, 63}},
    {{15,  7}, {15, 15}, {15, 23}, {15, 31}, {15, 39}, {15, 47}, {15, 55}, {15, 63}},
    {{18,  7}, {18, 15}, {18, 23}, {18, 31}, {18, 39}, {18, 47}, {18, 55}, {18, 63}},
    {{21,  7}, {21, 15}, {21, 23}, {21, 31}, {21, 39}, {21, 47}, {21, 55}, {21, 63}},
    {{24,  7}, {24, 15}, {24, 23}, {24, 31}, {24, 39}, {24, 47}, {24, 55}, {24, 63}}
};

/* contains multipliers to calculate cell positions in each direction */
pair_t dir_multipliers[8] = {
    {-1,  0},
    {-1,  1},
    { 0,  1},
    { 1,  1},
    { 1,  0},
    { 1, -1},
    { 0, -1},
    {-1, -1}
};

/* contains values used to calculate valid moves for knights */
pair_t knight_moves[8] = {
    { 2, -1},
    { 2,  1},
    {-2, -1},
    {-2,  1},
    {-1,  2},
    { 1,  2},
    {-1, -2},
    { 1, -2}
};

/* contains max possible move distance for king, queen, rook, and bishop */
int piece_max_dist[4][8] = {
    {1, 1, 1, 1, 1, 1, 1, 1},
    {7, 7, 7, 7, 7, 7, 7, 7},
    {7, 0, 7, 0, 7, 0, 7, 0},
    {0, 7, 0, 7, 0, 7, 0, 7}
};

/* utility function used to check if a move would cause the movee's king to become in check - returns true if valid */
bool check_king(Piece *p, int new_y, int new_x) {
    /* simulate the move */
    Piece *tmp = board.board[new_y][new_x] ? board.board[new_y][new_x] : NULL;
    board.board[p->pos[dim_y]][p->pos[dim_x]] = NULL;
    board.board[new_y][new_x] = p;
    bool res = true;

    /* find pos of movee's king */
    pair_t king_pos = {-1, -1};
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board.board[y][x] && board.board[y][x]->type == piece_king && board.board[y][x]->is_player == p->is_player) {
                king_pos[dim_y] = y;
                king_pos[dim_x] = x;
                break;
            }
        }
        if (king_pos[dim_y] != -1 && king_pos[dim_x] != -1)
            break;
    }

    /* check if king is in check */
    for (int i = 0; i < 8 && res; i++) {
        /* check each direction until edge of map or piece encountered */
        for (int j = 1; j < 8; j++) {
            pair_t g = {king_pos[dim_y] + (dir_multipliers[i][dim_y] * j), king_pos[dim_x] + (dir_multipliers[i][dim_x] * j)};
            if (g[dim_y] < 0 || g[dim_y] >= BOARD_SIZE || g[dim_x] < 0 || g[dim_x] >= BOARD_SIZE)
                break;
            if (board.board[g[dim_y]][g[dim_x]]) {
                if (board.board[g[dim_y]][g[dim_x]]->is_player != p->is_player) {
                    // can enemy reach king? >> invalid
                    Piece *en = board.board[g[dim_y]][g[dim_x]];
                    if (en->type == piece_queen || (en->type == piece_bishop && i % 2 == 1) || (en->type == piece_rook && i % 2 == 0) ||
                         (en->type == piece_pawn && ((p->is_player && (i == 2 || i == 8)) || (!p->is_player && (i == 4 || i == 6)))))
                        res = false;
                }
                break;
            }
        }

        /* check each possible location for an enemy knight */
        pair_t h = {king_pos[dim_y] + knight_moves[i][dim_y], king_pos[dim_x] + knight_moves[i][dim_x]};
        if ((h[dim_y] >= 0 && h[dim_y] < BOARD_SIZE && h[dim_x] >= 0 && h[dim_x] < BOARD_SIZE) && board.board[h[dim_y]][h[dim_x]] && 
             board.board[h[dim_y]][h[dim_x]]->type == piece_knight && board.board[h[dim_y]][h[dim_x]]->is_player != p->is_player)
            res = false;
    }

    /* undo move */
    board.board[p->pos[dim_y]][p->pos[dim_x]] = p;
    board.board[new_y][new_x] = tmp ? tmp : NULL;

    return res;
}

// /* returns the char representation of the piece */
// char Piece_2::symbol() {
//     return piece_abbr[this->piece_type];
// }

// /* returns a string containing the UCI representation of the move */
// string Move_2::uci() {
//     // implement later
// }

/* temporarily stores the valid moves */
void Piece::store_moves() {
    this->tmp_moves.clear();
    for (int i = 0; i < (int)this->valid_moves.size(); i++) {
        tmp_moves.push_back(valid_moves.at(i));
    }
}

/* restores the valid moves from the temporary storage */
void Piece::restore_moves() {
    this->valid_moves.clear();
    for (int i = 0; i < (int)this->tmp_moves.size(); i++) {
        valid_moves.push_back(tmp_moves.at(i));
    }
}

/* updates the list of valid moves for a piece */
void Piece::update_valid_moves() {
    /* start by clearing previous valid moves */
    this->valid_moves.clear();

    /* special case if knight */
    if (this->type == piece_knight) {
        /* go through each possible knight move and check for validity */
        for (int i = 0; i < 8; i++) {
            pair_t t = {this->pos[dim_y] + knight_moves[i][dim_y], this->pos[dim_x] + knight_moves[i][dim_x]};
            if ((t[dim_y] >= 0 && t[dim_y] < BOARD_SIZE && t[dim_x] >= 0 && t[dim_x] < BOARD_SIZE) &&
                 (!board.board[t[dim_y]][t[dim_x]] || board.board[t[dim_y]][t[dim_x]]->is_player != this->is_player) &&
                 check_king(this, t[dim_y], t[dim_x])) {
                Move q;
                q.m[dim_x] = t[dim_x];
                q.m[dim_y] = t[dim_y];
                q.start[dim_x] = this->pos[dim_x];
                q.start[dim_y] = this->pos[dim_y];
                this->valid_moves.push_back(q);
            }
        }
        return;
    }

    /* special case if pawn */
    if (this->type == piece_pawn) {
        /* determine the direction pawn travels */
        int multiplier = this->is_player ? 1 : -1;

        /* validate movement one space forward */
        pair_t v = {this->pos[dim_y] - multiplier, this->pos[dim_x]};
        if (v[dim_y] >= 0 && v[dim_y] < BOARD_SIZE && !board.board[v[dim_y]][v[dim_x]] && check_king(this, v[dim_y], v[dim_x])) {
            Move q;
            q.m[dim_x] = v[dim_x];
            q.m[dim_y] = v[dim_y];
            q.start[dim_x] = this->pos[dim_x];
            q.start[dim_y] = this->pos[dim_y];
            this->valid_moves.push_back(q);
        }

        /* check if its the pawn's first move */
        if ((this->is_player && this->pos[dim_y] == BOARD_SIZE - 2) || (!this->is_player && this->pos[dim_y] == 1)) {
            /* validate movement two spaces forward */
            pair_t t = {this->pos[dim_y] - (2 * multiplier), this->pos[dim_x]};
            if (!board.board[t[dim_y]][t[dim_x]] && check_king(this, t[dim_y], t[dim_x])) {
                Move q;
                q.m[dim_x] = t[dim_x];
                q.m[dim_y] = t[dim_y];
                q.start[dim_x] = this->pos[dim_x];
                q.start[dim_y] = this->pos[dim_y];
                this->valid_moves.push_back(q);
            }
        }

        /* check if pawn can attack */
        pair_t left = {this->pos[dim_y] - multiplier, this->pos[dim_x] - 1};
        pair_t right = {this->pos[dim_y] - multiplier, this->pos[dim_x] + 1};
        if (left[dim_x] >= 0 && board.board[left[dim_y]][left[dim_x]] && (board.board[left[dim_y]][left[dim_x]]->is_player !=
             this->is_player) && check_king(this, left[dim_y], left[dim_x])) {
            Move q;
            q.m[dim_x] = left[dim_x];
            q.m[dim_y] = left[dim_y];
            q.start[dim_x] = this->pos[dim_x];
            q.start[dim_y] = this->pos[dim_y];
            this->valid_moves.push_back(q);
        }
        if (right[dim_x] < BOARD_SIZE && board.board[right[dim_y]][right[dim_x]] &&
             (board.board[right[dim_y]][right[dim_x]]->is_player != this->is_player) && check_king(this, right[dim_y], right[dim_x])) {
            Move q;
            q.m[dim_x] = right[dim_x];
            q.m[dim_y] = right[dim_y];
            q.start[dim_x] = this->pos[dim_x];
            q.start[dim_y] = this->pos[dim_y];
            this->valid_moves.push_back(q);
        }
        return;
    }

    /* check each direction */
    for (int i = 0; i < 8; i++) {
        /* check each possible spot in this direction */
        for (int j = 1; j <= piece_max_dist[this->type][i]; j++) {
            /* store coords to check in tmp var */
            pair_t t = {this->pos[dim_y] + (dir_multipliers[i][dim_y] * j), this->pos[dim_x] + (dir_multipliers[i][dim_x] * j)};

            /* make sure this spot is in bounds */
            if (t[dim_y] >= BOARD_SIZE || t[dim_y] < 0 || t[dim_x] >= BOARD_SIZE || t[dim_x] < 0)
                break;

            /* check if this spot is occupied */
            if (board.board[t[dim_y]][t[dim_x]]) {
                /* check if friendly or enemy */
                if (board.board[t[dim_y]][t[dim_x]]->is_player != this->is_player && check_king(this, t[dim_y], t[dim_x])) {
                    Move q;
                    q.m[dim_x] = t[dim_x];
                    q.m[dim_y] = t[dim_y];
                    q.start[dim_x] = this->pos[dim_x];
                    q.start[dim_y] = this->pos[dim_y];
                    this->valid_moves.push_back(q);
                }
                break;
            }

            /* check if move would put own king in check */
            if (check_king(this, t[dim_y], t[dim_x])) {
                Move q;
                q.m[dim_x] = t[dim_x];
                q.m[dim_y] = t[dim_y];
                q.start[dim_x] = this->pos[dim_x];
                q.start[dim_y] = this->pos[dim_y];
                this->valid_moves.push_back(q);
            }
        }
    }
}

// /* define the functions for the Board_2 class */

// /* returns piece at pos and removes it - if no piece then returns empty_piece */
// Piece_2 Board_2::remove_piece_at(pair_t pos) {
//     Piece_2 tmp = this->board[pos[dim_y]][pos[dim_x]];
//     this->board[pos[dim_y]][pos[dim_x]] = empty_piece;
//     return tmp;
// }

// /* set the piece at the specified position */
// void Board_2::set_piece_at(pair_t pos, Piece_2 p, bool promotion = false) {
//     this->board[pos[dim_y]][pos[dim_x]] = p;
//     // update promotion once implemented
// }

// /* pushes a move into the stack */
// void Board_2::push(Move_2 move) {
//     this->move_stack.push_back(move);
// }

// /* returns the last move that was pushed into the stack and removes it from the stack */
// Move_2 Board_2::pop() {
//     Move_2 tmp = this->move_stack.back();
//     this->move_stack.pop_back();
//     return tmp;
// }

// /* returns the last move that was pushed into the stack without removing it */
// Move_2 Board_2::peek() {
//     return this->move_stack.back();
// }

// /* returns the piece at the position */
// Piece_2 Board_2::piece_at(pair_t pos) {
//     return this->board[pos[dim_y]][pos[dim_x]];
// }

// /* returns the piece type at the position */
// piece_type_t Board_2::piece_type_at(pair_t pos) {
//     return piece_at(pos).piece_type;
// }

// /* returns the color at the position */
// color_t Board_2::color_at(pair_t pos) {
//     return piece_at(pos).color;
// }

// /* returns the position of the king of the specified color */
// pair_t* king(color_t color) {
//     for (int y = 0; y < BOARD_SIZE; y++) {
//         for (int x = 0; x < BOARD_SIZE; x++) {
//             if (this->board[y][x].piece_type == piece_king && this->board[y][x].color == color) {
//                 pair_t p;
//                 p[dim_y] = y;
//                 p[dim_x] = x;
//                 return &p;
//             }
//         }
//     }
// }

/* checks if the player or opponent is in check */
bool is_check(bool player) {
    bool res = false;

    /* get the king's position */
    int k_x = -1, k_y = -1;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board.board[y][x] && board.board[y][x]->type == piece_king && board.board[y][x]->is_player == player) {
                k_x = board.board[y][x]->pos[dim_x];
                k_y = board.board[y][x]->pos[dim_y];
                break;
            }
        }
        if (k_x != -1 && k_y != -1)
            break;
    }

    /* check for threats */
    for (int i = 0; i < 8 && !res; i++) {
        for (int j = 1; j < 8; j++) {
            pair_t g = {k_y + (dir_multipliers[i][dim_y] * j), k_x + (dir_multipliers[i][dim_x] * j)};
            if (g[dim_y] < 0 || g[dim_y] >= BOARD_SIZE || g[dim_x] < 0 || g[dim_x] >= BOARD_SIZE)
                break;
            if (board.board[g[dim_y]][g[dim_x]]) {
                if (board.board[g[dim_y]][g[dim_x]]->is_player != player) {
                    // can enemy reach king? >> invalid
                    Piece *en = board.board[g[dim_y]][g[dim_x]];
                    if (en->type == piece_queen || (en->type == piece_bishop && i % 2 == 1) || (en->type == piece_rook && i % 2 == 0) ||
                         (en->type == piece_pawn && ((player && (i == 2 || i == 8)) || (!player && (i == 4 || i == 6)))))
                        res = true;
                }
                break;
            }
        }

        /* check each possible location for an enemy knight */
        pair_t h = {k_y + knight_moves[i][dim_y], k_x + knight_moves[i][dim_x]};
        if ((h[dim_y] >= 0 && h[dim_y] < BOARD_SIZE && h[dim_x] >= 0 && h[dim_x] < BOARD_SIZE) && board.board[h[dim_y]][h[dim_x]] && 
             board.board[h[dim_y]][h[dim_x]]->type == piece_knight && board.board[h[dim_y]][h[dim_x]]->is_player != player)
            res = true;
    }

    return res;
}

/* returns true if there are no possible moves by a player to get them out of check - player = true if checking for player */
bool is_checkmate(bool player) {
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board.board[y][x] && board.board[y][x]->is_player == player) {
                board.board[y][x]->update_valid_moves();
                if (board.board[y][x]->valid_moves.size() > 0)
                    return false;
            }
        }
    }
    return true;
}

/* evaluates a specific move and returns a score for it */
double evaluate_move(Move *m, Piece *p) {
    /* simulate move */
    Piece *tmp = board.board[m->m[dim_y]][m->m[dim_x]] ? board.board[m->m[dim_y]][m->m[dim_x]] : NULL;
    if (tmp)
        tmp->is_active = false;
    int d_x = m->m[dim_x], d_y = m->m[dim_y];
    board.board[p->pos[dim_y]][p->pos[dim_x]] = NULL;
    board.board[d_y][d_x] = p;

    /* do calculations */
    int num_plyr_pieces[6] = {0, 0, 0, 0, 0, 0}, num_opp_pieces[6] = {0, 0, 0, 0, 0, 0}, num_plyr_moves = 0, 
        num_opp_moves = 0, num_plyr_dsi = 0, num_opp_dsi = 0;
    /* count the number of pieces for each side */
    for (int i = 0; i < 32; i++) {
        if (board.pieces[i]->is_active) {
            board.pieces[i]->store_moves();
            board.pieces[i]->update_valid_moves();
            if (board.pieces[i]->is_player) {
                num_plyr_pieces[board.pieces[i]->type]++;
                num_plyr_moves += board.pieces[i]->valid_moves.size();
                if (board.pieces[i]->type == piece_pawn && board.pieces[i]->valid_moves.size() == 0)
                    num_plyr_dsi++;
            } else {
                num_opp_pieces[board.pieces[i]->type]++;
                num_opp_moves += board.pieces[i]->valid_moves.size();
                if (board.pieces[i]->type == piece_pawn && board.pieces[i]->valid_moves.size() == 0)
                    num_opp_dsi++;
            }
        }
    }
    /* count number of pawns in each column */
    int p_count[8], o_count[8];
    for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (board.board[y][x] && board.board[y][x]->type == piece_pawn) {
                if (board.board[y][x]->is_player)
                    p_count[x]++;
                else
                    o_count[x]++;
            }
        }
    }
    /* count number of doubled and isolated pawns */
    for (int i = 0; i < 8; i++) {
        /* count number of doubled pawns for player and opponent */
        if (p_count[i] > 1)
            num_plyr_dsi++;
        if (o_count[i] > 1)
            num_opp_dsi++;
        
        /* count number of player's isolated pawns */
        if (i == 0 && p_count[i] > 0 && p_count[i + 1] == 0)
            num_plyr_dsi += p_count[i];
        else if (i == 7 && p_count[i] > 0 && p_count[i - 1] == 0)
            num_plyr_dsi += p_count[i];
        else if (p_count[i] > 0 && p_count[i - 1] == 0 && p_count[i + 1] == 0)
            num_plyr_dsi += p_count[i];

        /* count number of opponent's isolated pawns */
        if (i == 0 && o_count[i] > 0 && o_count[i + 1] == 0)
            num_opp_dsi += o_count[i];
        else if (i == 7 && o_count[i] > 0 && o_count[i - 1] == 0)
            num_opp_dsi += o_count[i];
        else if (o_count[i] > 0 && o_count[i - 1] == 0 && o_count[i + 1] == 0)
            num_opp_dsi += o_count[i];
    }

    /* calculate score */
    double eval = KING_WEIGHT     * (num_opp_pieces[piece_king]   - num_plyr_pieces[piece_king]  )
                + QUEEN_WEIGHT    * (num_opp_pieces[piece_queen]  - num_plyr_pieces[piece_queen] )
                + ROOK_WEIGHT     * (num_opp_pieces[piece_rook]   -  num_plyr_pieces[piece_rook] )
                + BISHOP_WEIGHT   * (num_opp_pieces[piece_bishop] - num_plyr_pieces[piece_bishop])
                + KNIGHT_WEIGHT   * (num_opp_pieces[piece_knight] - num_plyr_pieces[piece_knight])
                + PAWN_WEIGHT     * (num_opp_pieces[piece_pawn]   - num_plyr_pieces[piece_pawn]  )
                - DSI_WEIGHT      * (num_opp_dsi - num_plyr_dsi)
                + MOBILITY_WEIGHT * (num_opp_moves - num_plyr_moves);

    /* reset move */
    if (tmp)
        tmp->is_active = true;
    board.board[p->pos[dim_y]][p->pos[dim_x]] = p;
    board.board[d_y][d_x] = tmp;
    for (int i = 0; i < 32; i++) {
        if (board.pieces[i]->is_active) {
            board.pieces[i]->restore_moves();
        }
    }

    return eval;
}

/* evaluates all possible moves for the opponent and returns the best one */
Move *get_opp_move() {
    Move *m = new Move;
    m->eval = -10000.0;

    /* get evaluation for each possible move */
    for (int i = 0; i < 16; i++) {
        if (!board.pieces[i]->is_player && board.pieces[i]->is_active) {
            board.pieces[i]->update_valid_moves();
            for (int j = 0; j < (int)board.pieces[i]->valid_moves.size(); j++) {
                if (evaluate_move(&board.pieces[i]->valid_moves.at(j), board.pieces[i]) > m->eval) {
                    m = &board.pieces[i]->valid_moves.at(j);
                    m->start[dim_y] = board.pieces[i]->pos[dim_y];
                    m->start[dim_x] = board.pieces[i]->pos[dim_x];
                }
            }
        }
    }

    return m;
}

// /* may not actually be needed but oh well */
// Move empty_move;

// vector<Move> get_legal_moves(color_t cur, Board_2 b) {
//     for (int y = 0; y < BOARD_SIZE; y++) {
//         for (int x = 0; x < BOARD_SIZE; x++) {
//             if (b.board[y][x].piece_type != piece_none && b.board[y][x].color == cur) {
//                 // add valid moves
//             }
//         }
//     }
// }

// /* new and updated version of move selection relying on min-max algorithm w/ alpha-beta pruning */
// Move get_ai_move(int depth) {
//     Move best_move = empty_move;
//     int best_value = -99999;
//     int alpha = -100000;
//     int beta = 100000;

//     return best_move;
// }

/* initializes and sets up the board */
void init_board() {
    // /* intitialize empty piece */
    // empty_piece.piece_type = piece_none;

    /* initialize all the pieces */
    int count = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (y < 2 || y > 5) {
                Piece *p = new Piece;
                p->is_active = true;
                p->is_player = y > 5;
                p->pos[dim_y] = y;
                p->pos[dim_x] = x;
                board.pieces[count] = p;
                board.board[y][x] = p;
                count++;
            } else
                board.board[y][x] = NULL;
        }
    }

    /* set the type for each piece */

    /* opponent pieces */
    board.board[0][0]->type = piece_rook;
    board.board[0][1]->type = piece_knight;
    board.board[0][2]->type = piece_bishop;
    board.board[0][3]->type = piece_queen;
    board.board[0][4]->type = piece_king;
    board.board[0][5]->type = piece_bishop;
    board.board[0][6]->type = piece_knight;
    board.board[0][7]->type = piece_rook;

    board.board[1][0]->type = piece_pawn;
    board.board[1][1]->type = piece_pawn;
    board.board[1][2]->type = piece_pawn;
    board.board[1][3]->type = piece_pawn;
    board.board[1][4]->type = piece_pawn;
    board.board[1][5]->type = piece_pawn;
    board.board[1][6]->type = piece_pawn;
    board.board[1][7]->type = piece_pawn;

    /* player pieces */
    board.board[7][0]->type = piece_rook;
    board.board[7][1]->type = piece_knight;
    board.board[7][2]->type = piece_bishop;
    board.board[7][3]->type = piece_queen;
    board.board[7][4]->type = piece_king;
    board.board[7][5]->type = piece_bishop;
    board.board[7][6]->type = piece_knight;
    board.board[7][7]->type = piece_rook;

    board.board[6][0]->type = piece_pawn;
    board.board[6][1]->type = piece_pawn;
    board.board[6][2]->type = piece_pawn;
    board.board[6][3]->type = piece_pawn;
    board.board[6][4]->type = piece_pawn;
    board.board[6][5]->type = piece_pawn;
    board.board[6][6]->type = piece_pawn;
    board.board[6][7]->type = piece_pawn;

    /* initialize any other vars */
    board.quit_game = false;
    board.player_turn = false;
}