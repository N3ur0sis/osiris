#include "io.h"
#include "framebuffer.h"

#define FB_ADDRESS            0x000B8000
#define FB_WIDTH              80
#define FB_HEIGHT             25
#define FB_SIZE               (FB_WIDTH * FB_HEIGHT)

#define FB_COMMAND_PORT       0x3D4
#define FB_DATA_PORT          0x3D5
#define FB_HIGH_BYTE_COMMAND  14
#define FB_LOW_BYTE_COMMAND   15

static volatile char *fb = (char *) FB_ADDRESS;
static unsigned int cursor_pos = 0;

// Low-level write to framebuffer
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i * 2]     = c;
    fb[i * 2 + 1] = (bg << 4) | (fg & 0x0F);  // FG is low bits, BG is high bits
}

// Move cursor to position in framebuffer
void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
    outb(FB_DATA_PORT, (pos >> 8) & 0xFF);
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
    outb(FB_DATA_PORT, pos & 0xFF);
}

// Write a buffer to the screen
int fb_write(const char *buf, unsigned int len) {
    for (unsigned int i = 0; i < len; i++) {
        if (cursor_pos >= FB_SIZE) break;
        fb_write_cell(cursor_pos++, buf[i], FB_WHITE, FB_BLACK);
    }
    fb_move_cursor(cursor_pos);
    return (int)len;
}

// Clear the screen
void fb_clear() {
    for (unsigned int i = 0; i < FB_SIZE; i++) {
        fb_write_cell(i, ' ', FB_WHITE, FB_BLACK);
    }
    cursor_pos = 0;
    fb_move_cursor(0);
}
