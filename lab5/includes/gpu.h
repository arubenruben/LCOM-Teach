#pragma once

#include <lcom/lcf.h>

int vg_map(vbe_mode_info_t vbe_mode_info);

void *(vg_init)(uint16_t mode);

int vbe_set_mode(uint16_t mode);

uint16_t get_h_res();

uint16_t get_v_res();

uint8_t get_bits_per_pixel();

uint8_t get_RedMaskSize();
uint8_t get_RedFieldPosition();
uint8_t get_GreenMaskSize();
uint8_t get_GreenFieldPosition();
uint8_t get_BlueMaskSize();
uint8_t get_BlueFieldPosition();

int(draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);

int(erase_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y);
