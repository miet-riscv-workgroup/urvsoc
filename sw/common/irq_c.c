#include "irq_c.h"

void irq_enable_all()
{
    asm volatile ("csrs mstatus, 0x1");
}

void irq_disable_all()
{
    asm volatile ("csrw mstatus, 0x0");
}

void irq_set_mask (uint32_t mask)
{
    uint32_t mask_shift = mask << 10;
    __asm__ (
      "csrr t0, mie\n\t"
      "lw   t0, %1\n\t"
      //"nop \n\t"
      "csrw mie, t0\n\t"
      : "=m" (mask_shift)
      : "m" (mask_shift)
   );
}
