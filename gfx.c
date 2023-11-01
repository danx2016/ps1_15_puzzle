#include <stddef.h>
#include <stdio.h>
#include <libgpu.h>
#include <libetc.h>

#include "gfx.h"

// enviroment
DISPENV gfx_disp[2];
DRAWENV gfx_draw[2];
uint32_t gfx_db = 0;

// primitives
uint32_t gfx_ot[2][OT_LEN];

static uint8_t primitives_buffer[2][GFX_PRIMITIVES_BUFFER_SIZE];
uint8_t *gfx_next_primitive;

// note: gfx_translation.vz is used to indicate that gfx translation was set
static SVECTOR gfx_translation;

// tilesets
typedef struct
{
    uint16_t cols;
    uint16_t rows;
    uint16_t tile_width;
    uint16_t tile_height;
    TIM_IMAGE *tim_image;
} TILESET;

static TILESET tilesets[GFX_MAX_NUM_TILESETS];
static TILESET *last_tileset = NULL;

static void gfx_begin_draw()
{
    ClearOTagR(gfx_ot[gfx_db], OT_LEN);
    gfx_next_primitive = primitives_buffer[gfx_db];
    last_tileset = NULL;
    gfx_translate(0, 0);
}

void gfx_init(void)
{
    ResetGraph(0);
    SetDefDispEnv(&gfx_disp[0], 0, 0, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    SetDefDispEnv(&gfx_disp[1], 0, GFX_SCREEN_HEIGHT, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    SetDefDrawEnv(&gfx_draw[0], 0, GFX_SCREEN_HEIGHT, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    SetDefDrawEnv(&gfx_draw[1], 0, 0, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    gfx_draw[0].isbg = 0;
    gfx_draw[1].isbg = 0;
    //setRGB0(&draw[0], 32, 32, 32);
    //setRGB0(&draw[1], 32, 32, 32);

    gfx_disp[0].screen.h = GFX_SCREEN_HEIGHT;
    gfx_disp[1].screen.h = GFX_SCREEN_HEIGHT;
    
    // clear entire vram
    RECT rect = { 0, 0, 1024, 512 };
    ClearImage(&rect, 0, 0, 0);
    DrawSync(0);

    SetDispMask(1);
    gfx_begin_draw();
}

static void gfx_add_dr_tpage(TIM_IMAGE *tim_image)
{
    DR_TPAGE *tpage = (DR_TPAGE*) gfx_next_primitive;
    uint16_t tp = getTPage(tim_image->mode & 3, 0, tim_image->prect->x, tim_image->prect->y);
    setDrawTPage(tpage, 0, 0, tp);
    AddPrim(&gfx_ot[gfx_db][0], tpage);
    gfx_next_primitive += sizeof(DR_TPAGE);
}

static void gfx_add_dr_offset(SVECTOR *translation)
{
    DR_OFFSET *offset = (DR_OFFSET*) gfx_next_primitive;
    SetDrawOffset(offset, (uint16_t*) translation);
    AddPrim(&gfx_ot[gfx_db][0], offset);
    gfx_next_primitive += sizeof(DR_OFFSET);
}

void gfx_swap_buffers(void)
{
    // translation
    if (gfx_translation.vz != 0)
    {
        gfx_add_dr_offset(&gfx_translation);
        gfx_translation.vz = 0;
    }
    // draw last dr_tpage
    if (last_tileset != NULL)
    {
        gfx_add_dr_tpage(last_tileset->tim_image);
    }

    DrawSync(0);
    VSync(0);
    DrawOTag(&gfx_ot[gfx_db][OT_LEN - 1]);
    gfx_db = !gfx_db;
    PutDispEnv(&gfx_disp[gfx_db]);
    PutDrawEnv(&gfx_draw[gfx_db]);
    gfx_begin_draw();
}

void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1)
{
    LINE_F2 *line = (LINE_F2*) gfx_next_primitive;
    setLineF2(line);
    setXY2(line, x0, y0, x1, y1);
    setRGB0(line, 255, 255, 255);
    AddPrim(&gfx_ot[gfx_db][0], line);
    gfx_next_primitive += sizeof(LINE_F2);
}

void gfx_fill_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t r,  uint8_t g,  uint8_t b)
{
    TILE *line = (TILE*) gfx_next_primitive;
    setTile(line);
    setXY0(line, x, y);
    setWH(line, w, h);
    setRGB0(line, r, g, b);
    AddPrim(&gfx_ot[gfx_db][0], line);
    gfx_next_primitive += sizeof(TILE);
}

void gfx_translate(int16_t tx, int16_t ty)
{
    if (gfx_translation.vz != 0)
    {
        gfx_add_dr_offset(&gfx_translation);
    }
    setVector(&gfx_translation, tx, ty + (!gfx_db) * GFX_SCREEN_HEIGHT, 1);
}

void gfx_add_tileset(size_t tileset_id, TIM_IMAGE *tileset, uint16_t cols, uint16_t rows)
{
    tilesets[tileset_id].cols = cols;
    tilesets[tileset_id].rows = rows;
    tilesets[tileset_id].tile_width = (tileset->prect->w << (tileset->mode & 3)) / cols; 
    tilesets[tileset_id].tile_height = tileset->prect->h / rows;
    tilesets[tileset_id].tim_image = tileset;
}

void gfx_use_tileset(size_t tileset_id)
{
    if (last_tileset != NULL)
    {
        gfx_add_dr_tpage(last_tileset->tim_image);
    }
    last_tileset = &tilesets[tileset_id];
}

void gfx_draw_tile(uint32_t tile_id, int16_t x, int16_t y)
{
    TIM_IMAGE *tim_image = last_tileset->tim_image;
    SPRT *sprt = (SPRT*) gfx_next_primitive;
    setSprt(sprt);
    setXY0(sprt, x, y);
    setWH(sprt, last_tileset->tile_width, last_tileset->tile_height);
    uint8_t u = (tile_id % last_tileset->cols) * last_tileset->tile_width;
    uint8_t v = (tile_id / last_tileset->cols) * last_tileset->tile_height;
    setUV0(sprt, u, v);
    setRGB0(sprt, 128, 128, 128);

    // clut
    if (tim_image->mode & 0x8)
    {
        setClut(sprt, tim_image->crect->x, tim_image->crect->y);
    }

    addPrim(&gfx_ot[gfx_db][0], sprt);
    gfx_next_primitive += sizeof(SPRT);    
}

void gfx_draw_custom_tile(int16_t src_x, int16_t src_y, int16_t src_w, int16_t src_h, int16_t dst_x, int16_t dst_y)
{
    TIM_IMAGE *tim_image = last_tileset->tim_image;
    SPRT *sprt = (SPRT*) gfx_next_primitive;
    setSprt(sprt);
    setXY0(sprt, dst_x, dst_y);
    setWH(sprt, src_w, src_h);
    setUV0(sprt, src_x, src_y);
    setRGB0(sprt, 128, 128, 128);

    // clut
    if (tim_image->mode & 0x8)
    {
        setClut(sprt, tim_image->crect->x, tim_image->crect->y);
    }

    addPrim(&gfx_ot[gfx_db][0], sprt);
    gfx_next_primitive += sizeof(SPRT);    
}