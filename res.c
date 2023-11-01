#include <stdio.h>
#include <memory.h>
#include <libapi.h>
#include <libgpu.h>
#include <libds.h>

#include "res.h"
#include "mem.h"

void res_init(void)
{
    //_96_remove();
    while (!DsInit());
}

void *res_load(uint8_t *filename)
{
    DslFILE file;
    uint8_t *data_buffer = NULL;
    if (DsSearchFile(&file, filename))
    {
        uint32_t num_sectors = (file.size + 2047) / 2048;
        uint32_t file_size = num_sectors * 2048;
        data_buffer = mem_alloc(file_size);
        DsRead(&file.pos, num_sectors, (uint32_t*) data_buffer, DslModeSpeed);

        // wait read file
        uint8_t sync_result[8];
        int32_t read_result = 0;
        while ((read_result = DsReadSync(sync_result)) > 0);

        // error
        if (read < 0)
        {
            mem_free(data_buffer);
            data_buffer = NULL;
        }
    }
    return data_buffer;
}

static void transfer_image_to_vram(TIM_IMAGE *tim_image, uint8_t *image_data)
{
    LoadImage(tim_image->prect, tim_image->paddr);
    DrawSync(0);
    if (tim_image->mode & 0x8)
    {
        LoadImage(tim_image->crect, tim_image->caddr);
        DrawSync(0);
    }
}

TIM_IMAGE *res_load_image(uint8_t *filename)
{
    TIM_IMAGE *tim_image = mem_alloc(sizeof(TIM_IMAGE));
    uint8_t *image_data = NULL;

    // try until success
    while ((image_data = res_load(filename)) == NULL);

    OpenTIM((uint32_t*) image_data);
    ReadTIM(tim_image);
    transfer_image_to_vram(tim_image, image_data);

    // copy the RECT info before freeing image_data from memory
    RECT *prect = mem_alloc(sizeof(RECT));
    RECT *crect = mem_alloc(sizeof(RECT));
    memcpy(prect, tim_image->prect, sizeof(RECT));
    memcpy(crect, tim_image->crect, sizeof(RECT));
    tim_image->prect = prect;
    tim_image->crect = crect;

    // once image was transferred to vram, now it can be freed
    mem_free(image_data);

    return tim_image;
}

FONT *res_load_font(uint8_t *bin_filename, uint8_t *tim_filename)
{
    FONT *font = NULL;
    // try to load font.bin until success
    while ((font = res_load(bin_filename)) == NULL);

    TIM_IMAGE *tim_image = NULL;
    // try to load font.tim until success
    while ((tim_image = res_load_image(tim_filename)) == NULL);

    font->tim_image = tim_image;
    return font;
}