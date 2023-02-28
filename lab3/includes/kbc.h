#pragma once

#include <lcom/lcf.h>
#include "includes/constants.h"

typedef struct scan_code_t
{
    uint8_t scan_code;
    bool error;
    bool expecting_second_byte;
} scan_code_t;

int(kbc_subscribe_int)(uint8_t *bit_no);

void (kbc_ih)(void);

int(kbc_unsubscribe_int)(void);