#include "scene_factory.h"
#include "gfx.h"
#include "alpha.h"
#include "font.h"
#include "input.h"
#include "audio.h"
#include "puzzle.h"
#include "game.h"

static uint8_t title_oppacity;
static uint32_t start_time;
static uint32_t blink_time;

static void scene_title_init(void)
{
    // do nothing
}

static void scene_title_on_enter(void)
{
    audio_play_music(GAME_MUSIC_ID_TITLE);
    title_oppacity = 0;
}

static void scene_title_on_exit(void)
{
    // do nothing
}

static void scene_title_on_transition_finished(void)
{
    start_time = 0;
    blink_time = scene_frame_count;
}

static void scene_title_fixed_update(void)
{
    if (title_oppacity < 128)
    {
        title_oppacity += 4;
        return;
    }

    if (!start_time && input_is_action_pressed(0, INPUT_ACTION_START))
    {
        start_time = scene_frame_count + 40;
        blink_time = scene_frame_count;

        audio_stop_music();
        audio_play_sound(GAME_SFX_ID_START, 0x1000);
    }
    else if (start_time && scene_frame_count >= start_time)
    {
        scene_switch_to(SCENE_ID_PLAYING);
    }
}

static void scene_title_render(void)
{
    // draw text
    font_use(GAME_FONT_ID_SMALL);
    if (scene_is_transition_finished && title_oppacity >= 128)
    {
        uint8_t *press_start_msg = "Press START button";
        if (start_time)
        {
            if (((scene_frame_count - blink_time) % 5) > 2)
            {
                font_draw(press_start_msg, 60, 148);
            }
        }
        else if (((scene_frame_count - blink_time) % 15) > 5)
        {
            font_draw(press_start_msg, 60, 148);
        }
    }
    font_draw("(c)2023 danx2016", 70, 200);

    // workaround to send the pending dr_tpage
    gfx_use_tileset(GAME_TILESET_ID_TITLE); 

    // draw title
    alpha_draw(0, 24, 32, title_oppacity);
}

SCENE* scene_title_create(void)
{
    SCENE *scene_title = scene_create();

    scene_title->init = scene_title_init;
    scene_title->on_enter = scene_title_on_enter;
    scene_title->on_exit = scene_title_on_exit;
    scene_title->on_transition_finished = scene_title_on_transition_finished;
    scene_title->fixed_update = scene_title_fixed_update;
    scene_title->render = scene_title_render;

    return scene_title;
}