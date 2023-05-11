#include "includes/utils.h"

int(util_get_LSB)(uint16_t val, uint8_t *lsb)
{
  // Extract LSB from a 16-bit integer
  *lsb = (uint8_t)val;

  return 0;
}

int(util_get_MSB)(uint16_t val, uint8_t *msb)
{
  // Extract MSB from a 16-bit integer

  *msb = (uint8_t)(val >> 8);

  return 0;
}

int(util_sys_inb)(int port, uint8_t *value)
{
  // Extract the first byte of a 32-bit integer
  uint32_t byte = 0x00;

  if (sys_inb(port, &byte) != 0)
  {
    printf("Error reading from port!\n");
    return -1;
  }

  *value = (uint8_t)byte;

  return 0;
}
