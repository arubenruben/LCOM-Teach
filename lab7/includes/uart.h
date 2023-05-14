#pragma once

#include <lcom/lcf.h>
#include "includes/constants.h"

int(uart_subscribe_int)(uint8_t *bit_no);

int(uart_enable_int)(uint16_t base_addr);

int(uart_disable_int)(uint16_t base_addr);

int(uart_unsubscribe_int)();

void(uart_ih)(void);

uint8_t(uart_read_line_control)(uint16_t base_addr);

uint16_t(uart_read_bit_rate)(uint16_t base_addr);

int(uart_set_line_control)(uint16_t base_addr, uint32_t bits, uint32_t stop, long parity, uint32_t rate);

int(uart_read_data)(uint8_t *data);

int(uart_write_data)(uint8_t data);
