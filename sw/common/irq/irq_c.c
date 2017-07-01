#include "irq_c.h"

void enable_all_irqs()
{
    asm volatile ("csrs mstatus, 0x1");
}

void disable_all_irqs()
{
    asm volatile ("csrw mstatus, 0x0");
}
//===============================================
void enable_irq_0()
{
    asm volatile ("li t0, 0x400\ncsrs mie, t0");
}
void disable_irq_0()
{
    asm volatile ("li t0, 0x400\ncsrc mie, t0");
}
//===============================================
void enable_irq_1()
{
    asm volatile ("li t0, 0x800\ncsrs mie, t0");
}
void disable_irq_1()
{
    asm volatile ("li t0, 0x800\ncsrc mie, t0");
}
//===============================================
void enable_irq_2()
{
    asm volatile ("li t0, 0x1000\ncsrs mie, t0");
}
void disable_irq_2()
{
    asm volatile ("li t0, 0x1000\ncsrc mie, t0");
}
//===============================================
void enable_irq_3()
{
    asm volatile ("li t0, 0x2000\ncsrs mie, t0");
}
void disable_irq_3()
{
    asm volatile ("li t0, 0x2000\ncsrc mie, t0");
}
//===============================================
void enable_irq_4()
{
    asm volatile ("li t0, 0x4000\ncsrs mie, t0");
}
void disable_irq_4()
{
    asm volatile ("li t0, 0x4000\ncsrc mie, t0");
}
//===============================================
void enable_irq_5()
{
    asm volatile ("li t0, 0x8000\ncsrs mie, t0");
}
void disable_irq_5()
{
    asm volatile ("li t0, 0x8000\ncsrc mie, t0");
}
//===============================================
void enable_irq_6()
{
    asm volatile ("li t0, 0x10000\ncsrs mie, t0");
}
void disable_irq_6()
{
    asm volatile ("li t0, 0x10000\ncsrc mie, t0");
}
//===============================================
void enable_irq_7()
{
    asm volatile ("li t0, 0x20000\ncsrs mie, t0");
}
void disable_irq_7()
{
    asm volatile ("li t0, 0x20000\ncsrc mie, t0");
}