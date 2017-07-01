
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
void enable_all_irqs();
void disable_all_irqs();

void enable_irq_0();
void disable_irq_0();

void enable_irq_1();
void disable_irq_1();

void enable_irq_2();
void disable_irq_2();

void enable_irq_3();
void disable_irq_3();

void enable_irq_4();
void disable_irq_4();

void enable_irq_5();
void disable_irq_5();

void enable_irq_6();
void disable_irq_6();

void enable_irq_7();
void disable_irq_7();

#endif
