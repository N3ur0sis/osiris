global idt_flush

idt_flush:
    mov eax, [esp + 4]   ; Address of the IDT pointer (lidt operand)
    lidt [eax]           ; Load IDT
    ret