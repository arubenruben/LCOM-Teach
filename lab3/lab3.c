#include <lcom/lcf.h>

#include <lcom/lab3.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/time.h>

#include "includes/timer.h"
#include "includes/keyboard.h"

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

int(kbd_test_scan)()
{
  // subscribes kbc interrupts
  uint8_t bit_no = 1;

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
          kbc_ih();
          if (!scan_code.error)
            kbc_reading_task();
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

  while (scan_code.scan_code != ESC_BREAK_CODE)
  {
    kbc_ih();
    if (!scan_code.error)
      kbc_reading_task();
  }

  if (kbc_enable_interrupts() != 0)
  {
    printf("Error enabling interrupts\n");
    return 1;
  }

  return 0;
}

int(kbd_test_timed_scan)(uint8_t n)
{
  // subscribes kbc interrupts
  uint8_t timer_bit_no = 0;
  uint8_t kb_bit_no = 1;
  uint8_t seconds_counter = 0;

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

  while (scan_code.scan_code != ESC_BREAK_CODE && seconds_counter < n)
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

        if (msg.m_notify.interrupts & BIT(timer_bit_no))
        {
          timer_int_handler();

          if (counter % sys_hz() == 0)
            seconds_counter++;
        }
        if (msg.m_notify.interrupts & BIT(kb_bit_no))
        {
          kbc_ih();

          if (!scan_code.error)
            kbc_reading_task();

          counter = 0;
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

  return 0;
}
