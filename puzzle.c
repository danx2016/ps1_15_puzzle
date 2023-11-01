#include <stddef.h>
#include <rand.h>

#include "puzzle.h"

uint8_t puzzle_table[4][4]; // table[row][col]

int8_t puzzle_empty_col;
int8_t puzzle_empty_row;

int32_t puzzle_moves_count;

void puzzle_reset(void)
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            puzzle_table[row][col] = row * 4 + col;
        }
    }
    puzzle_empty_col = 3;
    puzzle_empty_row = 3;
    puzzle_moves_count = 0;
}

void puzzle_shuffle(uint32_t num_shuffles)
{
    PUZZLE_DIRECTION directions[] = { LEFT, RIGHT, UP, DOWN };
    PUZZLE_MOVE puzzle_move;
    for (int i = 0; i < num_shuffles; i++)
    {
        PUZZLE_DIRECTION direction = directions[rand() % 4];
        if (puzzle_move_start(direction, &puzzle_move))
        {
            puzzle_move_commit(&puzzle_move);
        }
    }
    puzzle_moves_count = 0;
}

bool puzzle_move_start(PUZZLE_DIRECTION direction, PUZZLE_MOVE *move)
{
    int32_t dc = (direction & 3) - 2;
    int32_t dr = (direction >> 2) - 2;

    size_t nc = puzzle_empty_col - dc;
    size_t nr = puzzle_empty_row - dr;

    if (nc < 0 || nr < 0 || nc > 3 || nr > 3)
    {
        return false;
    }

    move->src_num = puzzle_table[nr][nc];
    puzzle_table[nr][nc] = PUZZLE_EMPTY_TILE_ID;
    setVector(&move->src, nc, nr, 0);
    setVector(&move->dst, puzzle_empty_col, puzzle_empty_row, 0);
    move->direction = direction;
}

void puzzle_move_commit(PUZZLE_MOVE *move)
{
    puzzle_empty_col = move->src.vx;
    puzzle_empty_row = move->src.vy;
    puzzle_table[move->dst.vy][move->dst.vx] = move->src_num;

    // update moves count
    puzzle_moves_count++;
    if (puzzle_moves_count > 999999)
    {
        puzzle_moves_count = 999999;
    }
}

bool puzzle_is_cleared()
{
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            if (puzzle_table[row][col] != row * 4 + col)
            {
                return false;
            }
        }
    }
    return true;    
}