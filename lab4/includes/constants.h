#pragma once

#define MOUSE_IRQ 12

#define MAX_TRIES 3

#define DELAY_US 20000

#define OBF BIT(0)

#define STAT_REG 0x64

#define OUT_BUF 0x60

#define ERROR_KBC (BIT(7) | BIT(6))

#define MOUSE BIT(5)

#define INH BIT(4)

#define IBF BIT(1)

#define OBF BIT(0)

#define TIMEOUT BIT(6)

#define AUX BIT(5)

#define TWO_COMPLEMENT 0xFF00
