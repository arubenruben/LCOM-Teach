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

struct mouse_ev(mouse_process_event)(struct packet *mouse_packet, bool moving_up, uint8_t tolerance)
{
    // To Avoid Modify State. Start in neutral state. The BUTTON_EV state.
    static struct mouse_ev current_event = {BUTTON_EV, 0, 0};

    // BUTTON_EV is the neutral state. Avoid to Modify Teacher Code.
    if (mouse_packet == NULL)
    {
        printf("mouse_packet is NULL");
        current_event.type = BUTTON_EV;
        current_event.delta_x = 0;
        current_event.delta_y = 0;
        return current_event;
    }

    if (mouse_packet->mb == true)
    {
        current_event.type = BUTTON_EV;
    }
    else if (mouse_packet->lb == true && mouse_packet->rb == false)
    {
        current_event.type = LB_PRESSED;
    }
    else if (mouse_packet->lb == false && mouse_packet->rb == true)
    {
        current_event.type = RB_PRESSED;
    }
    else if (mouse_packet->lb == false && mouse_packet->rb == false && current_event.type == MOUSE_MOV && moving_up == true)
    {
        current_event.type = LB_RELEASED;
    }
    else if (mouse_packet->lb == false && mouse_packet->rb == false && current_event.type == MOUSE_MOV && moving_up == false)
    {
        current_event.type = RB_RELEASED;
    }

    if (mouse_packet->delta_x || mouse_packet->delta_y)
    {
        current_event.type = MOUSE_MOV;
    }

    current_event.delta_x = mouse_packet->delta_x;
    current_event.delta_y = mouse_packet->delta_y;

    return current_event;
}

mouse_state_t(mouse_process_state)(struct mouse_ev *mouse_event, struct packet *mouse_packet, uint8_t tolerance)
{
    // To Avoid Modify State. Start in neutral state. The MOUSE_MOV state.
    static mouse_state_t current_state;

    if (mouse_event == NULL)
    {
        printf("mouse_event is NULL");
        return START;
    }

    if (mouse_packet == NULL)
    {
        printf("mouse_packet is NULL");
        return START;
    }

    switch (mouse_event->type)
    {
    case LB_PRESSED:
        if (current_state == START)
        {
            current_state = DRAW_UP;
        }
        else
        {
            current_state = START;
        }
        break;

    case LB_RELEASED:
        if (current_state == DRAW_UP)
        {
            current_state = VERTEX;
        }
        else
        {
            current_state = START;
        }
        break;

    case RB_PRESSED:
        if (current_state == VERTEX)
        {
            current_state = DRAW_DOWN;
        }
        else
        {
            current_state = START;
        }
        break;
    case RB_RELEASED:
        if (current_state == DRAW_DOWN)
        {
            current_state = END;
        }
        else
        {
            current_state = START;
        }
        break;
    case BUTTON_EV:
        current_state = START;
        break;
    case MOUSE_MOV:
        if (current_state == DRAW_UP)
        {
            // Check if the mouse is moving Diagonally UP Right
            if ((((mouse_packet->delta_x + tolerance >= 0) & TWO_COMPLEMENT) == 0) && ((mouse_packet->delta_y + tolerance) & TWO_COMPLEMENT) == 0)
            {
                current_state = DRAW_UP;
                displacement_x += mouse_packet->delta_x;
                displacement_y += mouse_packet->delta_y;
            }
            else
            {
                current_state = START;
            }
        }
        else if (current_state == DRAW_DOWN)
        {
            // Check if the mouse is moving Diagonally DOWN Left
            if ((((mouse_packet->delta_x + tolerance >= 0) & TWO_COMPLEMENT) == 0) && ((mouse_packet->delta_y - tolerance) & TWO_COMPLEMENT) != 0)
            {
                current_state = DRAW_DOWN;
                displacement_x += mouse_packet->delta_x;
                displacement_y += mouse_packet->delta_y;
            }
            else
            {
                current_state = START;
            }
        }
        else
        {
            current_state = START;
        }
        break;
    default:
        break;
    }

    return current_state;
}

mouse_state_t(mouse_confirm_end_state)(uint8_t x_len)
{

    if (displacement_x < x_len)
    {
        return START;
    }

    return END;
}

mouse_state_t(mouse_confirm_slope)(void)
{

    if (displacement_y / displacement_x < 1)
    {
        return START;
    }

    displacement_x=0;
    
    displacement_y=0;

    return VERTEX;
}
