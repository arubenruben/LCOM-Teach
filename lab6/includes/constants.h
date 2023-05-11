#pragma once

#define RTC_IRQ 8

#define RTC_UF BIT(4)

#define RTC_AF BIT(5)

#define RTC_PF BIT(6)

#define RTC_ADDR_REG 0x70

#define RTC_DATA_REG 0x71

#define RTC_REG_A 0x0A

#define RTC_REG_B 0x0B

#define RTC_REG_C 0x0C

#define RTC_REG_D 0x0D

#define MAX_RTC_TRIES 5

#define RTC_SECONDS 0x00

#define RTC_MINUTES 0x02

#define RTC_HOURS 0x04

#define RTC_DAY 0x07

#define RTC_MONTH 0x08

#define RTC_YEAR 0x09

#define RTC_UIP BIT(7)

#define RTC_DM BIT(2)

#define BCD_TO_BIN(x) ((x >> 4) * 10 + (x & 0x0F))

#define RTC_IRQ_LINE 8

#define RTC_AIE_BIT_MASK BIT(5)

#define RTC_SECONDS_ALARM 0x01

#define RTC_MINUTES_ALARM 0x03

#define RTC_HOURS_ALARM 0x05
