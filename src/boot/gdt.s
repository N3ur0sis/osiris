global gdt_flush

[bits 32]
[extern gdt]
[extern gdt_descriptor]
global load_gdt

load_gdt:
    mov eax, gdt_descriptor   ; <-- you are using this symbol
    lgdt [eax]
    ret

gdt_flush:
    lgdt [gdt_descriptor]       ; Load GDT pointer

    mov ax, 0x10                ; Data segment selector (offset 2 * 8)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush_cs          ; Code segment selector (offset 1 * 8)
.flush_cs:
    ret
