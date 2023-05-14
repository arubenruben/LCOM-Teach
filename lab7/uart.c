#include "includes/uart.h"

static int uart_hook_id;

static uint16_t global_base_addr;

static uint8_t global_word_size;

uint8_t data = 0x00;

int(uart_subscribe_int)(uint8_t *bit_no)
{
    uart_hook_id = *bit_no;

    if (sys_irqsetpolicy(IRQ_COM1, (IRQ_REENABLE | IRQ_EXCLUSIVE), &uart_hook_id) != 0)
    {
        printf("Error setting policy\n");
        return 1;
    }

    return 0;
}

int(uart_enable_int)(uint16_t base_addr)
{
    uint8_t byte_aux = 0x00;

    if ((byte_aux = uart_read_line_control(base_addr)) && byte_aux == 1)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    if (byte_aux & DLAB_BIT)
    {
        printf("DLAB bit is set\n");
        return 1;
    }

    if (util_sys_inb(base_addr + UART_INTERRUPT_ENABLE, &byte_aux) != 0)
    {
        printf("Error reading interrupt enable register\n");
        return 1;
    }

    byte_aux |= UART_INT_ENABLE;

    if (sys_outb(base_addr + UART_INTERRUPT_ENABLE, byte_aux) != 0)
    {
        printf("Error writing interrupt enable register\n");
        return 1;
    }

    return 0;
}

int(uart_disable_int)(uint16_t base_addr)
{
    uint8_t byte_aux = 0x00;

    if ((byte_aux = uart_read_line_control(base_addr)) && byte_aux == 1)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    if (byte_aux & DLAB_BIT)
    {
        printf("DLAB bit is set\n");
        return 1;
    }

    if (util_sys_inb(COM1_BASE_ADDR + UART_INTERRUPT_ENABLE, &byte_aux) != 0)
    {
        printf("Error reading interrupt enable register\n");
        return 1;
    }

    byte_aux &= ~UART_INT_ENABLE;

    if (sys_outb(COM1_BASE_ADDR + UART_INTERRUPT_ENABLE, byte_aux) != 0)
    {
        printf("Error writing interrupt enable register\n");
        return 1;
    }

    return 0;
}

int(uart_unsubscribe_int)()
{
    if (sys_irqrmpolicy(&uart_hook_id) != 0)
    {
        printf("Error removing policy\n");
        return 1;
    }

    return 0;
}

void(uart_ih)(void)
{
    uint8_t interrupt_status = 0x00;

    if (util_sys_inb(global_base_addr + UART_INTERRUPT_ID, &interrupt_status) != 0)
    {
        printf("Error reading interrupt status\n");
        return;
    }

    if ((interrupt_status & INTERRUPT_STATUS) != 0)
    {
        printf("Interrupt is Pending\n");
        return;
    }

    switch (interrupt_status & INTERRUPT_ID)
    {

    case MODEM_INTERRUPT:
        printf("Modem interrupt\n");
        break;

    case TRANSMITTER_INTERRUPT:
        printf("Transmitter interrupt\n");
        break;

    case TIMEOUT_INTERRUPT:
        printf("Timeout interrupt\n");
        break;

    case RECEIVER_INTERRUPT:
        uart_read_data(&data);
        break;

    case LINE_STATUS_INTERRUPT:
        printf("Line status interrupt\n");
        break;

    default:
        break;
    }

    printf("Not implemented\n");
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
    uint8_t control_word = 0x00;

    if ((control_word = uart_read_line_control(base_addr)) && control_word == 1)
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

    global_base_addr = base_addr;
    global_word_size = bits;

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

int(ensure_word_size(uint8_t data))
{
    if (global_word_size == 5)
    {
        if ((data & 0x1F) != data)
        {
            printf("Error: word size is 5 but data is bigger than 5 bits\n");
            return 1;
        }
    }
    else if (global_word_size == 6)
    {
        if ((data & 0x3F) != data)
        {
            printf("Error: word size is 6 but data is bigger than 6 bits\n");
            return 1;
        }
    }
    else if (global_word_size == 7)
    {
        if ((data & 0x7F) != data)
        {
            printf("Error: word size is 7 but data is bigger than 7 bits\n");
            return 1;
        }
    }
    else if (global_word_size == 8)
    {
        if ((data & 0xFF) != data)
        {
            printf("Error: word size is 8 but data is bigger than 8 bits\n");
            return 1;
        }
    }
    else
    {
        printf("Error: word size is not 5, 6, 7 or 8\n");
        return 1;
    }

    return 0;
}

int(uart_read_data)(uint8_t *data)
{

    uint8_t byte_aux = 0x00;

    if (data == NULL)
    {
        printf("Error: data is NULL\n");
        return 1;
    }

    if ((byte_aux = uart_read_line_control(global_base_addr)) && byte_aux == 1)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    if (byte_aux & DLAB_BIT)
    {
        printf("Error: DLAB is set\n");
        return 1;
    }

    if (util_sys_inb(global_base_addr + UART_RECEIVER_BUFFER, data) != OK)
    {
        printf("Error reading receiver buffer\n");
        data = NULL;
        return 1;
    }

    return 0;
}

int(uart_write_data)(uint8_t data)
{

    uint8_t byte_aux = 0x00;

    if (ensure_word_size(data) != 0)
    {
        printf("Error: word size is not 5, 6, 7 or 8\n");
        return 1;
    }

    if ((byte_aux = uart_read_line_control(global_base_addr)) && byte_aux == 1)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    if (byte_aux & DLAB_BIT)
    {
        printf("Error: DLAB is set\n");
        return 1;
    }

    if (sys_outb(global_base_addr + UART_TRANSMITTER_BUFFER, data) != OK)
    {
        printf("Error writing to transmitter buffer\n");
        return 1;
    }

    return 0;
}
