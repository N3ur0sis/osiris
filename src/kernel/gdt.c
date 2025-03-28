#include "gdt.h"
#include <stddef.h>

struct gdt_entry gdt_entries[3];
struct gdt_ptr gdt_descriptor;

extern void gdt_flush();  // from gdt.s

static void gdt_set_entry(int i, u32 base, u32 limit, u8 access, u8 gran) {
    gdt_entries[i].base_low    = base & 0xFFFF;
    gdt_entries[i].base_middle = (base >> 16) & 0xFF;
    gdt_entries[i].base_high   = (base >> 24) & 0xFF;

    gdt_entries[i].limit_low   = limit & 0xFFFF;
    gdt_entries[i].granularity = (limit >> 16) & 0x0F;

    gdt_entries[i].granularity |= gran & 0xF0;
    gdt_entries[i].access      = access;
}

void init_gdt() {
    gdt_descriptor.limit = sizeof(gdt_entries) - 1;
    gdt_descriptor.base  = (u32)&gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0);                // Null segment
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment

    gdt_flush();  // Defined in Assembly
}
