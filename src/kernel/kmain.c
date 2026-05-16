#include "gdt.h"
#include "framebuffer.h"
#include "stdio.h"
#include "idt.h"
#include "timer.h"
#include "isr.h"
#include "multiboot.h"

void kmain(uint32_t magic, multiboot_info_t* mbd) {
    init_gdt();
    init_idt();
    fb_clear();
	init_isr();  
    init_timer(100); // 100 Hz = 100 ticks per second

    if(magic != MULTIBOOT_MAGIC){
        printf("Error: Not loaded by GRUB");
        return;
    }
    printf("Hello, Osiris!\n");

// Check if the detailed Memory Map is present (Bit 6)
    if (mbd->flags & 0x40) {
        printf("Detailed Memory Map Flag is ON!\n");
        
        // TEST: We changed %x to %d here!
        printf("Map Address: %d\n", mbd->mmap_addr);
        printf("Map Length:  %d\n", mbd->mmap_length);
        
        printf("Successfully read Multiboot Struct!\n");
        
    } else {
        printf("No memory map provided by GRUB.\n");
    }
    
    printf("Type below to echo characters:\n\n");

	asm volatile("sti");  // enable interrupts globally

    while (1); // wait for interrupts
}
