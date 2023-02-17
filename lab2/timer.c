#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"

int(timer_set_frequency)(uint8_t timer, uint32_t freq)
{

  if (freq > TIMER_FREQ)
  {
    printf("Invalid frequency!\n");
    return -1;
  }

  uint8_t st = 0x00;

  // Read current timer configuration
  if (timer_get_conf(timer, &st))
  {
    printf("Error reading timer configuration!\n");
    return -1;
  }


  // Select LSB of Status Register
  uint8_t first_four_st_bits = st & (BIT(0) | BIT(1) | BIT(2) | BIT(3));

  // Set Operating Mode
  uint8_t initialization_mode = TIMER_LSB_MSB;

  // Set Timer Register
  uint8_t timer_register = (TIMER_0 + timer) << 6;
  
  // Construct Control Word
  uint8_t control_word = timer_register | first_four_st_bits | initialization_mode;

  // Write Control Word to Control Register
  if (sys_outb(TIMER_CTRL, (uint8_t)control_word))
  {
    printf("Error writing to control register!\n");
    return -1;
  }

  // Calculate Timer Initial Value
  uint16_t initial_value = TIMER_FREQ / freq;

  // Write Timer Initial LSB Value To Timer Register
  if (sys_outb(TIMER_0 + timer, (uint8_t)initial_value))
  {
    printf("Error writing LSB to timer register!\n");
    return -1;
  }

  // Write Timer Initial MSB Value To Timer Register
  if (sys_outb(TIMER_0 + timer, (uint8_t)(initial_value >> 8)))
  {
    printf("Error writing MSB to timer register!\n");
    return -1;
  }

  return 0;
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

  if (sys_outb(TIMER_CTRL, read_back_cmd))
  {
    printf("Error writing to timer control register!\n");
    return -1;
  }

  // Read Timer Register
  if (util_sys_inb(TIMER_0 + timer, st))
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
  {
    uint8_t operating_mode_mask = BIT(3) | BIT(2) | BIT(1);

    uint8_t value_right_shifted_normalized = (st & operating_mode_mask) >> 1;

    if (value_right_shifted_normalized == 6)
      value_right_shifted_normalized = 2;

    if (value_right_shifted_normalized == 7)
      value_right_shifted_normalized = 3;

    val.count_mode = value_right_shifted_normalized;
  }

  else if (field == tsf_base)
    val.bcd = st & TIMER_BCD;

  else
  {
    printf("Invalid timer status field!\n");
    return -1;
  }

  if (timer_print_config(timer, field, val))
  {
    printf("Error printing timer configuration!\n");
    return -1;
  }

  return 0;
}
