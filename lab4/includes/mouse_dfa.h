#pragma once

#include <lcom/lcf.h>
#include "mouse.h"
#include "constants.h"

typedef enum {
    START,
    DRAW_UP,
    VERTEX,
    DRAW_DOWN,
    END
} mouse_state_t;

struct mouse_ev(mouse_process_event)(struct packet *mouse_packet, bool moving_up, uint8_t tolerance);

mouse_state_t(mouse_process_state)(struct mouse_ev *mouse_event, struct packet *mouse_packet, uint8_t tolerance);

mouse_state_t(mouse_confirm_end_state)(uint8_t x_len);

mouse_state_t (mouse_confirm_slope)(void);
