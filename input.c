#include <stdlib.h>
#include <libpad.h>

#include "input.h"

static uint8_t pad_info[2][34];

static uint16_t pad_btn_pressed[2];
static uint16_t pad_btn_just_pressed[2]; // keep track just_pressed states

void input_init(void)
{
    PadInitDirect(pad_info[0], pad_info[1]);
    PadStartCom();
    pad_info[0][0] = pad_info[0][1] = 0xff;
    pad_info[1][0] = pad_info[1][1] = 0xff;
}

void input_fixed_update(void)
{
    for (size_t controller_id = 0; controller_id < 2; controller_id++)
    {
        uint16_t pad = ~(*(uint16_t*) &pad_info[controller_id][2]);
        pad_btn_just_pressed[controller_id] = pad & (pad ^ pad_btn_pressed[controller_id]);
        pad_btn_pressed[controller_id] = pad;
    }
}

static inline bool check_button(size_t controller_id, ACTION action, uint16_t pad_btn[])
{
    uint8_t controller_type = pad_info[controller_id][1] & 0xf0;

    // accepts 16-button digital (4) & analog (7) mode 
    if(pad_info[controller_id][0] == 0 && (controller_type == 0x40 || controller_type == 0x70))
    {
        return (pad_btn[controller_id] & action);
    }
    else 
    {
        return false;
    }
}

bool input_is_action_pressed(size_t controller_id, ACTION action)
{
    return check_button(controller_id, action, pad_btn_pressed);
}

bool input_is_action_just_pressed(size_t controller_id, ACTION action)
{
    return check_button(controller_id, action, pad_btn_just_pressed);
}
