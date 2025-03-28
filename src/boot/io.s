global inb
global outb

; outb - send a byte to an I/O port
; void outb(uint16_t port, uint8_t value)
outb:
    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al
    ret

; inb - read a byte from an I/O port
; uint8_t inb(uint16_t port)
inb:
    mov dx, [esp + 4]
    in al, dx
    movzx eax, al
    ret