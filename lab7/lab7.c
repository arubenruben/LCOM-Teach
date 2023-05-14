#include "includes/lab7.h"

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

int print_byte(uint8_t byte)
{
    // iterate over each bit in the byte
    for (int i = 7; i >= 0; i--)
    {
        uint8_t bit = (byte >> i) & 1; // get the i-th bit of the byte
        printf("%u", bit);             // print the bit value (0 or 1)
    }
    printf("\n");
    return 0;
}

int(ser_test_conf)(uint16_t base_addr)
{
    uint8_t readed_value = 0x00;
    uint16_t bit_rate = 0x00;

    if ((readed_value = uart_read_line_control(base_addr)) && readed_value == 1)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    if ((bit_rate = uart_read_bit_rate(COM1_BASE_ADDR)) && bit_rate == 1)
    {
        printf("Error reading bit rate\n");
        return 1;
    }

    printf("Line control register value: ");

    print_byte(readed_value);

    printf("Bit rate: %u\n", bit_rate);

    return 0;
}

int(ser_test_set)(uint16_t base_addr, uint32_t bits, uint32_t stop, long parity, uint32_t rate)
{
    if (uart_set_line_control(base_addr, bits, stop, parity, rate) != 0)
    {
        printf("Error setting line control register\n");
        return 1;
    }
    return 0;
}

int(proj_main_loop)(int argc, char **argv)
{
    
    if (ser_test_set(COM1_BASE_ADDR, 8, 1, 'E', 5000) != 0)
    {
        printf("Error setting line control register\n");
        return 1;
    }

    if (ser_test_conf(COM1_BASE_ADDR) != 0)
    {
        printf("Error reading line control register\n");
        return 1;
    }

    // ser_test_conf(COM2_BASE_ADDR);

    return 0;
}
