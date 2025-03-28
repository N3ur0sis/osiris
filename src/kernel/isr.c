// File: src/interrupts/isr.c
#include "idt.h"
#include "keyboard.h"
#include "io.h"
#include "framebuffer.h"
#include "stdio.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

// External assembly handler
extern void isr33(); // Our keyboard interrupt handler

// Initialize PIC and unmask keyboard interrupt
void init_isr() {
    // Map IRQs properly
    // Initialize PIC1 and PIC2
    outb(0x20, 0x11); // Initialize command
    outb(0xA0, 0x11); 
    outb(0x21, 0x20); // PIC1 offset (IRQ0 -> INT32)
    outb(0xA1, 0x28); // PIC2 offset (IRQ8 -> INT40)
    outb(0x21, 0x04); // Tell PIC1 about PIC2 at IRQ2
    outb(0xA1, 0x02); // Tell PIC2 its cascade identity
    outb(0x21, 0x01); // 8086 mode
    outb(0xA1, 0x01); // 8086 mode
    
    // Register keyboard handler for IRQ1 (INT33)
    idt_set_gate(0x21, (uint32_t)isr33, 0x08, 0x8E);
    
    // Unmask only keyboard IRQ (IRQ1)
    outb(PIC1_DATA, 0xFD); // 0xFD = 11111101, bit 1 = 0 (unmask IRQ1)
    outb(PIC2_DATA, 0xFF); // Mask all interrupts from PIC2
}

void isr_handler(int interrupt_number) {
    printf("Received interrupt: %d\n", interrupt_number);
    
    // Send End-of-Interrupt signal to PIC
    if (interrupt_number >= 32) {
        outb(PIC1_COMMAND, PIC_EOI);
        if (interrupt_number >= 40) {
            outb(PIC2_COMMAND, PIC_EOI);
        }
    }
}
