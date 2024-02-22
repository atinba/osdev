#ifndef _KERNEL_CPU_H
#define _KERNEL_CPU_H

#include <stdint.h>

// https://wiki.osdev.org/Global_Descriptor_Table
#define GDT_CODE        0x00CF9A000000FFFF
#define GDT_DATA        0x00CF92000000FFFF
#define NUM_IDT_ENTRIES 256
#define STS_IG32        0xE // 32-bit Interrupt Gate
#define STS_TG32        0xF // 32-bit Trap Gate

void idt_init(void);
void set_idt_entry(int, uint32_t, int);

#endif
