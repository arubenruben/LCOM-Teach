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

void(kbc_ih)(void);

int(kbc_read_command_byte)(uint8_t *command);

int(kbc_write_command_byte)(uint8_t command);

int(kbc_unsubscribe_int)(void);

void(kbc_reading_task)();

int(kbc_enable_interrupts)(void);

int(kbc_disable_interrupts)(void);

int(kbc_read_dummy_byte)(void);
