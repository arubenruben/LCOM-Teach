#pragma once

#include "lcom/lcf.h"
#include "lcom/utils.h"

#define KBC_IRQ 1

#define STAT_REG 0x64

#define OUT_BUF 0x60

#define ERROR_KBC (BIT(7)|BIT(6))

#define MOUSE BIT(5)

#define INH BIT(4)

#define IBF BIT(1)

#define OBF BIT(0)

#define ESC_BREAK_CODE 0x81
