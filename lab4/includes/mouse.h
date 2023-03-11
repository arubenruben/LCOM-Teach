#pragma once

#include "lcom/lcf.h"

#include "includes/constants.h"

typedef struct mouse_reading_t
{
    uint8_t packet;
    bool error;
    uint8_t byte_index;

} mouse_reading_t;

int(personal_mouse_enable_data_reporting)(void);

int(mouse_disable_data_reporting)(void);

int(mouse_subscribe_int)(uint8_t *bit_no);

int(mouse_unsubscribe_int)(void);

int(mouse_read_out_buf)(uint8_t *data, bool sleep);

void(mouse_ih)(void);

int(mouse_reading_task)(void);

int(mouse_set_stream_mode)(void);
