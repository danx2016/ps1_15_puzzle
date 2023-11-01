#include <libgpu.h>
#include "alpha.h"
#include "mem.h"
#include "gfx.h"

typedef struct
{
    TIM_IMAGE *image;
    TIM_IMAGE *mask;
} ALPHA;

static ALPHA *alphas[ALPHAS_MAX_NUM];
static DRAWENV offscr_drawenv;

void alpha_init(int16_t offscr_x, int16_t offscr_y, int16_t offscr_w, int16_t offscr_h)
{
    SetDefDrawEnv(&offscr_drawenv, offscr_x, offscr_y, offscr_w, offscr_h);
}

void *alpha_add(size_t alpha_id, TIM_IMAGE *image, TIM_IMAGE *mask)
{
    ALPHA *alpha = (ALPHA*) mem_alloc(sizeof(ALPHA));
    alpha->image = image;
    alpha->mask = mask;
    alphas[alpha_id] = alpha;
}

void alpha_draw(size_t alpha_id, int16_t x, int16_t y, uint8_t oppacity)
{
    if (oppacity == 0)
    {
        return;
    }

    ALPHA *alpha = alphas[alpha_id];

    int16_t image_x = alpha->image->prect->x;
    int16_t image_y = alpha->image->prect->y;
    int16_t image_w = alpha->image->prect->w << (2 - (alpha->image->mode & 3));
    int16_t image_h = alpha->image->prect->h;

    int16_t mask_x = alpha->mask->prect->x;
    int16_t mask_y = alpha->mask->prect->y;
    int16_t mask_w = alpha->mask->prect->w << (2 - (alpha->mask->mode & 3));
    int16_t mask_h = alpha->mask->prect->h;

    // draw main image using additive blend
    SPRT *image = (SPRT*) gfx_next_primitive;
    setSprt(image);
    setXY0(image, x, y);
    setWH(image, image_w, image_h);
    setUV0(image, 0, 0);
    setRGB0(image, oppacity, oppacity, oppacity);
    setSemiTrans(image, 1);
    if (alpha->image->mode & 0x8)
    {
        setClut(image, alpha->image->crect->x, alpha->image->crect->y);
    }    
    AddPrim(&gfx_ot[gfx_db][0], image);
    gfx_next_primitive += sizeof(SPRT);

    DR_TPAGE *image_tpage = (DR_TPAGE*) gfx_next_primitive;
    setDrawTPage(image_tpage, 0, 0, getTPage(alpha->image->mode & 3, 1, image_x, image_y));
    AddPrim(&gfx_ot[gfx_db][0], image_tpage);
    gfx_next_primitive += sizeof(DR_TPAGE);

    // draw mask in background of main draw context
    SPRT *mask2 = (SPRT*) gfx_next_primitive;
    setSprt(mask2);
    setXY0(mask2, x, y);
    setWH(mask2, mask_w, mask_h);
    setUV0(mask2, 0, 0);
    setRGB0(mask2, 128 - oppacity, 128 - oppacity, 128 - oppacity);
    setSemiTrans(mask2, 0);
    AddPrim(&gfx_ot[gfx_db][0], mask2);
    gfx_next_primitive += sizeof(SPRT);

    DR_TPAGE *mask2_tpage = (DR_TPAGE*) gfx_next_primitive;
    setDrawTPage(mask2_tpage, 0, 0, getTPage(2, 1, offscr_drawenv.clip.x, offscr_drawenv.clip.y));
    AddPrim(&gfx_ot[gfx_db][0], mask2_tpage);
    gfx_next_primitive += sizeof(DR_TPAGE);

    // return to main draw context
    DR_ENV *dr_env = (DR_ENV*) gfx_next_primitive;
    SetDrawEnv(dr_env, &gfx_draw[gfx_db]);
    AddPrim(&gfx_ot[gfx_db][0], dr_env);
    gfx_next_primitive += sizeof(DR_ENV);

    // draw mask in offscreen    
    SPRT *mask = (SPRT*) gfx_next_primitive;
    setSprt(mask);
    setXY0(mask, 0, 0);
    setWH(mask, mask_w, mask_h);
    setUV0(mask, 0, 0);
    setRGB0(mask, 0, 0, 0);
    setSemiTrans(mask, 0);
    if (alpha->mask->mode & 0x8)
    {
        setClut(mask, alpha->mask->crect->x, alpha->mask->crect->y);
    }    
    addPrim(&gfx_ot[gfx_db][0], mask);
    gfx_next_primitive += sizeof(SPRT);

    DR_TPAGE *mask_tpage = (DR_TPAGE*) gfx_next_primitive;
    setDrawTPage(mask_tpage, 0, 0, getTPage(alpha->mask->mode & 3, 0, mask_x, mask_y));
    addPrim(&gfx_ot[gfx_db][0], mask_tpage);
    gfx_next_primitive += sizeof(DR_TPAGE);    

    // copy background to offscreen
    RECT rect;
    setRECT(&rect, x, y + (gfx_db ? 0 : GFX_SCREEN_HEIGHT), image_w, image_h);
    DR_MOVE *dr_move = (DR_MOVE*) gfx_next_primitive;
    SetDrawMove(dr_move, &rect, offscr_drawenv.clip.x, offscr_drawenv.clip.y);
    AddPrim(&gfx_ot[gfx_db][0], dr_move);
    gfx_next_primitive += sizeof(DR_MOVE);
    
    // change draw context to offscreen
    DR_ENV *dr_env_offscr = (DR_ENV*) gfx_next_primitive;
    SetDrawEnv(dr_env_offscr, &offscr_drawenv);
    AddPrim(&gfx_ot[gfx_db][0], dr_env_offscr);
    gfx_next_primitive += sizeof(DR_ENV);
}
