#ifndef _CPU_IDT_H
#define _CPU_IDT_H

#include <kernel/types.h>
#include <kernel/cpu.h>

#define NUM_IDT_ENTRIES 256
#define STS_IG32        0xE // 32-bit Interrupt Gate
#define STS_TG32        0xF // 32-bit Trap Gate

void set_idt_entry(int, u32, bool);

#endif
