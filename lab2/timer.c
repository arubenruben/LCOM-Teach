#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int(timer_set_frequency)(uint8_t timer, uint32_t freq)
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_subscribe_int)(uint8_t *bit_no)
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(timer_unsubscribe_int)()
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

void(timer_int_handler)()
{
  /* To be implemented by the students */
  printf("%s is not yet implemented!\n", __func__);
}

int(timer_get_conf)(uint8_t timer, uint8_t *st)
{
  // Construct Read-Back Command
  uint32_t read_back_cmd = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);

  if (!sys_outb(TIMER_CTRL, read_back_cmd))
    return -1;

  // Read Timer Register
  if (!util_sys_inb(TIMER_0 + timer, st))
    return -1;

  return 0;
}

int(timer_display_conf)(uint8_t timer, uint8_t st, enum timer_status_field field)
{
  union timer_status_field_val val;

  if (field == tsf_all)
    val.byte = st;

  else if (field == tsf_initial)
    val.in_mode = (st & TIMER_LSB_MSB) >> 4;

  else if (field == tsf_mode)
    val.count_mode = (st & TIMER_SQR_WAVE) >> 1;

  else if (field == tsf_base)
    val.bcd = st & TIMER_BCD;

  else
    return -1;

  if(!timer_print_config(timer, field, val))
    return -1;

  return 0;
}
