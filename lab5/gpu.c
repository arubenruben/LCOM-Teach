#include <lcom/lcf.h>
#include "includes/gpu.h"
#include "includes/constants.h"

static uint8_t *video_mem;

static uint16_t h_res;
static uint16_t v_res;
static uint8_t bits_per_pixel;

static uint8_t RedMaskSize;        /* size of direct color red mask in bits */
static uint8_t RedFieldPosition;   /* bit position of lsb of red mask */
static uint8_t GreenMaskSize;      /* size of direct color green mask in bits */
static uint8_t GreenFieldPosition; /* bit position of lsb of green mask */
static uint8_t BlueMaskSize;       /* size of direct color blue mask in bits */
static uint8_t BlueFieldPosition;  /* bit position of lsb of blue mask */

int vbe_set_mode(uint16_t mode)
{
    reg86_t reg86;

    memset(&reg86, 0, sizeof(reg86));

    reg86.intno = BIOS_VIDEO_SERVICES;
    reg86.ah = AH_SET_VBE_MODE;
    reg86.al = AL_SET_VBE_MODE;
    reg86.bx = mode | LINEAR_FRAME_BUFFER;

    if (sys_int86(&reg86) != OK)
    {
        printf("Error in sys_int86() in vbe_set_mode()");
        return 1;
    }

    return 0;
}

int vg_map(vbe_mode_info_t vbe_mode_info)
{
    int r;
    struct minix_mem_range mr;

    unsigned int vram_base = vbe_mode_info.PhysBasePtr; /* VRAM's physical addresss */

    unsigned int vram_size = (vbe_mode_info.XResolution * vbe_mode_info.YResolution * vbe_mode_info.BitsPerPixel) / 8; /* VRAM's size, but you can use the frame-buffer size, instead */

    /* Allow memory mapping */
    mr.mr_base = (phys_bytes)vram_base;
    mr.mr_limit = mr.mr_base + vram_size;

    if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
        panic("sys_privctl (ADD_MEM) failed: %d\n", r);

    /* Map memory */

    video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

    if (video_mem == MAP_FAILED)
        panic("couldn't map video memory");

    return 0;
}

void *(vg_init)(uint16_t mode)
{
    // Read the mode info
    vbe_mode_info_t vbe_mode_info;

    if (vbe_get_mode_info(mode, &vbe_mode_info) != 0)
    {
        printf("Error in vbe_get_mode_info()");
        return NULL;
    }

    h_res = vbe_mode_info.XResolution;
    v_res = vbe_mode_info.YResolution;
    bits_per_pixel = vbe_mode_info.BitsPerPixel;
    RedFieldPosition = vbe_mode_info.RedFieldPosition;
    RedMaskSize = vbe_mode_info.RedMaskSize;
    GreenFieldPosition = vbe_mode_info.GreenFieldPosition;
    GreenMaskSize = vbe_mode_info.GreenMaskSize;
    BlueFieldPosition = vbe_mode_info.BlueFieldPosition;
    BlueMaskSize = vbe_mode_info.BlueMaskSize;

    // Map the memory
    if (vg_map(vbe_mode_info) != 0)
    {
        printf("Error in vg_map()");
        return NULL;
    }

    // Set the mode
    if (vbe_set_mode(mode) != 0)
    {
        printf("Error in vbe_set_mode()");
        return NULL;
    }

    return video_mem;
}

int(vg_set_pixel)(uint16_t x, uint16_t y, uint32_t color)
{
    if (x >= h_res || y >= v_res)
    {
        printf("Error: x or y is out of bounds");
        return 1;
    }

    if (x < 0 || y < 0)
    {
        printf("Error: x or y is negative");
        return 1;
    }

    uint8_t *pixel = video_mem + (y * h_res + x) * (bits_per_pixel / 8);

    //  Set the pixel
    switch (bits_per_pixel)
    {
    case 8:
        *pixel = color;
        break;
    case 16:
        *(uint16_t *)pixel = color;
        break;
    case 24:
        *(uint32_t *)pixel = color;
        break;
    case 32:
        *(uint32_t *)pixel = color;
        break;
    default:
        printf("Error: bits_per_pixel is not 8, 15, 16, 24 or 32");
        return 1;
    }

    return 0;
}

int(vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color)
{
    for (uint16_t i = x; i < x + len; i++)
    {
        if (vg_set_pixel(i, y, color) != 0)
        {
            printf("Error in vg_set_pixel()");
            return 1;
        }
    }

    return 0;
}

int(vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{

    for (uint16_t i = y; i < y + height; i++)
    {
        if (vg_draw_hline(x, i, width, color) != 0)
        {
            printf("Error in vg_draw_hline()");
            return 1;
        }
    }

    return 0;
}

uint16_t get_h_res()
{
    return h_res;
}

uint16_t get_v_res()
{
    return v_res;
}

uint8_t get_bits_per_pixel()
{
    return bits_per_pixel;
}

uint8_t get_RedMaskSize()
{
    return RedMaskSize;
}

uint8_t get_RedFieldPosition()
{
    return RedFieldPosition;
}

uint8_t get_GreenMaskSize()
{
    return GreenMaskSize;
}

uint8_t get_GreenFieldPosition()
{
    return GreenFieldPosition;
}

uint8_t get_BlueMaskSize()
{
    return BlueMaskSize;
}

uint8_t get_BlueFieldPosition()
{
    return BlueFieldPosition;
}

int(draw_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y)
{

    xpm_image_t img;

    uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);

    if (pixmap == NULL)
    {
        printf("Error loading xpm");
        return 1;
    }

    for (int i = 0; i < img.height; i++)
    {
        for (int j = 0; j < img.width; j++)
        {
            if (*(pixmap + i * img.width + j) != 0)
                vg_set_pixel(x + j, y + i, *(pixmap + i * img.width + j));
        }
    }

    free(pixmap);

    return 0;
}

int(erase_xpm)(xpm_map_t xpm, uint16_t x, uint16_t y)
{

    xpm_image_t img;

    uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);

    if (pixmap == NULL)
    {
        printf("Error loading xpm");
        return 1;
    }

    for (int i = 0; i < img.height; i++)
    {
        for (int j = 0; j < img.width; j++)
        {
            if (*(pixmap + i * img.width + j) != 0)
                vg_set_pixel(x + j, y + i, *(pixmap + i * img.width + j));
        }
    }

    free(pixmap);

    return 0;
}
