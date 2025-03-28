#include "io.h"
#include "framebuffer.h"
#include "stdio.h"
#include "keyboard.h"

#define KEYBOARD_DATA_PORT 0x60
#define PIC1_COMMAND 0x20
#define PIC_EOI 0x20

// Special key codes
#define KEY_BACKSPACE 0x0E
#define KEY_ENTER 0x1C
#define KEY_LEFT_SHIFT 0x2A
#define KEY_RIGHT_SHIFT 0x36
#define KEY_CAPS_LOCK 0x3A

// Scancode to ASCII conversion tables for US keyboard layout
static const char kbd_US_lowercase[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Shifted keys
static const char kbd_US_uppercase[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Track shift and caps lock state
static int shift_pressed = 0;
static int caps_lock_on = 0;

static void handle_backspace() {
    printf("\b \b");
}

static void handle_enter() {
    printf("\n");
}

// Main keyboard interrupt handler
void isr_keyboard() {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    
    // Key press event
    if (scancode < 0x80) {
        // Handle special keys
        if (scancode == KEY_LEFT_SHIFT || scancode == KEY_RIGHT_SHIFT) {
            shift_pressed = 1;
        } 
        else if (scancode == KEY_CAPS_LOCK) {
            caps_lock_on = !caps_lock_on;
        }
        else if (scancode == KEY_BACKSPACE) {
            handle_backspace();
        }
        else if (scancode == KEY_ENTER) {
            handle_enter();
        }
        else {
            // Determine which character table to use based on shift/caps state
            int uppercase = shift_pressed ^ caps_lock_on;
            char c = uppercase ? kbd_US_uppercase[scancode] : kbd_US_lowercase[scancode];
            
            if (c != 0) {
                printf("%c", c);
            }
        }
    }
    // Key release event (bit 7 set)
    else {
        scancode &= 0x7F;  // Clear bit 7 to get the actual key code
        if (scancode == KEY_LEFT_SHIFT || scancode == KEY_RIGHT_SHIFT) {
            shift_pressed = 0;
        }
    }
    
    // Send End-of-Interrupt signal to PIC
    outb(PIC1_COMMAND, PIC_EOI);
}

// Keep this for backward compatibility
void keyboard_handler() {
    isr_keyboard();
}