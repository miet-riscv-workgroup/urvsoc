
#ifndef __IRQ_C_H
#define __IRQ_C_H

#include <stdint.h>

#define IRQ_0 10
#define IRQ_1 11
#define IRQ_2 12
#define IRQ_3 13
#define IRQ_4 14
#define IRQ_5 15
#define IRQ_6 16
#define IRQ_7 17

/* function declaration*/
void irq_enable_all();
void irq_disable_all();

void irq_set_mask(uint32_t mask);


#endif
