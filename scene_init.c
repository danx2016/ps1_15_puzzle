#include "scene_factory.h"
#include "gfx.h"
#include "font.h"
#include "input.h"
#include "game.h"

static uint32_t state = 0;
static uint32_t start_time = 0;

static void scene_init_init(void)
{
    // do nothing
}

static void scene_init_on_enter(void)
{
    // do nothing
}

static void scene_init_on_exit(void)
{
    scene_is_common_background_visible = true;    
}

static void scene_init_on_transition_finished(void)
{
    // do nothing
}

static void scene_init_fixed_update(void)
{
    switch (state)
    {
        case 0:
        {
            state = 1;
            start_time = scene_frame_count + 180;
            break;
        }

        case 1:
        {
            game_load_all_assets();
            state = 2;
            break;
        }
    
        case 2:
        {
            if (scene_frame_count >= start_time)
            {
                scene_switch_to(SCENE_ID_TITLE);
            }
        }
    }
}

static void scene_init_render(void)
{
    font_use(GAME_FONT_ID_BIG);
    font_draw("danx2016", 60, 92);
}

SCENE* scene_init_create(void)
{
    SCENE *scene_init = scene_create();

    scene_init->init = scene_init_init;
    scene_init->on_enter = scene_init_on_enter;
    scene_init->on_exit = scene_init_on_exit;
    scene_init->on_transition_finished = scene_init_on_transition_finished;
    scene_init->fixed_update = scene_init_fixed_update;
    scene_init->render = scene_init_render;

    return scene_init;
}