#include <lcom/lcf.h>

#include "includes/kbc.h"

static int hook_id;

scan_code_t scan_code;

// Subscribes and enables KBC interrupts
int(kbc_subscribe_int)(uint8_t *bit_no)
{
    hook_id = *bit_no;

    if (sys_irqsetpolicy(KBC_IRQ, (IRQ_REENABLE | IRQ_EXCLUSIVE), &hook_id) != 0)
    {
        printf("Error subscribing KBC interrupts\n");
        return 1;
    }

    return 0;
}

void(kbc_ih)(void)
{
    uint8_t status;

    if (util_sys_inb(STAT_REG, &status) != 0)
    {
        printf("Error reading status register\n");
        return;
    }

    if (status & OBF)
    {
        if (util_sys_inb(OUT_BUF, &scan_code.scan_code) != 0)
        {
            printf("Error reading output buffer\n");
            scan_code.error = true;
            return;
        }

        if ((status & ERROR_KBC) != 0)
        {
            printf("Error reading scancode\n");
            return;
        }

        scan_code.error = false;
    }
    else
    {
        scan_code.error = true;
    }
}

void(kbc_reading_task)()
{
    // To Preserve the Value Between Interrupts
    static uint8_t scan_codes[2] = {0, 0};

    bool is_make_code = true;
    uint8_t num_valid_scan_codes = 1;

    if (scan_code.error)
    {
        printf("Error reading scancode\n");
        return;
    }

    if (scan_code.scan_code >= BIT(7))
        is_make_code = false;

    if (scan_code.scan_code == 0xE0)
    {
        scan_code.expecting_second_byte = true;
        num_valid_scan_codes = 2;
    }
    else
    {
        scan_code.expecting_second_byte = false;
    }

    if (scan_codes[0] == 0xE0)
    {
        scan_codes[1] = scan_code.scan_code;
        num_valid_scan_codes = 2;
    }
    else
    {
        scan_codes[0] = scan_code.scan_code;
    }

    // If the first byte is 0xE0 and the second byte is -1, then the scancode is incomplete
    // and we should wait for the next interrupt to get the second byte
    // Cannot Print scancode
    if (scan_codes[0] == 0xE0 && scan_code.expecting_second_byte == true)
        return;

    kbd_print_scancode(is_make_code, num_valid_scan_codes, scan_codes);
}

int(kbc_read_command)(uint8_t *command)
{
    uint8_t status;

    if (util_sys_inb(STAT_REG, &status) != 0)
    {
        printf("Error reading status register\n");
        return 1;
    }

    if (status & IBF)
    {
        printf("IBF is set\n");
        return -1;
    }

    if (sys_outb(STAT_REG, READ_COMMAND_BYTE) != 0)
    {
        printf("Error writing command\n");
        return 1;
    }

    for (u_int8_t i = 0; i < MAX_TRIES; i++)
    {
        if (util_sys_inb(STAT_REG, &status) != 0)
        {
            printf("Error reading status register\n");
            return 1;
        }

        // If OBF is set and TIMEOUT is not set, then we can read the output buffer
        if ((status & OBF) != 0 && (status & TIMEOUT) == 0)
            break;

        tickdelay(micros_to_ticks(DELAY_US));
    }

    if ((status & ERROR_KBC) != 0)
    {
        printf("Error reading scancode\n");
        return 1;
    }

    if (util_sys_inb(OUT_BUF, command) != 0)
    {
        printf("Error reading output buffer\n");
        return 1;
    }
    return 0;
}

int(kbc_write_command)(uint8_t command)
{
    uint8_t status;

    if (util_sys_inb(STAT_REG, &status) != 0)
    {
        printf("Error reading status register\n");
        return 1;
    }

    if (status & IBF)
    {
        printf("IBF is set\n");
        return -1;
    }

    if (sys_outb(STAT_REG, WRITE_COMMAND_BYTE) != 0)
    {
        printf("Error writing command\n");
        return 1;
    }

    for (size_t i = 0; i < MAX_TRIES; i++)
    {
        if (util_sys_inb(STAT_REG, &status) != 0)
        {
            printf("Error reading status register\n");
            return 1;
        }

        if ((status & IBF) == 0 || (status & TIMEOUT) == 0)
            break;
    }

    if (status & IBF)
    {
        printf("IBF is set\n");
        return -1;
    }

    if (sys_outb(OUT_BUF, command) != 0)
    {
        printf("Error writing command\n");
        return 1;
    }

    return 0;
}

int(kbc_unsubscribe_int)(void)
{
    if (sys_irqrmpolicy(&hook_id) != 0)
    {
        printf("Error unsubscribing KBC interrupts\n");
        return 1;
    }

    return 0;
}
