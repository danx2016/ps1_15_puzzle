// Variable Width Bitmap-Font
// refereces: 
// http://www.angelcode.com/products/bmfont/
// https://www.gamedev.net/forums/topic.asp?topic_id=330742

#ifndef _FONT_H
#define _FONT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <libgpu.h>

#define FONTS_MAX_NUM 8

typedef struct
{
	uint16_t x;
    uint16_t y;
	uint16_t width;
    uint16_t height;
	int16_t x_offset;
    int16_t y_offset;
	uint16_t x_advance;
	uint16_t page;
} FONT_CHAR;

typedef struct
{
	uint16_t line_height;
	uint16_t base;
	uint16_t width;
    uint16_t height;
	uint16_t pages;
	FONT_CHAR chars[256];
    TIM_IMAGE *tim_image;
	size_t tileset_id;
} FONT;

extern void font_add(size_t font_id, size_t tileset_id, FONT *font);
extern void font_use(size_t font_id);
extern void font_draw(uint8_t *text, int16_t x, int16_t y);

#endif /* _FONT_H */