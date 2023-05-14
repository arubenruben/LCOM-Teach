#include "includes/uart.h"

int(uart_subscribe_int)(uint8_t *bit_no)
{

    printf("Not Implemented Yet!\n");
    return 1;
}

int(uart_enable_int)(void)
{
    printf("Not Implemented Yet!\n");
    return 1;
}

int(uart_disable_int)(void)
{
    printf("Not Implemented Yet!\n");
    return 1;
}

int(uart_unsubscribe_int)()
{

    printf("Not Implemented Yet!\n");
    return 1;
}

void(uart_ih)(void)
{

    printf("Not Implemented Yet!\n");
    return;
}

uint8_t(uart_read_line_control)(uint16_t base_addr)
{
    uint8_t read_byte = 0x00;

    if (util_sys_inb(base_addr + UART_LINE_CONTROL, &read_byte) != 0)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    return read_byte;
}

uint16_t(uart_read_bit_rate)(uint16_t base_addr)
{
    uint16_t read_result = 0x00;
    uint8_t read_byte = 0x00;
    uint8_t control_word=0x00;

    if((control_word = uart_read_line_control(base_addr))&& control_word == 1)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    control_word |= DLAB_BIT;

    if (sys_outb(base_addr + UART_LINE_CONTROL, control_word) != 0)
    {
        printf("Error activating dlab\n");
        return 1;
    }

    if (util_sys_inb(base_addr + UART_DIVISOR_LATCH_LSB, &read_byte) != 0)
    {
        printf("Error reading divisor latch lsb\n");
        return 1;
    }

    read_result |= read_byte;

    if (util_sys_inb(base_addr + UART_DIVISOR_LATCH_MSB, &read_byte) != 0)
    {
        printf("Error reading divisor latch msb\n");
        return 1;
    }

    read_result |= (read_byte << 8);

    control_word &= ~DLAB_BIT;

    if (sys_outb(base_addr + UART_LINE_CONTROL, control_word) != 0)
    {
        printf("Error deactivating dlab\n");
        return 1;
    }

    return MAX_BIT_RATE / read_result;
}

uint8_t line_control_word_length(uint32_t bits)
{
    switch (bits)
    {
    case 5:
        return 0x00;
        break;
    case 6:
        return BIT(0);
        break;

    case 7:
        return BIT(1);
        break;
    case 8:
        return BIT(1) | BIT(0);
        break;

    default:
        break;
    }

    printf("Error Panic Word Length\n");
    return 0;
}

uint8_t line_control_stop_bits(uint32_t stop)
{
    switch (stop)
    {
    case 1:
        return 0x00;
        break;
    case 2:
        return BIT(2);
        break;

    default:
        break;
    }

    printf("Error Panic Stop Bits\n");
    return 0;
}

uint8_t lines_control_parity_bits(long parity)
{
    switch (parity)
    {
    case 'N':
        return 0x00;
        break;
    case 'O':
        return BIT(3);
        break;

    case 'E':
        return BIT(4) | BIT(3);
        break;

    case '1':
        return BIT(5) | BIT(3);
        break;

    case '0':
        return BIT(5) | BIT(4) | BIT(3);
        break;

    default:
        break;
    }

    printf("Error Panic Parity\n");
    return 0;
}

int(uart_set_bit_rate)(uint16_t base_addr, uint32_t rate)
{

    if (rate > MAX_BIT_RATE)
    {
        printf("Error: rate is too high\n");
        return 1;
    }

    uint16_t reg_value = MAX_BIT_RATE / rate;

    uint8_t status = uart_read_line_control(base_addr);

    if (status == 1)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    if ((status & BIT(7)) == 0)
    {
        printf("DLAB not SET\n");
        return 1;
    }

    uint8_t lsb_rate = (uint8_t)reg_value;

    uint8_t msb_rate = (uint8_t)(reg_value >> 8);

    if (sys_outb(base_addr + UART_DIVISOR_LATCH_LSB, lsb_rate) != OK)
    {
        printf("Error writing to divisor latch lsb\n");
        return 1;
    }

    if (sys_outb(base_addr + UART_DIVISOR_LATCH_MSB, msb_rate) != OK)
    {
        printf("Error writing to divisor latch msb\n");
        return 1;
    }

    return 0;
}

int(uart_set_line_control)(uint16_t base_addr, uint32_t bits, uint32_t stop, long parity, uint32_t rate)
{
    uint8_t control_byte = 0x00;

    control_byte |= line_control_word_length(bits);

    control_byte |= line_control_stop_bits(stop);

    control_byte |= lines_control_parity_bits(parity);

    control_byte |= DLAB_BIT;

    if (sys_outb(base_addr + UART_LINE_CONTROL, control_byte) != OK)
    {
        printf("Error writing to line control register\n");
        return 1;
    }

    if (uart_set_bit_rate(base_addr, rate) != 0)
    {
        printf("Error setting bit rate\n");
        return 1;
    }

    control_byte &= ~DLAB_BIT;

    if (sys_outb(base_addr + UART_LINE_CONTROL, control_byte) != OK)
    {
        printf("Error writing to line control register\n");
        return 1;
    }

    return 0;
}
