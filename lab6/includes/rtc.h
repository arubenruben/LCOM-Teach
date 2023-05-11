#pragma once

#include <lcom/lcf.h>

#include "includes/constants.h"

typedef struct rtc
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} rtc_t;

int(rtc_subscribe_int)(uint8_t *bit_no);

int(rtc_enable_interrupts)(void);

int(rtc_unsubscribe_int)(void);

int(rtc_define_alarm)(uint8_t seconds, uint8_t minutes, uint8_t hours);

void(rtc_ih)(void);

rtc_t(read_time)();
