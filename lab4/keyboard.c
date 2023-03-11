#include "includes/keyboard.h"

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
