#ifndef _RES_H
#define _RES_H

#include <stdint.h>
#include <stdbool.h>
#include <libgpu.h>

#include "font.h"

extern void res_init(void);
extern void *res_load(uint8_t *filename);
extern TIM_IMAGE *res_load_image(uint8_t *filename);
extern FONT *res_load_font(uint8_t *bin_filename, uint8_t *tim_filename);

#endif /* _RES_H */