#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Called from low-level ASM dispatcher
void isr_handler(int interrupt_number);

// Initialization routine for ISR handlers
void init_isr();

#endif // ISR_H