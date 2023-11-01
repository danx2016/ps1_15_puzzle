#ifndef _GFX_H
#define _GFX_H

#include <stdint.h>
#include <libgpu.h>

#define GFX_SCREEN_WIDTH  256
#define GFX_SCREEN_HEIGHT 240

#define GFX_PRIMITIVES_BUFFER_SIZE 32768
#define GFX_MAX_NUM_TILESETS 8

extern DISPENV gfx_disp[2];
extern DRAWENV gfx_draw[2];

#define OT_LEN 1
extern uint32_t gfx_ot[2][OT_LEN];

extern uint32_t gfx_db;
extern uint8_t *gfx_next_primitive;

extern void gfx_init(void);
extern void gfx_swap_buffers(void);

extern void gfx_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
extern void gfx_fill_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t r,  uint8_t g,  uint8_t b);

// translate all primitives by (tx, ty)
extern void gfx_translate(int16_t tx, int16_t ty);

extern void gfx_add_tileset(size_t tileset_id, TIM_IMAGE *tileset, uint16_t cols, uint16_t rows);
extern void gfx_use_tileset(size_t tileset_id);
extern void gfx_draw_tile(uint32_t tile_id, int16_t x, int16_t y);
extern void gfx_draw_custom_tile(int16_t src_x, int16_t src_y, int16_t src_w, int16_t src_h, int16_t dst_x, int16_t dst_y);

#endif /* _GFX_H */