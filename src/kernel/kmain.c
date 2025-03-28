#include "gdt.h"
#include "framebuffer.h"
#include "stdio.h"
#include "idt.h"
#include "isr.h"

void kmain() {
    init_gdt();
    init_idt();
    fb_clear();
	init_isr();  
    printf("Hello, Osiris!\n");
    printf("Type below to echo characters:\n\n");

	asm volatile("sti");  // enable interrupts globally

    while (1); // wait for interrupts
}
