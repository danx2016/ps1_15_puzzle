#include <stdio.h>
#include <rand.h>
#include <libetc.h>

#include "scene_factory.h"
#include "gfx.h"
#include "font.h"
#include "input.h"
#include "audio.h"
#include "puzzle.h"
#include "game.h"

typedef enum
{
    GAME_STATE_WAITING_INPUT,
    GAME_STATE_MOVING_TILE,
    GAME_STATE_CLEARED_PAUSE,
    GAME_STATE_CLEARED
} GAME_STATE;

static GAME_STATE game_state = GAME_STATE_WAITING_INPUT;
static PUZZLE_MOVE puzzle_move;
static int32_t moving_tile_progress; // 0~32

static const uint8_t tile_ids[] = 
{ 
     0,  1,  2,  3, 
     8,  9, 10, 11,
    16, 17, 18, 19, 
    24, 25, 26, 27
};

static int8_t hud_moves_count[32];
static uint32_t game_cleared_time;

static void scene_playing_init(void)
{
    // do nothing
}

static void scene_playing_on_enter(void)
{
    puzzle_reset();
    srand(VSync(-1));
    puzzle_shuffle(1000);
    game_state = GAME_STATE_WAITING_INPUT;
}

static void scene_playing_on_exit(void)
{
    // do nothing
}

static void scene_playing_on_transition_finished(void)
{
    audio_play_music(GAME_MUSIC_ID_PLAYING);
}

static void handle_input()
{
    PUZZLE_DIRECTION direction = NONE;

    if (input_is_action_just_pressed(0, INPUT_ACTION_LEFT))
    {
        direction = LEFT;
    }
    else if (input_is_action_just_pressed(0, INPUT_ACTION_RIGHT))
    {
        direction = RIGHT;
    }
    else if (input_is_action_just_pressed(0, INPUT_ACTION_UP))
    {
        direction = UP;
    }
    else if (input_is_action_just_pressed(0, INPUT_ACTION_DOWN))
    {
        direction = DOWN;
    }
    // quit
    else if (input_is_action_just_pressed(0, INPUT_ACTION_SELECT))
    {
        scene_switch_to(SCENE_ID_TITLE);
    }
    // shuffle puzzle
    else if (input_is_action_just_pressed(0, INPUT_ACTION_START))
    {
        puzzle_shuffle(1000);
    }

    if (direction != NONE && puzzle_move_start(direction, &puzzle_move))
    {
        game_state = GAME_STATE_MOVING_TILE;
        moving_tile_progress = 0;
        audio_play_sound(GAME_SFX_ID_MOVING, 0x1000);
    }
}

static void scene_playing_fixed_update(void)
{
    switch (game_state)
    {
        case GAME_STATE_WAITING_INPUT:
        {
            if (game_state != GAME_STATE_CLEARED)
            {
                handle_input();
            }
            break;
        }

        case GAME_STATE_MOVING_TILE:
        {
            moving_tile_progress += 8;
            if (moving_tile_progress >= 32)
            {
                moving_tile_progress = 32;
                puzzle_move_commit(&puzzle_move);

                if (puzzle_is_cleared())
                {
                    game_state = GAME_STATE_CLEARED_PAUSE;
                    game_cleared_time = scene_frame_count + 20;
                    audio_stop_music();
                }
                else
                {
                    game_state = GAME_STATE_WAITING_INPUT;
                }
            }
            break;
        }

        // level was cleared, but maintains a short pause 
        // before showing the message to the player 
        case GAME_STATE_CLEARED_PAUSE:
        {
            if (scene_frame_count >= game_cleared_time)
            {
                game_state = GAME_STATE_CLEARED;
                game_cleared_time = scene_frame_count + 300;
                audio_play_sound(GAME_SFX_ID_CLEARED, 0xc00);
            }
            break;
        }

        case GAME_STATE_CLEARED:
        {
            if (scene_frame_count >= game_cleared_time)
            {
                scene_switch_to(SCENE_ID_TITLE);
            }
            break;
        }
    }    
}

static void scene_playing_render(void)
{
    // draw hud
    font_use(GAME_FONT_ID_SMALL);
    gfx_translate(0, 0);
    sprintf(hud_moves_count, "MOVES: %06d", puzzle_moves_count);    
    font_draw(hud_moves_count, 8, 8);

    if (game_state != GAME_STATE_CLEARED)
    {
        font_draw("SELECT-QUIT", 8, 212);
        font_draw("START-SHUFFLE", 134, 212);
    }
    // draw game cleared
    else
    {
        font_use(GAME_FONT_ID_BIG);
        gfx_translate(0, 0);
        font_draw("Puzzle Cleared", 28, 96);
    }

    // draw puzzle
    gfx_use_tileset(GAME_TILESET_ID_TILESET);
    gfx_translate(64, 56);

    if (game_state == GAME_STATE_MOVING_TILE)
    {
        int16_t dx = puzzle_move.dst.vx - puzzle_move.src.vx;
        int16_t dy = puzzle_move.dst.vy - puzzle_move.src.vy;
        dx = dx > 0 ? 1 : dx < 0 ? -1 : 0;
        dy = dy > 0 ? 1 : dy < 0 ? -1 : 0;
        int16_t tx = 32 * puzzle_move.src.vx + dx * moving_tile_progress;
        int16_t ty = 32 * puzzle_move.src.vy + dy * moving_tile_progress;
        gfx_draw_tile(tile_ids[puzzle_move.src_num], tx, ty);
    }

    if (game_state != GAME_STATE_CLEARED)
    {
        for (int row = 0; row < 4; row++)
        {
            for (int col = 0; col < 4; col++)
            {
                uint32_t tile_id = tile_ids[puzzle_table[row][col]];
                gfx_draw_tile(tile_id, 32 * col, 32 * row);
            }
        }
    }
}

SCENE* scene_playing_create(void)
{
    SCENE *scene_playing = scene_create();

    scene_playing->init = scene_playing_init;
    scene_playing->on_enter = scene_playing_on_enter;
    scene_playing->on_exit = scene_playing_on_exit;
    scene_playing->on_transition_finished = scene_playing_on_transition_finished;
    scene_playing->fixed_update = scene_playing_fixed_update;
    scene_playing->render = scene_playing_render;

    return scene_playing;
}