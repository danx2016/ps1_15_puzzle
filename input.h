#ifndef _INPUT_H
#define _INPUT_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    INPUT_ACTION_UP      =    16,
    INPUT_ACTION_DOWN    =    64,
    INPUT_ACTION_LEFT    =   128,
    INPUT_ACTION_RIGHT   =    32,
    INPUT_ACTION_CONFIRM =  8192,
    INPUT_ACTION_CANCEL  = 16384,
    INPUT_ACTION_START   =     8,
    INPUT_ACTION_SELECT  =     1
} ACTION;

extern void input_init(void);
extern void input_fixed_update(void);
extern bool input_is_action_pressed(size_t controller_id, ACTION action);
extern bool input_is_action_just_pressed(size_t controller_id, ACTION action);

#endif /* _INPUT_H */