#include "includes/rtc.h"

int(rtc_subscribe_int)(uint8_t *bit_no)
{
    printf("Not implemented yet!\n");
    return 1;
}

int(rtc_enable_interrupts)(void)
{
    printf("Not implemented yet!\n");
    return 1;
}

int(rtc_disable_interrupts)(void)
{
    printf("Not implemented yet!\n");
    return 1;
}

int(rtc_unsubscribe_int)(void)
{
    printf("Not implemented yet!\n");
    return 1;
}

void(rtc_ih)(void)
{
    printf("Not implemented yet!\n");
    return;
}

uint8_t(read_register(uint32_t reg))
{
    uint8_t reg_value = 0x00;
    uint8_t status = 0x00;

    if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != OK)
    {
        printf("Error writing to RTC_ADDR_REG\n");
        return 1;
    }

    if (util_sys_inb(RTC_DATA_REG, &status) != 0)
    {
        printf("Error reading from RTC_DATA_REG\n");
        return 1;
    }
    if (sys_outb(RTC_ADDR_REG, reg) != OK)
    {
        printf("Error writing to RTC_ADDR_REG\n");
        return 1;
    }

    if (util_sys_inb(RTC_DATA_REG, &reg_value) != 0)
    {
        printf("Error reading from RTC_DATA_REG\n");
        return 1;
    }

    if (status & RTC_DM)
    {
        printf("DM is set\n");

        reg_value = BCD_TO_BIN(reg_value);
    }

    return reg_value;
}

rtc_t(read_time)()
{

    rtc_t time;

    for (size_t i = 0; i < MAX_RTC_TRIES; i++)
    {
        sys_outb(RTC_ADDR_REG, RTC_REG_A);

        uint8_t regA;

        util_sys_inb(RTC_DATA_REG, &regA);

        if (regA & RTC_UIP)
        {
            continue;
        }
        else
        {
            time.seconds = read_register(RTC_SECONDS);

            time.minutes = read_register(RTC_MINUTES);

            time.hours = read_register(RTC_HOURS);

            time.day = read_register(RTC_DAY);

            time.month = read_register(RTC_MONTH);

            time.year = read_register(RTC_YEAR);

            break;
        }
    }
    return time;
}
