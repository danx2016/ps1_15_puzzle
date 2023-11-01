#ifndef _ALPHA_H
#define _ALPHA_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <libgpu.h>

#define ALPHAS_MAX_NUM 8

extern void alpha_init(int16_t offscr_x, int16_t offscr_y, int16_t offscr_w, int16_t offscr_h);
extern void *alpha_add(size_t alpha_id, TIM_IMAGE *image, TIM_IMAGE *mask);
extern void alpha_draw(size_t alpha_id, int16_t x, int16_t y, uint8_t oppacity);

#endif /* _ALPHA_H */