#include "timer.h"
#include "io.h"
#include "stdio.h"
#include "idt.h"
#include "framebuffer.h"

uint32_t tick = 0;

void init_timer(uint32_t frequency){

    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0x36);

    uint8_t low_byte = (uint8_t)(divisor & 0xFF);
    uint8_t high_byte = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, low_byte);
    outb(0x40, high_byte);
}

void timer_handler() {
    tick++;
    // Send EOI to the PIC
    outb(0x20, 0x20); 

    uint32_t tot_seconds = tick / 100; // Assuming 100 ticks per second
    uint32_t minutes = tot_seconds / 60;
    uint32_t seconds = tot_seconds % 60;

    char time_str[] = "Time: 00:00";

    time_str[6] = '0' + (minutes / 10);
    time_str[7] = '0' + (minutes % 10);
    time_str[9] = '0' + (seconds / 10);
    time_str[10] = '0' + (seconds % 10);

    int start_index = 69;

    for(int i = 0; time_str[i] != '\0'; i++) {
        fb_write_cell(start_index + i, time_str[i], FB_WHITE, FB_BLUE); 
    }
}