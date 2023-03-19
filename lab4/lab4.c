// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>

#include "includes/mouse.h"
#include "includes/keyboard.h"
#include "includes/constants.h"
#include "includes/timer.h"

// Any header files included below this line should have been created by you

int main(int argc, char *argv[])
{
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(mouse_test_packet)(uint32_t cnt)
{
  uint8_t bit_no = 12;

  int ipc_status, r;
  message msg;

  if (mouse_subscribe_int(&bit_no) != 0)
  {
    printf("Error in mouse_subscribe_int()\n");
    return 1;
  }

  if (mouse_disable_interrupts() != 0)
  {
    printf("Error in mouse_disable_interrupts()\n");
    return 1;
  }

  if (personal_mouse_enable_data_reporting() != 0)
  {
    printf("Error in personal_mouse_enable_data_reporting()\n");
    return 1;
  }

  if (mouse_enable_interrupts() != 0)
  {
    printf("Error in mouse_enable_interrupts()\n");
    return 1;
  }

  while (cnt > 0)
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
          mouse_ih();
          if (mouse_packet.error == false && mouse_reading_task() == 0)
          {
            cnt--;
          }
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

  if ((mouse_disable_interrupts()) != 0)
  {
    printf("Error in mouse_disable_interrupts()\n");
    return 1;
  }

  if (mouse_disable_data_reporting() != 0)
  {
    printf("Error in mouse_disable_data_reporting()\n");
    return 1;
  }

  if (mouse_enable_interrupts() != 0)
  {
    printf("Error in mouse_enable_interrupts()\n");
    return 1;
  }

  if (mouse_unsubscribe_int() != 0)
  {
    printf("Error in mouse_unsubscribe_int()\n");
    return 1;
  }

  return 0;
}

int(mouse_test_async)(uint8_t idle_time)
{
  uint8_t timer_bit_no=0;
  uint8_t mouse_bit_no=12;
  uint8_t seconds_counter=0;
  
  int ipc_status, r;
  message msg;

  if (timer_subscribe_int(&timer_bit_no) != 0)
  {
    printf("Error in timer_subscribe_int()\n");
    return 1;
  }

  if (mouse_subscribe_int(&mouse_bit_no) != 0)
  {
    printf("Error in mouse_subscribe_int()\n");
    return 1;
  }

  if (mouse_disable_interrupts() != 0)
  {
    printf("Error in mouse_disable_interrupts()\n");
    return 1;
  }

  if (personal_mouse_enable_data_reporting() != 0)
  {
    printf("Error in personal_mouse_enable_data_reporting()\n");
    return 1;
  }

  if (mouse_enable_interrupts() != 0)
  {
    printf("Error in mouse_enable_interrupts()\n");
    return 1;
  }

  while (seconds_counter<idle_time)
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

        if (msg.m_notify.interrupts & BIT(timer_bit_no))
        {
          timer_int_handler();

          if (counter % sys_hz() == 0)
            seconds_counter++;
        }
        if (msg.m_notify.interrupts & BIT(mouse_bit_no))
        {
          mouse_ih();          
          if (mouse_packet.error == false && mouse_reading_task() == 0)
          {
            seconds_counter=0;
            counter=0;
          }         
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

  if ((mouse_disable_interrupts()) != 0)
  {
    printf("Error in mouse_disable_interrupts()\n");
    return 1;
  }

  if (mouse_disable_data_reporting() != 0)
  {
    printf("Error in mouse_disable_data_reporting()\n");
    return 1;
  }

  if (mouse_enable_interrupts() != 0)
  {
    printf("Error in mouse_enable_interrupts()\n");
    return 1;
  }

  if (mouse_unsubscribe_int() != 0)
  {
    printf("Error in mouse_unsubscribe_int()\n");
    return 1;
  }
  
  if(timer_unsubscribe_int()!=0)
  {
    printf("Error in timer_unsubscribe_int()\n");
    return 1;
  }

  return 0;
}

int(mouse_test_gesture)(uint8_t x_len, uint8_t tolerance)
{
  /* To be completed */
  printf("%s: under construction\n", __func__);
  return 1;
}
