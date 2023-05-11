#include "includes/lab6.h"

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

void print_byte(unsigned char byte)
{
    int i;
    for (i = 7; i >= 0; i--)
    {                                     // Loop through each bit of the byte
        printf("%d", (byte >> i) & 0x01); // Shift the bit into the least significant position and print it out
    }
    printf("\n");
}

int(rtc_test_conf)()
{
    uint8_t conf;

    sys_outb(RTC_ADDR_REG, RTC_REG_A);

    util_sys_inb(RTC_DATA_REG, &conf);

    printf("\nConf Register A: ");
    print_byte(conf);

    sys_outb(RTC_ADDR_REG, RTC_REG_B);

    util_sys_inb(RTC_DATA_REG, &conf);

    printf("Conf Register B: ");
    print_byte(conf);

    sys_outb(RTC_ADDR_REG, RTC_REG_C);

    util_sys_inb(RTC_DATA_REG, &conf);

    printf("Conf Register C: ");
    print_byte(conf);

    return 0;
}

int(rtc_test_date)()
{
    rtc_t return_value = read_time();

    printf("Date: %02x/%02x/%02x\n", return_value.day, return_value.month, return_value.year);

    printf("Time: %02x:%02x:%02x\n", return_value.hours, return_value.minutes, return_value.seconds);

    return 0;
}

int(rtc_test_int)()
{
    uint8_t rtc_bit_no = RTC_IRQ_LINE;

    int ipc_status, r;
    message msg;
    
    printf("Aqui 1\n");

    if (rtc_subscribe_int(&rtc_bit_no) != 0)
    {
        printf("Error subscribing interrupts\n");
        return 1;
    }

    printf("Aqui 2\n");

    if (rtc_enable_interrupts() != 0)
    {
        printf("Error enabling interrupts\n");
        return 1;
    }

    printf("Aqui 3\n");

    if (rtc_define_alarm(10, 0, 0) != 0)
    {
        printf("Error defining alarm\n");
        return 1;
    }

    printf("Subscribed interrupts\n");

    while (true)
    {
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
                if (msg.m_notify.interrupts & BIT(rtc_bit_no))
                {
                    printf("rtc_ih\n");
                    rtc_ih();
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

    printf("Unsubscribing interrupts\n");

    if (rtc_unsubscribe_int() != 0)
    {
        printf("Error unsubscribing interrupts\n");
        return 1;
    }

    return 0;
}

int(proj_main_loop)(int argc, char **argv)
{
    // rtc_test_conf();

    // rtc_test_date();

    rtc_test_int();

    return 0;
}
