; src/boot/isr_handlers.s

[bits 32]

extern isr_handler   ; C-level handler
extern isr_keyboard  ; Keyboard handler

%macro ISR_NOERR 1
global isr%1
isr%1:
    cli
    push 0               ; Dummy error code
    push %1              ; Interrupt number
    jmp isr_common
%endmacro

%macro ISR_ERR 1
global isr%1
isr%1:
    cli
    push %1              ; Interrupt number
    jmp isr_common
%endmacro

; Special handler for keyboard interrupt (IRQ1, INT 0x21)
global isr33
isr33:
    cli
    pusha                ; Save all registers
    call isr_keyboard
    popa
    sti                  ; Re-enable interrupts before returning
    iret

isr_common:
    pusha                ; Save all general-purpose registers
    mov ax, ds
    push eax
    mov ax, 0x10         ; Kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call isr_handler
    pop eax              ; Restore original data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    popa
    add esp, 8           ; Clean up error code and int num
    sti                  ; Re-enable interrupts
    iret

; --- ISRs 0–31 (exceptions) ---
ISR_ERR 0   ; Division by zero
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_ERR  7  ; Device not available
ISR_ERR  8  ; Double fault
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31
