#pragma once

#define BIOS_VIDEO_SERVICES 0x10

#define AH_SET_VBE_MODE 0x4F

#define AL_SET_VBE_MODE 0x02

#define LINEAR_FRAME_BUFFER BIT(14)

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

#define MODE_0X110_15BIT_PROBLEM 0x7fff

#define MODE_0x14C_8bits_PROBLEM 0xFFFFFF

#define INDEXED_MODE 0x105

