#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <rand.h>
#include <libetc.h>
#include <libgpu.h>
#include <libgte.h>
#include <stdlib.h>

#include "gfx.h"
#include "alpha.h"
#include "mem.h"
#include "res.h"
#include "font.h"
#include "audio.h"
#include "input.h"
#include "scene_factory.h"
#include "game.h"

static void init_all_subsystems(void)
{
    gfx_init();
    alpha_init(768, 0, 256, 240);
    InitGeom();
    mem_init();
    res_init();
    audio_init();
    input_init();
}

static void load_big_font(void)
{
    FONT *font_0 = res_load_font(GAME_RES_FILENAME_FONT_BIN_BIG, GAME_RES_FILENAME_FONT_TIM_BIG);
    font_add(GAME_FONT_ID_BIG, GAME_TILESET_ID_FONT_BIG, font_0);
}

void game_load_all_assets(void)
{
    TIM_IMAGE *tim_image = res_load_image(GAME_RES_FILENAME_IMAGE_TILESET);
    gfx_add_tileset(GAME_TILESET_ID_BACKGROUND, tim_image, 2, 256); // background
    gfx_add_tileset(GAME_TILESET_ID_TILESET, tim_image, 8, 8);   // tileset
    gfx_add_tileset(GAME_TILESET_ID_TITLE, tim_image, 1, 2);   // title

    FONT *font_1 = res_load_font(GAME_RES_FILENAME_FONT_BIN_SMALL, GAME_RES_FILENAME_FONT_TIM_SMALL);
    font_add(GAME_FONT_ID_SMALL, GAME_TILESET_ID_FONT_SMALL, font_1);
    
    // title alpha

    TIM_IMAGE *title = res_load_image(GAME_RES_FILENAME_ALPHA_TITLE);
    TIM_IMAGE *title_mask = res_load_image(GAME_RES_FILENAME_ALPHA_MASK_TITLE);
    alpha_add(GAME_ALPHA_ID_TITLE, title, title_mask);

    // load audio assets

    uint8_t *xm0 = res_load(GAME_RES_FILENAME_MUSIC_XM_TITLE);
    uint8_t *vb0 = res_load(GAME_RES_FILENAME_MUSIC_VB_TITLE);
    uint8_t *vh0 = res_load(GAME_RES_FILENAME_MUSIC_VH_TITLE);
    
    uint8_t *xm1 = res_load(GAME_RES_FILENAME_MUSIC_XM_PLAYING);
    uint8_t *vb1 = res_load(GAME_RES_FILENAME_MUSIC_VB_PLAYING);
    uint8_t *vh1 = res_load(GAME_RES_FILENAME_MUSIC_VH_PLAYING);

    uint8_t *vb2 = res_load(GAME_RES_FILENAME_MUSIC_VB_SFX);
    uint8_t *vh2 = res_load(GAME_RES_FILENAME_MUSIC_VH_SFX);

    audio_add_music(GAME_MUSIC_ID_TITLE, vb0, vh0, xm0);
    audio_add_music(GAME_MUSIC_ID_PLAYING, vb1, vh1, xm1);
    audio_set_sound(vb2, vh2);
}

static void draw_common_background(void)
{
    gfx_use_tileset(GAME_TILESET_ID_BACKGROUND);
    gfx_translate(0, 0);
    int32_t a = scene_frame_count * 10;
    for (int y = 0; y < GFX_SCREEN_HEIGHT; y++)
    {
        uint16_t tx = (csin(a + 64 * y) + ccos(3 * a + 32 * y)) >> 10;
        uint16_t ty = 2 * ((a / 10) % 64);
        gfx_draw_tile((y * 2 + 1 + ty) % 64, -64 + tx, y);
        gfx_draw_tile((y * 2 + 1 + ty) % 64, 64 + tx, y);
        gfx_draw_tile((y * 2 + 1 + ty) % 64, 192 + tx, y);
    }    
}

static void init_all_scenes(void)
{
    scene_is_common_background_visible = false;
    scene_render_common_background = draw_common_background;

    SCENE *scene_init = scene_init_create();
    SCENE *scene_title = scene_title_create();
    SCENE *scene_playing = scene_playing_create();

    scene_add(SCENE_ID_INIT, scene_init);
    scene_add(SCENE_ID_TITLE, scene_title);
    scene_add(SCENE_ID_PLAYING, scene_playing);
    
    scene_init_all();
}

void game_entry_point(void)
{
    init_all_subsystems();
    load_big_font();
    init_all_scenes();

    // start game with 'init' scene
    scene_switch_to(SCENE_ID_INIT);
    
    // main game loop
    while (true)
    {
        input_fixed_update();
        scene_fixed_update();
        scene_render();
        gfx_swap_buffers();
    }
}

