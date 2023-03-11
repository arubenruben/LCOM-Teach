
#include "lcom/lcf.h"

#include "includes/mouse.h"

static int hook_id_mouse = 12;

static mouse_reading_t mouse_packet;

/*
struct packet {
    uint8_t bytes[3]; // mouse packet raw bytes
    bool rb, mb, lb;  // right, middle and left mouse buttons pressed
    int16_t delta_x;  // mouse x-displacement: rightwards is positive
    int16_t delta_y;  // mouse y-displacement: upwards is positive
    bool x_ov, y_ov;  // mouse x-displacement and y-displacement overflows
};
*/

int(mouse_subscribe_int)(uint8_t *bit_no)
{
    hook_id_mouse = *bit_no;

    if (sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse) != 0)
    {
        printf("Error in sys_irqsetpolicy()\n");
        return 1;
    }

    return 0;
}

int(personal_mouse_enable_data_reporting)(void)
{
    mouse_enable_data_reporting();
    
    return 0;
}

int(mouse_read_out_buf)(uint8_t *data, bool sleep)
{
    if (data == NULL)
    {
        printf("Invalid pointer!\n");
        return 1;
    }

    for (size_t i = 0; i < MAX_TRIES; i++)
    {
        uint8_t stat;

        util_sys_inb(STAT_REG, &stat); /* assuming it returns OK */

        /* loop while 8042 output buffer is empty */
        if ((stat & OBF) && (stat & AUX))
        {
            util_sys_inb(OUT_BUF, data); /* ass. it returns OK */

            if ((stat & ERROR_KBC) != 0)
                mouse_packet.error = true;
            else
                return 0;
        }

        if (sleep)
            tickdelay(micros_to_ticks(DELAY_US));
    }

    return -1;
}

void(mouse_ih)(void)
{
    mouse_packet.error = false;

    mouse_read_out_buf(&mouse_packet.packet, true);
}

int(mouse_reading_task)(void)
{
    static uint8_t byte_counter = 0;
    static struct packet lcf_mouse_packet;

    if (mouse_packet.error)
    {
        printf("Error in mouse packet!\n");
        byte_counter = 0;
        return -1;
    }

    lcf_mouse_packet.bytes[byte_counter] = mouse_packet.packet;

    byte_counter++;

    if (byte_counter == 3)
    {
        lcf_mouse_packet.lb = lcf_mouse_packet.bytes[0] & BIT(0);

        lcf_mouse_packet.rb = lcf_mouse_packet.bytes[0] & BIT(1);

        lcf_mouse_packet.mb = lcf_mouse_packet.bytes[0] & BIT(2);

        lcf_mouse_packet.x_ov = lcf_mouse_packet.bytes[0] & BIT(6);

        lcf_mouse_packet.y_ov = lcf_mouse_packet.bytes[0] & BIT(7);

        lcf_mouse_packet.delta_x = lcf_mouse_packet.bytes[1];

        lcf_mouse_packet.delta_y = lcf_mouse_packet.bytes[2];

        mouse_print_packet(&lcf_mouse_packet);

        byte_counter = 0;
    }

    return byte_counter;
}

int(mouse_disable_data_reporting)(void)
{
    return 0;
}

int(mouse_unsubscribe_int)(void)
{
    if (sys_irqrmpolicy(&hook_id_mouse) != 0)
    {
        printf("Error in sys_irqrmpolicy()\n");
        return 1;
    }

    return 0;
}
