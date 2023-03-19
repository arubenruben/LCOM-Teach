#pragma once

#include <lcom/lcf.h>
#include "mouse.h"
#include "constants.h"
/*
struct packet {
  uint8_t bytes[3]; // mouse packet raw bytes
  bool rb, mb, lb;  // right, middle and left mouse buttons pressed
  int16_t delta_x;  // mouse x-displacement: rightwards is positive
  int16_t delta_y;  // mouse y-displacement: upwards is positive
  bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
};

enum mouse_ev_t { LB_PRESSED,
                  LB_RELEASED,
                  RB_PRESSED,
                  RB_RELEASED,
                  BUTTON_EV,
                  MOUSE_MOV };

struct mouse_ev {
  enum mouse_ev_t type;
  int16_t delta_x, delta_y;
};

typedef enum
{
    INIT,
    DRAW_UP,
    VERTEX,
    DRAW_DOWN,
    DONE
} mouse_state_t;
*/

mouse_ev_t (mouse_process_event)(mouse_reading_t *mouse_event)
{
    static mouse_ev_t current_event=START;

    if (mouse_event == NULL)
    {
        printf("mouse_event is NULL");
        return NULL;
    }

    if (mouse_event->lb == 1 && mouse_event->rb == 0)
    {
        current_event = LB_PRESSED;
    }
    else if (mouse_event->lb == 0 && mouse_event->rb == 1)
    {
        current_event = RB_PRESSED;
    }
    else if (mouse_event->lb == 0 && mouse_event->rb == 0 && current_event == LB_PRESSED)
    {
        current_event = LB_RELEASED;
    }
    else if (mouse_event->lb == 0 && mouse_event->rb == 0 && current_event == RB_PRESSED)
    {
        current_event = RB_RELEASED;
    }



}