#include "includes/rtc.h"

static int rtc_hook_id;

int(rtc_subscribe_int)(uint8_t *bit_no)
{
    rtc_hook_id = *bit_no;

    if (sys_irqsetpolicy(RTC_IRQ_LINE, IRQ_REENABLE, &rtc_hook_id) != OK)
    {
        printf("Error setting policy\n");
        return 1;
    }

    return 0;
}

int(rtc_enable_interrupts)(void)
{
    uint8_t aux_byte = 0x00;

    // Force that Register C is read. According to the specifications, this is necessary to enable interrupts
    if (sys_outb(RTC_ADDR_REG, RTC_REG_C) != OK)
    {
        printf("Error writing to RTC_ADDR_REG\n");
        return 1;
    }

    if (util_sys_inb(RTC_DATA_REG, &aux_byte) != OK)
    {
        printf("Error reading from RTC_DATA_REG\n");
        return 1;
    }
    // Update Register B Has 3 Stages
    // Read Register B
    if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != OK)
    {
        printf("Error writing to RTC_ADDR_REG\n");
        return 1;
    }

    if (util_sys_inb(RTC_DATA_REG, &aux_byte) != OK)
    {
        printf("Error reading from RTC_DATA_REG\n");
        return 1;
    }
    // Set the AIE bit
    aux_byte |= RTC_AIE_BIT_MASK;

    // Update Register B
    if (sys_outb(RTC_ADDR_REG, RTC_REG_B) != OK)
    {
        printf("Error writing to RTC_ADDR_REG\n");
        return 1;
    }

    if (sys_outb(RTC_DATA_REG, aux_byte) != OK)
    {
        printf("Error writing to RTC_DATA_REG\n");
        return 1;
    }

    return 0;
}

int(rtc_define_alarm)(uint8_t seconds, uint8_t minutes, uint8_t hours)
{

    for (size_t i = 0; i < MAX_RTC_TRIES; i++)
    {
        sys_outb(RTC_ADDR_REG, RTC_REG_A);

        uint8_t regA;

        util_sys_inb(RTC_DATA_REG, &regA);

        if (regA & RTC_UIP)
        {
            continue;
        }

        if (sys_outb(RTC_ADDR_REG, RTC_SECONDS_ALARM) != OK)
        {
            printf("Error writing to RTC_ADDR_REG\n");
            return 1;
        }

        if (sys_outb(RTC_DATA_REG, seconds) != OK)
        {
            printf("Error writing to RTC_DATA_REG\n");
            return 1;
        }

        if (sys_outb(RTC_ADDR_REG, RTC_MINUTES_ALARM) != OK)
        {
            printf("Error writing to RTC_ADDR_REG\n");
            return 1;
        }

        if (sys_outb(RTC_DATA_REG, minutes) != OK)
        {
            printf("Error writing to RTC_DATA_REG\n");
            return 1;
        }

        if (sys_outb(RTC_ADDR_REG, RTC_HOURS_ALARM) != OK)
        {
            printf("Error writing to RTC_ADDR_REG\n");
            return 1;
        }

        if (sys_outb(RTC_DATA_REG, hours) != OK)
        {
            printf("Error writing to RTC_DATA_REG\n");
            return 1;
        }

        return 0;
    }

    printf("Error setting alarm\n");

    return 1;
}

int(rtc_unsubscribe_int)(void)
{
    if (sys_irqrmpolicy(&rtc_hook_id) != OK)
    {
        printf("Error removing policy\n");
        return 1;
    }
    return 0;
}

void(handle_alarm_int)(void)
{
    time_t epoch_time = time(NULL);

    printf("Alarm Interrupt\n");
    printf("Epoch Time: %d\n", epoch_time);
}

void(rtc_ih)(void)
{

    uint8_t aux_byte = 0x00;

    sys_outb(RTC_ADDR_REG, RTC_REG_C);

    util_sys_inb(RTC_DATA_REG, &aux_byte);

    if (aux_byte & RTC_UF)
        printf("Update Interrupt. Not Implemented\n");

    if (aux_byte & RTC_AF)
        handle_alarm_int();

    if (aux_byte & RTC_PF)
        printf("Periodic Interrupt. Not Implemented\n");
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
