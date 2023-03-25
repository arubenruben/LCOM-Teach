#include <lcom/lcf.h>

#include "includes/mouse_dfa.h"
#include "includes/constants.h"

long displacement_x = 0;
long displacement_y = 0;
/*
typedef enum {
    START,
    DRAW_UP,
    VERTEX,
    DRAW_DOWN,
    END
} mouse_state_t;
*/

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

struct mouse_ev(mouse_process_event)(struct packet mouse_packet)
{
    static struct mouse_ev event_to_return = {MOUSE_MOV, 0, 0};

    event_to_return.delta_x = mouse_packet.delta_x;
    event_to_return.delta_y = mouse_packet.delta_y;

    if (mouse_packet.lb == true && mouse_packet.rb == false && mouse_packet.mb == false)
    {
        event_to_return.type = LB_PRESSED;
    }
    else if (mouse_packet.lb == false && mouse_packet.rb == true && mouse_packet.mb == false)
    {
        event_to_return.type = RB_PRESSED;
    }
    else if (mouse_packet.lb == false && mouse_packet.rb == false && mouse_packet.mb == true)
    {
        event_to_return.type = BUTTON_EV;
    }
    else
    {
        switch (event_to_return.type)
        {
        case LB_PRESSED:
            event_to_return.type = LB_RELEASED;
            break;
        case RB_PRESSED:
            event_to_return.type = RB_RELEASED;
            break;
        default:
            event_to_return.type = MOUSE_MOV;
            break;
        }
    }

    return event_to_return;
}

mouse_state_t(mouse_process_state)(struct mouse_ev event, uint8_t x_len, uint8_t tolerance)
{
    static mouse_state_t current_state = START;

    switch (current_state)
    {
    case START:
        if (event.type == LB_PRESSED)
        {
            if (displacement_x > 0 && displacement_y > 0)
            {
                displacement_x = event.delta_x;
                displacement_y = event.delta_y;

                current_state = DRAW_UP;
            }
            else if ((displacement_x < 0 && abs(displacement_x) < tolerance) && displacement_y >= 0)
            {
                displacement_x = event.delta_x;
                displacement_y = event.delta_y;

                current_state = DRAW_UP;
            }
            else if((displacement_x >0 && displacement_y <0 && abs(displacement_y)<tolerance)){
                displacement_x = event.delta_x;
                displacement_y = event.delta_y;

                current_state = DRAW_UP;
            }
            else if ((displacement_x < 0 && abs(displacement_x) < tolerance) && (displacement_y < 0 && abs(displacement_y) < tolerance))
            {
                displacement_x = event.delta_x;
                displacement_y = event.delta_y;

                current_state = DRAW_UP;
            }
            else
            {
                displacement_x = 0;
                displacement_y = 0;
            }
        }
        break;
    case DRAW_UP:
        if (event.type == LB_PRESSED)
        {
            // Verify Dispacement Signal and Tolerance
            if (displacement_x > 0 && displacement_y > 0)
            {
                displacement_x += event.delta_x;
                displacement_y += event.delta_y;
            }
            else if ((displacement_x < 0 && abs(displacement_x) < tolerance) && (displacement_y < 0 && abs(displacement_y) < tolerance))
            {
                displacement_x += event.delta_x;
                displacement_y += event.delta_y;
            }
            else
            {
                current_state = START;
            }
        }
        else if (event.type == LB_RELEASED)
        {
            current_state = VERTEX;
        }
        else
        {
            current_state = START;
        }
        break;
    case VERTEX:
        if (event.type == RB_PRESSED)
        {
            displacement_x = 0;
            displacement_y = 0;
            current_state = DRAW_DOWN;
        }
        else if (event.type == MOUSE_MOV)
        {
            if (abs(event.delta_x) > tolerance || abs(event.delta_y) > tolerance)
            {
                current_state = START;
            }
        }
        else
        {
            current_state = START;
        }
        break;

    case DRAW_DOWN:
        if (event.type == RB_PRESSED)
        {
            // Verify Dispacement Signal and Tolerance
            displacement_x += abs(event.delta_x);
            displacement_y += abs(event.delta_y);
        }
        else if (event.type == RB_RELEASED)
        {
            current_state = END;
        }
        else
        {
            current_state = START;
        }
        break;
    }

    // Verify Vertex Conditions
    if (current_state == VERTEX)
    {
    }

    // Verify End Conditions
    if (current_state == END)
    {
    }

    return current_state;
}