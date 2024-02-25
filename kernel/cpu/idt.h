#ifndef _CPU_IDT_H
#define _CPU_IDT_H

#include <stdint.h>

#define NUM_IDT_ENTRIES 256
#define STS_IG32        0xE // 32-bit Interrupt Gate
#define STS_TG32        0xF // 32-bit Trap Gate

#endif
