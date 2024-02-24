#ifndef _CPU_IDT_H
#define _CPU_IDT_H

#include <stdint.h>

// https://wiki.osdev.org/Global_Descriptor_Table
#define GDT_CODE        0x00CF9A000000FFFF
#define GDT_DATA        0x00CF92000000FFFF
#define NUM_IDT_ENTRIES 256
#define STS_IG32        0xE // 32-bit Interrupt Gate
#define STS_TG32        0xF // 32-bit Trap Gate

#endif
