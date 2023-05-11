#include <lcom/lcf.h>


static uint8_t global_mode;
static uint8_t x_res;
static uint8_t y_res;
static uint8_t bits_per_pixel;
static uint8_t *video_mem;

static uint8_t r_sz;
static uint8_t r_pos;
static uint8_t g_sz;
static uint8_t g_pos;
static uint8_t b_sz;
static uint8_t b_pos;

uint32_t normalize_color(uint32_t color){
    
    if (global_mode == 0 || global_mode == 1){
        return color;
    }
    
    // Enforce Transparency to Zero
    if(global_mode == 2){
        color &= 0x00FFFFFF;
    }

    uint32_t final_color = 0x00;

    // Ensure 5:6:5 format
    if(global_mode == 3){    
        uint8_t raw_red = (value >> 11) & 0x1F;

        // Extract the 6-bit green component
        uint8_t raw_green = (value >> 5) & 0x3F;

        // Extract the 5-bit blue component
        uint8_t raw_blue = value & 0x1F;
    }

    raw_blue = raw_blue << b_pos;
    raw_green = raw_green << g_pos;
    raw_red = raw_red << r_pos;

    return raw_blue | raw_green | raw_red;
}

int draw_pixel(uint16_t x, uint16_t y, uint32_t color){
    
    if (x >= h_res || y >= v_res){
        return 1;
    }
    
    uint8_t *ptr = (uint8_t *) (video_mem + (x + y * h_res) * (bits_per_pixel + 7) / 8);
    
    for (int i = 0; i < (bits_per_pixel + 7) / 8; i++)
    {
        *ptr = (uint8_t) (color >> (i * 8));
        ptr++;
    }
    
    return 0;
}

int draw_line(enum lpv_type_t dir, uint16_t x, uint16_t y, uint16_t len, uint32_t color){
    if (dir.lpv_hor){
        for (int i = 0; i < len; i++)
        {
            vg_draw_pixel(x+i, y, color);
        }
    }else{
        for(int i = 0; i < len; i++){
            vg_draw_pixel(x, y+i, color);
        }
    }
    return 0;
}

int pp_test_line(uint8_t mode, enum lpv_type_t dir, uint16_t x, uint16_t y, uint16_t len, uint32_t color, uint32_t delay){
    global_mode = mode;
    
    lpv_mode_info_t lmi_p;

    lpv_get_mode_inf(mode, &lmi_p);



    draw_line(dir, x, y, len, color);
        
    return 0;    
}

int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}
