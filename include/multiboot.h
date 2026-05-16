#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

#define MULTIBOOT_MAGIC 0x2BADB002

// This structure defines the layout of the data GRUB gives us
typedef struct {
    uint32_t flags;
    uint32_t mem_lower;   // Amount of RAM starting from 0 (in KB)
    uint32_t mem_upper;   // Amount of RAM starting from 1MB (in KB)
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
    uint32_t mmap_length; // Size of the detailed memory map
    uint32_t mmap_addr;   // Pointer to the detailed memory map
} __attribute__((packed)) multiboot_info_t;

// This represents one entry in the hardware memory map
typedef struct {
    uint32_t size;           // Size of the entry (not including the size field itself)
    uint32_t base_addr_low;  // Low 32 bits of the base address
    uint32_t base_addr_high; // High 32 bits of the base address
    uint32_t length_low;     // Low 32 bits of the length
    uint32_t length_high;    // High 32 bits of the length
    uint32_t type;           // Type of the region (1 = usable)
} __attribute__((packed)) multiboot_memory_map_t;

#endif