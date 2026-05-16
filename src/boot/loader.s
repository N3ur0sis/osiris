global loader
extern kmain                    ; tells NASM that kmain is defined elsewhere (in C)

MAGIC_NUMBER equ 0x1BADB002
FLAGS        equ 0x03
CHECKSUM     equ -(MAGIC_NUMBER + FLAGS)

KERNEL_STACK_SIZE equ 4096     ; 4 KB stack size

section .text
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

loader:
    ; Set up the stack
    mov esp, kernel_stack + KERNEL_STACK_SIZE

    push ebx ; multiboot info pointer
    push eax


    ; Call our C function
    call kmain

.hang:
    jmp .hang                   ; stay here forever

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE
