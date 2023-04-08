// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include "includes/gpu.h"
#include "includes/constants.h"
#include "includes/keyboard.h"
#include "includes/timer.h"
#include <math.h>

extern scan_code_t scan_code;

int main(int argc, char *argv[])
{
    // sets the language of LCF messages (can be either EN-US or PT-PT)
    lcf_set_language("EN-US");

    // enables to log function invocations that are being "wrapped" by LCF
    // [comment this out if you don't want/need it]
    lcf_trace_calls("/home/lcom/labs/lab5/trace.txt");

    // enables to save the output of printf function calls on a file
    // [comment this out if you don't want/need it]
    lcf_log_output("/home/lcom/labs/lab5/output.txt");

    // handles control over to LCF
    // [LCF handles command line arguments and invokes the right function]
    if (lcf_start(argc, argv))
        return 1;

    // LCF clean up tasks
    // [must be the last statement before return]
    lcf_cleanup();

    return 0;
}

int(video_test_init)(uint16_t mode, uint8_t delay)
{

    if (vbe_set_mode(mode) != 0)
    {
        printf("Error in vbe_set_mode()");
        return 1;
    }

    sleep(delay);

    if (vg_exit() != 0)
    {
        printf("Error in vg_exit()");
        return 1;
    }

    return 0;
}

int(video_test_rectangle)(uint16_t mode, uint16_t x, uint16_t y,
                          uint16_t width, uint16_t height, uint32_t color)
{

    // subscribes kbc interrupts
    uint8_t bit_no = 1;

    int ipc_status, r;
    message msg;

    if (kbc_subscribe_int(&bit_no) != 0)
    {
        printf("Error subscribing KBC interrupts\n");
        return 1;
    }

    if (vg_init(mode) == NULL)
    {
        printf("Error in vg_init()");
        return 1;
    }

    while (scan_code.scan_code != ESC_BREAK_CODE)
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status))
        { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & BIT(bit_no))
                {
                    kbc_ih();
                    if (!scan_code.error)
                        kbc_reading_task();
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */
            }
        }
        else
        { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }

        if (vg_draw_rectangle(x, y, width, height, color) != 0)
        {
            printf("Error in vg_draw_rectangle()");
            return 1;
        }
    }

    if (vg_exit() != 0)
    {
        printf("Error in vg_exit()");
        return 1;
    }

    if (kbc_unsubscribe_int() != 0)
    {
        printf("Error unsubscribing KBC interrupts\n");
        return 1;
    }

    return 0;
}

uint32_t get_color(uint32_t row, uint32_t col, uint32_t first, uint8_t no_rectangles, uint8_t step)
{
    uint32_t color = 0;

    uint32_t r, r1;
    uint32_t g, g1;
    uint32_t b, b1;

    if (get_bits_per_pixel() == 8)
        color = (first + (row * no_rectangles + col) * step) % (1 << get_bits_per_pixel());
    else
    {
        r1 = first >> (get_BlueMaskSize() + get_GreenMaskSize()) & ((1 << get_RedMaskSize()) - 1);
        g1 = first >> get_BlueMaskSize() & ((1 << get_GreenMaskSize()) - 1);
        b1 = first & ((1 << get_BlueMaskSize()) - 1);

        r = (r1 + col * step) % (1 << get_RedMaskSize());
        g = (g1 + row * step) % (1 << get_GreenMaskSize());
        b = (b1 + (col + row) * step) % (1 << get_BlueMaskSize());
        r = r << get_RedFieldPosition();
        g = g << get_GreenFieldPosition();
        b = b << get_BlueFieldPosition();

        color = r | g | b;
    }

    return color;
}

int(video_test_pattern)(uint16_t mode, uint8_t no_rectangles, uint32_t first, uint8_t step)
{
    // subscribes kbc interrupts
    uint8_t bit_no = 1;

    int ipc_status, r;
    message msg;

    if (kbc_subscribe_int(&bit_no) != 0)
    {
        printf("Error subscribing KBC interrupts\n");
        return 1;
    }

    if (vg_init(mode) == NULL)
    {
        printf("Error in vg_init()");
        return 1;
    }

    uint32_t color = 0;
    uint16_t width = 0;
    uint16_t height = 0;

    if (no_rectangles != 0)
    {
        width = ((get_h_res()) / no_rectangles);
        height = ((get_v_res()) / no_rectangles);
    }

    for (int row = 0; row < no_rectangles; row++)
    {
        for (int col = 0; col < no_rectangles; col++)
        {
            color = get_color(row, col, first, no_rectangles, step);

            vg_draw_rectangle(col * width, row * height, width, height, color);
        }
    }

    while (scan_code.scan_code != ESC_BREAK_CODE)
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status))
        { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & BIT(bit_no))
                {
                    kbc_ih();
                    if (!scan_code.error)
                        kbc_reading_task();
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */
            }
        }
        else
        { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    if (vg_exit() != 0)
    {
        printf("Error in vg_exit()");
        return 1;
    }

    if (kbc_unsubscribe_int() != 0)
    {
        printf("Error unsubscribing KBC interrupts\n");
        return 1;
    }

    return 0;
}

int(video_test_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y)
{
    // subscribes kbc interrupts
    uint8_t bit_no = 1;

    int ipc_status, r;
    message msg;

    if (kbc_subscribe_int(&bit_no) != 0)
    {
        printf("Error subscribing KBC interrupts\n");
        return 1;
    }

    if (vg_init(INDEXED_MODE) == NULL)
    {
        printf("Error in vg_init()");
        return 1;
    }

    draw_xpm(xpm, x, y);

    while (scan_code.scan_code != ESC_BREAK_CODE)
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status))
        { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE: /* hardware interrupt notification */
                if (msg.m_notify.interrupts & BIT(bit_no))
                {
                    kbc_ih();
                    if (!scan_code.error)
                        kbc_reading_task();
                }
                break;
            default:
                break; /* no other notifications expected: do nothing */
            }
        }
        else
        { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }

    if (vg_exit() != 0)
    {
        printf("Error in vg_exit()");
        return 1;
    }

    if (kbc_unsubscribe_int() != 0)
    {
        printf("Error unsubscribing KBC interrupts\n");
        return 1;
    }

    return 0;
}

int(video_test_move)(xpm_map_t xpm, uint16_t xi, uint16_t yi, uint16_t xf, uint16_t yf,
                     int16_t speed, uint8_t fr_rate)
{
    // subscribes kbc interrupts
    uint8_t timer_bit_no = 0;
    uint8_t kb_bit_no = 1;
    uint16_t increment_between_frames = 0;

    int ipc_status, r;
    message msg;

    if (timer_subscribe_int(&timer_bit_no) != 0)
    {
        printf("Error subscribing timer interrupts\n");
        return 1;
    }

    if (vg_init(INDEXED_MODE) == NULL)
    {
        printf("Error in vg_init()");
        return 1;
    }

    if (kbc_subscribe_int(&kb_bit_no) != 0)
    {
        printf("Error subscribing KBC interrupts\n");
        return 1;
    }

    uint16_t current_x = xi;
    uint16_t current_y = yi;

    draw_xpm(xpm, current_x, current_y);

    while (scan_code.scan_code != ESC_BREAK_CODE && (current_x <= xf || current_y <= yf))
    {
        /* Get a request message. */
        if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0)
        {
            printf("driver_receive failed with: %d", r);
            continue;
        }
        if (is_ipc_notify(ipc_status))
        { /* received notification */
            switch (_ENDPOINT_P(msg.m_source))
            {
            case HARDWARE: /* hardware interrupt notification */

                if (msg.m_notify.interrupts & BIT(timer_bit_no))
                {
                    timer_int_handler();

                    if (counter % (sys_hz() / fr_rate) != 0)
                        continue;

                    erase_xpm(xpm, current_x, current_y);

                    if (speed > 0)
                    {
                        increment_between_frames = speed;
                    }
                    else if (counter % abs(speed) == 0)
                    {
                        increment_between_frames = 1;
                    }
                    else
                    {
                        increment_between_frames = 0;
                    }

                    if (current_x <= xf)
                    {
                        current_x += increment_between_frames;
                    }
                    else
                    {
                        current_y += increment_between_frames;
                    }

                    if (current_x > xf)
                        current_x = xf;

                    if (current_y > yf)
                        current_y = yf;

                    draw_xpm(xpm, current_x, current_y);
                }
                if (msg.m_notify.interrupts & BIT(kb_bit_no))
                {
                    kbc_ih();

                    if (!scan_code.error)
                        kbc_reading_task();
                    
                }

                break;
            default:
                break; /* no other notifications expected: do nothing */
            }
        }
        else
        { /* received a standard message, not a notification */
            /* no standard messages expected: do nothing */
        }
    }
    // unsubscribes kbc interrupts
    if (kbc_unsubscribe_int() != 0)
    {
        printf("Error unsubscribing KBC interrupts\n");
        return 1;
    }

    if (timer_unsubscribe_int() != 0)
    {
        printf("Error unsubscribing timer interrupts\n");
        return 1;
    }

    if (vg_exit() != 0)
    {
        printf("Error in vg_exit()");
        return 1;
    }

    return 0;
}

int(video_test_controller)()
{
    /* To be completed */
    printf("%s(): under construction\n", __func__);

    return 1;
}
