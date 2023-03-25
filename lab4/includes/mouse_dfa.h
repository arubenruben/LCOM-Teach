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

*/

struct mouse_ev(mouse_process_event)(struct packet mouse_packet);

mouse_state_t(mouse_process_state)(struct mouse_ev event, uint8_t x_len, uint8_t tolerance);
