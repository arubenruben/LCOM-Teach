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
}

int(kbc_read_command)(uint8_t *command)
{
    uint8_t status;

    if (util_sys_inb(STAT_REG, &status) != 0)
    {
        printf("Error reading status register\n");
        return 1;
    }

    if (status & OBF)
    {
        if (util_sys_inb(OUT_BUF, command) != 0)
        {
            printf("Error reading output buffer\n");
            return 1;
        }

        if ((status & ERROR_KBC) != 0)
        {
            printf("Error reading scancode\n");
            return 1;
        }
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

    if ((status & IBF) == 0)
    {
        if (sys_outb(STAT_REG, command) != 0)
        {
            printf("Error writing command\n");
            return 1;
        }
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
