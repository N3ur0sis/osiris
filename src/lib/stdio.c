#include "framebuffer.h"
#include "stdarg.h"
#include "stdint.h"

static unsigned int fb_cursor = 0;
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

#define FB_ADDRESS 0x000B8000
static char *const fb = (char *)FB_ADDRESS;

// Enhanced character output handling with line wrapping, newline, and backspace
static void print_char(char c) {
    if (c == '\n') {
        // Handle newline
        fb_cursor += SCREEN_WIDTH - (fb_cursor % SCREEN_WIDTH);
    } 
    else if (c == '\b') {
        // Handle backspace - only if we're not at the beginning
        if (fb_cursor > 0) {
            fb_cursor--;
            fb_write_cell(fb_cursor, ' ', FB_WHITE, FB_BLACK);
        }
    } 
    else {
        // Regular character 
        fb_write_cell(fb_cursor, c, FB_WHITE, FB_BLACK);
        fb_cursor++;
    }

    // Scroll up if needed
    if (fb_cursor >= SCREEN_WIDTH * SCREEN_HEIGHT) {
        for (int i = 0; i < (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i++) {
            fb_write_cell(i, fb[i * 2 + SCREEN_WIDTH * 2], fb[i * 2 + 1], fb[i * 2 + SCREEN_WIDTH * 2 + 1]);
        }
        for (int i = (SCREEN_HEIGHT - 1) * SCREEN_WIDTH; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
            fb_write_cell(i, ' ', FB_WHITE, FB_BLACK);
        }
        fb_cursor -= SCREEN_WIDTH;
    }

    fb_move_cursor(fb_cursor);
}

static void print_dec(int value) {
    char buffer[12];
    int i = 10;
    buffer[11] = '\0';
    int is_negative = (value < 0);
    if (is_negative) value = -value;
    do {
        buffer[i--] = '0' + (value % 10);
        value /= 10;
    } while (value > 0);
    if (is_negative) buffer[i--] = '-';
    for (int j = i + 1; buffer[j]; j++) print_char(buffer[j]);
}

static void print_hex(uint32_t value) {
    char hex[] = "0123456789ABCDEF";
    print_char('0');
    print_char('x');
    for (int i = 28; i >= 0; i -= 4)
        print_char(hex[(value >> i) & 0xF]);
}

int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p; ++p) {
        if (*p == '%') {
            switch (*++p) {
                case 'd': print_dec(va_arg(args, int)); break;
                case 'x': print_hex(va_arg(args, uint32_t)); break;
                case 'c': print_char((char)va_arg(args, int)); break;
                case 's': {
                    const char *s = va_arg(args, const char *);
                    while (*s) print_char(*s++);
                    break;
                }
                case '%': print_char('%'); break;
                default: print_char('%'); print_char(*p); break;
            }
        } else {
            print_char(*p);
        }
    }

    va_end(args);
    return 0;
}