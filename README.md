# Osiris

> "OS In Real-mode, Interactive Simply"

Osiris is a toy x86 operating system written from scratch, following [The Little Book About OS Development](https://littleosbook.github.io/), using QEMU for emulation.

## Build & Run

```bash
./scripts/build.sh
./scripts/run.sh
```
 Technical Documentation

## Table of Contents
1. Introduction
2. System Architecture
3. Boot Process
4. Memory Management
5. Interrupt Handling
6. Input/Output Systems
7. Code Walkthrough
8. Building and Running Osiris
9. Extending the System
10. Theoretical Background
11. Glossary

## Introduction

Osiris is a minimalist x86 operating system designed for educational purposes. It implements fundamental OS concepts including segmentation, interrupt handling, and basic I/O operations. This documentation explains how these components work and interact with each other.

### Design Philosophy

Osiris follows these principles:
- **Simplicity**: Focus on core OS concepts without unnecessary complexity
- **Educational**: Clear code structure to demonstrate OS fundamentals
- **Barebones**: Minimal implementation with no user-space applications

## System Architecture

Osiris consists of several core components:

1. **Boot Loader**: Uses GRUB to load the kernel into memory
2. **Kernel**: The core OS component that initializes hardware and manages resources
3. **Memory Management**: Implements segmentation through the GDT
4. **Interrupt System**: Handles hardware and software interrupts via the IDT
5. **I/O System**: Manages keyboard input and screen output

### Directory Structure

```
osiris/
├── src/
│   ├── boot/         # Boot loader files
│   │   └── loader.s  # Assembly entry point
│   ├── kernel/       # Core kernel files
│   │   ├── kmain.c   # Kernel main function
│   │   ├── gdt.c     # Global Descriptor Table
│   │   ├── idt.c     # Interrupt Descriptor Table
│   │   ├── isr.c     # Interrupt Service Routines
│   │   └── io.c      # I/O port operations
│   └── drivers/
│       ├── framebuffer.c  # Screen output
│       └── keyboard.c     # Keyboard input
├── include/          # Header files
└── Makefile          # Build system
```

## Boot Process

### 1. BIOS to GRUB

When the computer powers on:
1. BIOS performs hardware initialization
2. BIOS loads GRUB from the boot device
3. GRUB loads the Osiris kernel into memory

### 2. Kernel Entry

The boot process continues with `loader.s`:

```assembly
global loader           ; Make entry point visible to linker
extern kmain            ; kmain is defined in kmain.c

KERNEL_STACK_SIZE equ 4096  ; Stack size

section .text
loader:
    mov esp, kernel_stack + KERNEL_STACK_SIZE  ; Set up stack
    call kmain           ; Call C entry point
    jmp $                ; Infinite loop if kmain returns

section .bss
align 4                  ; Align at 4 bytes
kernel_stack:            ; Label points to beginning of memory
    resb KERNEL_STACK_SIZE  ; Reserve stack
```

This assembly code:
- Sets up a stack for the kernel
- Jumps to the `kmain()` C function
- Provides a fallback infinite loop if `kmain()` ever returns

### 3. Kernel Initialization

The `kmain()` function initializes core OS components:

```c
void kmain() {
    init_gdt();     // Initialize Global Descriptor Table
    init_idt();     // Initialize Interrupt Descriptor Table
    fb_clear();     // Clear the framebuffer (screen)
    init_isr();     // Initialize Interrupt Service Routines
    
    printf("Hello, Osiris!\n");
    printf("Type below to echo characters:\n\n");
    
    asm volatile("sti");  // Enable interrupts globally
    
    while (1);      // Wait for interrupts (infinite loop)
}
```

## Memory Management

### Global Descriptor Table (GDT)

The GDT defines memory segments for the CPU. Osiris uses a flat memory model where code and data segments span the entire address space:

```c
// GDT entry structure
struct gdt_entry {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t  base_middle;
    uint8_t  access;
    uint8_t  granularity;
    uint8_t  base_high;
} __attribute__((packed));

// Setup function
void init_gdt() {
    // Set up GDT pointer
    gdt_ptr.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;
    
    // Null segment
    gdt_set_entry(0, 0, 0, 0, 0);
    // Code segment (0x9A: Present, Ring 0, Code, Readable)
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    // Data segment (0x92: Present, Ring 0, Data, Writable)
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    
    // Load GDT
    gdt_flush();
}
```

The GDT contains three segments:
- **Null segment**: Required by the CPU
- **Code segment**: Where executable code resides
- **Data segment**: Where data is stored

## Interrupt Handling

### Interrupt Descriptor Table (IDT)

The IDT maps interrupt vectors to handler functions:

```c
// IDT entry structure
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_hi;
} __attribute__((packed));

// Setup function
void init_idt() {
    // Set up IDT pointer
    idtp.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    idtp.base = (uint32_t)&idt;
    
    // Clear IDT
    memset(&idt, 0, sizeof(struct idt_entry) * IDT_ENTRIES);
    
    // Set up exception handlers (0-31)
    // Set up IRQ handlers (32-47)
    
    // Load IDT
    idt_flush();
}
```

### Interrupt Service Routines (ISRs)

ISRs handle CPU exceptions and hardware interrupts:

```c
void init_isr() {
    // Remap PIC interrupts
    // PIC1: master, handles IRQs 0-7
    outb(0x20, 0x11); // Initialize PIC1 in cascade mode
    outb(0xA0, 0x11); // Initialize PIC2 in cascade mode
    outb(0x21, 0x20); // Map PIC1 to interrupt vectors 32-39
    outb(0xA1, 0x28); // Map PIC2 to interrupt vectors 40-47
    outb(0x21, 0x04); // Tell PIC1 about PIC2 at IRQ2
    outb(0xA1, 0x02); // Tell PIC2 its cascade identity
    outb(0x21, 0x01); // Set PIC1 to 8086 mode
    outb(0xA1, 0x01); // Set PIC2 to 8086 mode
    
    // Install exception handlers (vectors 0-31)
    idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);  // Divide by zero
    // ... more exception handlers ...
    
    // Install IRQ handlers (vectors 32-47)
    idt_set_gate(32, (uint32_t)isr32, 0x08, 0x8E); // Timer
    idt_set_gate(33, (uint32_t)isr33, 0x08, 0x8E); // Keyboard
    // ... more IRQ handlers ...
    
    // Unmask keyboard IRQ (IRQ1)
    outb(PIC1_DATA, 0xFD);  // 0xFD = 11111101 in binary (bit 1 cleared)
}
```

### Handling Keyboard Interrupts

When a key is pressed:
1. Keyboard controller generates IRQ1
2. CPU vectors to interrupt handler via IDT
3. Handler reads scancode from keyboard port
4. Scancode is translated to ASCII
5. Character is displayed on screen

```c
void keyboard_handler() {
    unsigned char scancode = inb(KEYBOARD_DATA_PORT);
    
    // Process scancode and convert to ASCII
    char c = scancode_to_ascii(scancode);
    
    // Print character to screen
    if (c) {
        printf("%c", c);
    }
    
    // Send EOI (End of Interrupt) to PIC
    outb(PIC1_COMMAND, PIC_EOI);
}
```

## Input/Output Systems

### Screen Output

Osiris writes directly to the VGA text-mode framebuffer at physical address 0xB8000:

```c
#define FB_ADDRESS 0x000B8000
#define FB_COLS 80
#define FB_ROWS 25

static volatile char *fb = (char *) FB_ADDRESS;

// Write a character to a specific position with color attributes
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i * 2] = c;                        // Character byte
    fb[i * 2 + 1] = ((bg & 0x0F) << 4) | (fg & 0x0F);  // Attribute byte
}

// Clear the screen
void fb_clear() {
    unsigned int i;
    for (i = 0; i < FB_COLS * FB_ROWS; i++) {
        fb_write_cell(i, ' ', FB_COLOR_LIGHT_GREY, FB_COLOR_BLACK);
    }
    fb_move_cursor(0);
}
```

The printf function provides formatted output:

```c
int printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    int ret = vprintf(format, args);
    va_end(args);
    return ret;
}
```

### Keyboard Input

The keyboard driver handles IRQ1 and translates scancodes to ASCII:

```c
// Keyboard ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Handle keyboard interrupt
void keyboard_handler() {
    unsigned char status = inb(KEYBOARD_STATUS_PORT);
    
    if (status & 0x01) {  // Data available
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        char c = scancode_to_ascii(scancode);
        
        // Handle the character
        if (c) {
            printf("%c", c);
        }
    }
    
    // Send EOI to PIC
    outb(PIC1_COMMAND, PIC_EOI);
}
```

## Code Walkthrough

### Kernel Main Function

```c
void kmain() {
    // Initialize memory segmentation
    init_gdt();
    
    // Initialize interrupt handling
    init_idt();
    
    // Clear the screen
    fb_clear();
    
    // Set up interrupt service routines
    init_isr();
    
    // Display welcome message
    printf("Hello, Osiris!\n");
    printf("Type below to echo characters:\n\n");
    
    // Enable interrupts globally
    asm volatile("sti");
    
    // Main kernel loop - wait for interrupts
    while (1);
}
```

This function:
1. Initializes core hardware components
2. Sets up memory protection
3. Configures interrupt handling
4. Enables device drivers
5. Enables interrupts
6. Enters an infinite loop, allowing interrupt handlers to process events

### Component Interactions

1. **Boot Process → Kernel Initialization**:
   - GRUB loads the kernel and jumps to `loader`
   - `loader` sets up the stack and calls `kmain`
   - `kmain` initializes all subsystems

2. **Keyboard → Screen**:
   - Key press generates hardware interrupt (IRQ1)
   - CPU vectors to keyboard ISR via IDT
   - ISR reads scancode and converts to character
   - Character is displayed via `printf`

3. **GDT → Memory Protection**:
   - GDT defines code and data segments
   - CPU uses these segments to enforce memory access rules

4. **IDT → Interrupt Handling**:
   - IDT maps interrupt vectors to handler functions
   - When an interrupt occurs, CPU looks up the handler in the IDT
   - CPU transfers control to the handler

## Building and Running Osiris

### Prerequisites
- GCC cross-compiler for i686-elf
- NASM assembler
- GRUB bootloader
- QEMU for testing (or physical hardware)

### Build Process
```bash
# Clone the repository
git clone https://github.com/username/osiris.git
cd osiris

# Build the kernel
make

# Create bootable ISO
make iso

# Run in QEMU
make run
```

## Extending the System

Osiris can be extended with additional features:

### Adding a New Hardware Driver
1. Create driver source file in drivers
2. Add initialization to `kmain()`
3. Set up interrupt handler if needed

### Implementing a Simple Shell
1. Extend keyboard handler to buffer input
2. Add command parsing logic
3. Implement command handlers

## Theoretical Background

### Protected Mode

Osiris runs in x86 32-bit protected mode, which provides:
- Memory protection through segmentation
- Hardware-assisted privilege levels (rings)
- Extended memory addressing beyond 1MB

### Memory Segmentation

The x86 architecture uses segmentation to divide the address space:
- Each segment has a base address and limit
- CPU adds the segment base to offset to form physical address
- GDT entries define segment properties

### Interrupt Architecture

Interrupts allow asynchronous event handling:
- **Hardware interrupts**: Generated by devices (IRQs)
- **Software interrupts**: Generated by the `int` instruction
- **Exceptions**: Generated by the CPU on errors

The Programmable Interrupt Controller (PIC) manages hardware interrupts:
- Receives interrupt signals from hardware
- Prioritizes multiple interrupts
- Signals the CPU via the INTR pin

### Input/Output

Osiris uses two I/O methods:
- **Port I/O**: Using `inb`/`outb` instructions to communicate with hardware
- **Memory-mapped I/O**: Accessing device registers through normal memory operations

## Glossary

- **BIOS**: Basic Input/Output System - firmware that initializes hardware
- **GRUB**: GRand Unified Bootloader - loads the kernel into memory
- **GDT**: Global Descriptor Table - defines memory segments
- **IDT**: Interrupt Descriptor Table - maps interrupt vectors to handlers
- **ISR**: Interrupt Service Routine - handles interrupts
- **PIC**: Programmable Interrupt Controller - manages hardware interrupts
- **IRQ**: Interrupt ReQuest - hardware interrupt signal
- **Port I/O**: Method to communicate with hardware using special CPU instructions
- **Memory-mapped I/O**: Method to access hardware by reading/writing memory
- **VGA**: Video Graphics Array - standard display hardware

This documentation provides a comprehensive overview of the Osiris mini OS. While minimalist in design, it demonstrates the fundamental concepts that form the foundation of more complex operating systems.