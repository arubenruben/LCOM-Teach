#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>

#include "includes/kbc.h"
#include "timer.h"

#include <sys/time.h>

extern scan_code_t scan_code;

int main(int argc, char *argv[])
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

void(kbc_reading_task)(bool *is_make_code, uint8_t *num_valid_scan_codes, uint8_t *scan_codes)
{
  if (scan_code.error)
  {
    printf("Error reading scancode\n");
    return;
  }

  if (scan_code.scan_code >= BIT(7))
    *is_make_code = false;

  if (scan_code.scan_code == 0xE0)
  {
    scan_code.expecting_second_byte = true;
    *num_valid_scan_codes = 2;
  }
  else
  {
    scan_code.expecting_second_byte = false;
  }

  if (scan_codes[0] == 0xE0)
  {
    scan_codes[1] = scan_code.scan_code;
    *num_valid_scan_codes = 2;
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

  kbd_print_scancode(*is_make_code, *num_valid_scan_codes, scan_codes);
}

int(kbd_test_scan)()
{
  // subscribes kbc interrupts
  uint8_t bit_no = 1;

  uint8_t scan_codes[2];

  int ipc_status, r;
  message msg;

  if (kbc_subscribe_int(&bit_no) != 0)
  {
    printf("Error subscribing KBC interrupts\n");
    return 1;
  }

  while (scan_code.scan_code != ESC_BREAK_CODE)
  {
    /* Get a request message. */
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
        if (msg.m_notify.interrupts & BIT(bit_no))
        {

          bool is_make_code = true;
          uint8_t num_valid_scan_codes = 1;

          kbc_ih();

          kbc_reading_task(&is_make_code, &num_valid_scan_codes, scan_codes);
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

  // unsubscribes kbc interrupts
  if (kbc_unsubscribe_int() != 0)
  {
    printf("Error unsubscribing KBC interrupts\n");
    return 1;
  }

  return 0;
}

int(kbd_test_poll)()
{
  uint8_t command_byte;

  uint8_t scan_codes[2];

  while (scan_code.scan_code != ESC_BREAK_CODE)
  {
    bool is_make_code = true;
    uint8_t num_valid_scan_codes = 1;

    kbc_ih();

    if (!scan_code.error)
      kbc_reading_task(&is_make_code, &num_valid_scan_codes, scan_codes);
  }

  // read command byte
  if (kbc_read_command(&command_byte) != 0)
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

  // Enable keyboard interrupts
  if (kbc_write_command(command_byte) != 0)
  {
    printf("Error writing command byte\n");
    return 1;
  }

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n)
{

  struct timeval tv,tv2;
  
  
  // subscribes kbc interrupts
  uint8_t timer_bit_no = 0;
  uint8_t kb_bit_no = 1;
  uint8_t scan_codes[2];

  uint64_t seconds_counter = 0;

  int ipc_status, r;
  message msg;

  if (timer_subscribe_int(&timer_bit_no) != 0)
  {
    printf("Error subscribing timer interrupts\n");
    return 1;
  }

  if (kbc_subscribe_int(&kb_bit_no) != 0)
  {
    printf("Error subscribing KBC interrupts\n");
    return 1;
  }
  
  gettimeofday(&tv, NULL);

  while (scan_code.scan_code != ESC_BREAK_CODE)
  {
    gettimeofday(&tv2, NULL);

    long long milliseconds1 = (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;
  
    long long milliseconds2 = (long long)tv2.tv_sec * 1000LL + tv2.tv_usec / 1000;
    
    if(milliseconds2 - milliseconds1 >= n*1000)
      break;

    /* Get a request message. */
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

        if (msg.m_notify.interrupts & BIT(timer_bit_no))
        {
          timer_int_handler();          
        }
        if (msg.m_notify.interrupts & BIT(kb_bit_no))
        {
          bool is_make_code = true;

          uint8_t num_valid_scan_codes = 1;

          kbc_ih();
          
          gettimeofday(&tv, NULL);

          kbc_reading_task(&is_make_code, &num_valid_scan_codes, scan_codes);

          seconds_counter = 0;
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
  gettimeofday(&tv2, NULL);
  // unsubscribes kbc interrupts
  if (kbc_unsubscribe_int() != 0)
  {
    printf("Error unsubscribing KBC interrupts\n");
    return 1;
  }

  if (timer_unsubscribe_int() != 0)
  {
    printf("Error unsubscribing timer interrupts\n");
    return 1;
  }

  long long milliseconds1 = (long long)tv.tv_sec * 1000LL + tv.tv_usec / 1000;
  
  long long milliseconds2 = (long long)tv2.tv_sec * 1000LL + tv2.tv_usec / 1000;
  
  printf("Current time in milliseconds: %lld\n", milliseconds2-milliseconds1);
  
  return 0;
}
