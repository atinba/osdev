#ifndef _KERNEL_CPU_H
#define _KERNEL_CPU_H

#include <kernel/types.h>

void idt_init(void);
void set_idt_entry(int, u32, int);

#endif
