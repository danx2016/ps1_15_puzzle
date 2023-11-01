#ifndef _PUZZLE_H
#define _PUZZLE_H

#include <stdint.h>
#include <stdbool.h>
#include <libgpu.h>

#define PUZZLE_EMPTY_TILE_ID 15

// up=0110 down=1110 left=1001 right=1011
typedef enum
{
    NONE  =  0,
    UP    =  6, 
    DOWN  = 14, 
    LEFT  =  9, 
    RIGHT = 11     
} PUZZLE_DIRECTION;

typedef struct
{
    uint8_t src_num;
    SVECTOR src; // (src_col, src_row, 0)    
    SVECTOR dst; // (dst_col, dst_row, 0)
    PUZZLE_DIRECTION direction;    
} PUZZLE_MOVE;

extern uint8_t puzzle_table[4][4]; // table[row][col]

extern int8_t puzzle_empty_col;
extern int8_t puzzle_empty_row;

extern int32_t puzzle_moves_count;

extern void puzzle_reset(void);
extern void puzzle_shuffle(uint32_t num_shuffles);
extern bool puzzle_move_start(PUZZLE_DIRECTION direction, PUZZLE_MOVE *move); 
extern void puzzle_move_commit(PUZZLE_MOVE *move); 
extern bool puzzle_is_cleared(); 

#endif /* _PUZZLE_H */