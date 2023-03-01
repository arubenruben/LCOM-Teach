#pragma once

#include "lcom/lcf.h"
#include "lcom/utils.h"

#define KBC_IRQ 1

#define STAT_REG 0x64

#define OUT_BUF 0x60

#define WRITE_COMMAND_BYTE 0x60

#define READ_COMMAND_BYTE 0x20

#define ERROR_KBC (BIT(7)|BIT(6))

#define MOUSE BIT(5)

#define INH BIT(4)

#define IBF BIT(1)

#define OBF BIT(0)

#define TIMEOUT BIT(6)

#define ESC_BREAK_CODE 0x81

#define COMMAND_BYTE_KEYBOARD_INTERRUPT_MASK 0xFE;

#define MAX_TRIES 5

#define DELAY_US 20000
