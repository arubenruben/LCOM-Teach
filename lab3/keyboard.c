#include <lcom/lcf.h>

#include "includes/keyboard.h"

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

int(kbc_read_out_buf)(uint8_t *data, bool sleep)
{

    if (data == NULL)
    {
        printf("Invalid pointer!\n");
        return 1;
    }

    for (size_t i = 0; i < MAX_TRIES; i++)
    {
        uint8_t stat;

        scan_code.error = false;

        util_sys_inb(STAT_REG, &stat); /* assuming it returns OK */

        /* loop while 8042 output buffer is empty */
        if (stat & OBF)
        {
            util_sys_inb(OUT_BUF, data); /* ass. it returns OK */

            if ((stat & ERROR_KBC) != 0)
                scan_code.error = true;
            else
                return 0;
        }

        if (sleep)
            tickdelay(micros_to_ticks(DELAY_US));
    }

    return -1;
}

int(kbc_read_dummy_byte)(void)
{
    uint8_t dummy_byte;

    if (kbc_read_out_buf(&dummy_byte, false) != 0)
        printf("Error reading Dummy byte\n");

    return 0;
}

int(kbc_write_command)(uint8_t address, uint8_t command)
{

    for (size_t i = 0; i < MAX_TRIES; i++)
    {
        uint8_t stat;

        util_sys_inb(STAT_REG, &stat); /* assuming it returns OK */

        /* loop while 8042 input buffer is not empty */
        if ((stat & IBF) == 0)
        {
            sys_outb(address, command); /* assuming it returns OK */

            return 0;
        }
        tickdelay(micros_to_ticks(DELAY_US));
    }

    return 1;
}

void(kbc_ih)(void)
{
    kbc_read_out_buf(&scan_code.scan_code, false);
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

int(kbc_read_command_byte)(uint8_t *command)
{
    kbc_write_command(STAT_REG, READ_COMMAND_BYTE);

    kbc_read_out_buf(command, true);

    return 0;
}

int(kbc_write_command_byte)(uint8_t command)
{
    if (kbc_write_command(STAT_REG, WRITE_COMMAND_BYTE) != 0)
    {
        printf("Error writing command byte\n");
        return 1;
    }

    if (kbc_write_command(OUT_BUF, command) != 0)
    {
        printf("Error writing command byte\n");
        return 1;
    }

    return 0;
}

int(kbc_enable_interrupts)(void)
{
    uint8_t command_byte;

    if (kbc_read_command_byte(&command_byte) != 0)
    {
        printf("Error reading command byte\n");
        return 1;
    }

    // Enable keyboard interrupts
    command_byte |= BIT(0);

    // Enable mouse interrupts
    command_byte |= BIT(1);

    // Mask to Enable keyboard interrupts
    uint8_t dis_dis2_mask = ~(BIT(5) | BIT(4));

    command_byte &= dis_dis2_mask;

    if (kbc_write_command_byte(command_byte) != 0)
    {
        printf("Error writing command byte\n");
        return 1;
    }

    return 0;
}

int(kbc_disable_interrupts)(void)
{

    uint8_t command_byte;

    if (kbc_read_command_byte(&command_byte) != 0)
    {
        printf("Error reading command byte\n");
        return 1;
    }

    // Disable keyboard interrupts
    command_byte &= ~BIT(0);

    // Disable mouse interrupts
    command_byte &= ~BIT(1);

    // Mask to Disable keyboard interrupts
    uint8_t dis_dis2_mask = ~(BIT(5) | BIT(4));

    command_byte &= dis_dis2_mask;

    if (kbc_write_command_byte(command_byte) != 0)
    {
        printf("Error writing command byte\n");
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
