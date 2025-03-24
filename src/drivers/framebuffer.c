// framebuffer.c
#include "io.h"
#include "framebuffer.h"

#define FB_ADDRESS 0x000B8000
char *fb = (char *) FB_ADDRESS;

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i * 2]     = c;
    fb[i * 2 + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

#define FB_COMMAND_PORT 0x3D4
#define FB_DATA_PORT    0x3D5
#define FB_HIGH_BYTE_COMMAND 14
#define FB_LOW_BYTE_COMMAND  15

void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, (pos >> 8) & 0xFF);
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0xFF);
}

int fb_write(char *buf, unsigned int len) {
    unsigned int i;
    for (i = 0; i < len; i++) {
        fb_write_cell(i, buf[i], 15, 0); // White on black
    }
    fb_move_cursor(len);
    return (int)len;
}
