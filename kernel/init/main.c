#include "../include/vbe.h"
#include "../include/stdint.h"

void kernel_main() {
    VbeModeInfo_t *mode_info = (VbeModeInfo_t*)VBE_MODE_INFO_BASE;
    
    for (int y = 0; y < mode_info->y_resolution; y++) {
        for (int x = 0; x < mode_info->x_resolution; x++) {
            mode_info->frame_buffer[y * mode_info->x_resolution + x] = 0x00ff0000;
        }
    }

    while(1);
}