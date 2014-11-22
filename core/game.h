#ifndef TRN_GAME_H
#define TRN_GAME_H

#include "grid.h"
#include "piece.h"


typedef enum { TRN_GAME_ON, TRN_GAME_OVER, TRN_GAME_PAUSED} TrnGameStatus;

static int const NINTENDO_SCORING[5] = {0, 40, 100, 300, 1200};




typedef struct {
    TrnGameStatus status;
    TrnGrid* grid;
    TrnPiece* current_piece;
    TrnPiece* next_piece;
    int score;
    int lines_count;
    int level;
} Game;

void trn_game_next_piece(Game * const game);

Game* trn_game_new(int const numberOfRows, int const numberOfColumns);

void trn_game_destroy(Game * game);

bool trn_game_try_to_move(Game* game,
                          void (*move)(TrnPiece * const),
                          void (*unmove)(TrnPiece * const));

bool trn_game_try_to_move_right(Game * const game);

bool trn_game_try_to_move_left(Game * const game);

bool trn_game_try_to_move_bottom(Game * const game);

bool trn_game_try_to_rotate_clockwise(Game * const game);

int trn_game_update_score(Game* game, int const lines_count);

#endif
