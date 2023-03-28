#pragma once

#include <lcom/lcf.h>

int vg_map(vbe_mode_info_t vbe_mode_info);

void *(vg_init)(uint16_t mode);

int vbe_set_mode(uint16_t mode);

