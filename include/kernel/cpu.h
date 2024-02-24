#ifndef _KERNEL_CPU_H
#define _KERNEL_CPU_H

#include <stdint.h>

void idt_init(void);
void set_idt_entry(int, uint32_t, int);

#endif
