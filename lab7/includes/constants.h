#pragma once

#include <lcom/lcf.h>

#define IRQ_COM1 4

#define IRQ_COM2 3

#define COM1_BASE_ADDR 0x3F8

#define COM2_BASE_ADDR 0x2F8

#define UART_RECEIVER_BUFFER 0

#define UART_TRANSMITTER_BUFFER 0

#define UART_INTERRUPT_ENABLE 1

#define UART_INTERRUPT_ID 2

#define UART_FIFO_CONTROL 2

#define UART_LINE_CONTROL 3

#define UART_MODEM_CONTROL 4

#define UART_LINE_STATUS 5

#define UART_MODEM_STATUS 6

#define UART_SCRATCHPAD 7

#define UART_DLL 0

#define UART_DLM 1

#define UART_IER 1

#define UART_IIR 2

#define UART_FCR 2

#define UART_LCR 3

#define UART_MCR 4

#define UART_LSR 5

#define UART_MSR 6

#define UART_SR 7

#define UART_THR 0

#define UART_RBR 0

#define UART_DIVISOR_LATCH_MSB 1

#define UART_DIVISOR_LATCH_LSB 0

#define MAX_BIT_RATE 115200
