#include <stdint.h>

#define STS_IG32 0xE // 32-bit Interrupt Gate
#define STS_TG32 0xF // 32-bit Trap Gate

void idtinit(void);

uint64_t idt[256];
extern unsigned int vectors[]; // in vectors.S: array of 256 entry pointers

/* IDT Entry Structure:
 * 16: low 16 bits of offset in segment
 * 16: code segment selector
 *  5: # args, 0 for interrupt/trap gates
 *  3: reserved(should be zero)
 *  4: type(STS_{IG32,TG32})
 *  1: must be 0 (system)
 *  2: descriptor(meaning new) privilege level (highest priv i.e. 0)
 *  1: Present
 * 16: high bits of offset in segment
 */
static uint64_t get_idt_entry(uint32_t offset, int is_trap)
{
    uint16_t low_offset = (uint16_t)(offset & 0xffff);
    uint16_t high_offset = (uint16_t)(offset >> 16);
    uint8_t type = (is_trap ? STS_TG32 : STS_IG32) | 1 << 7;
    uint16_t code_segment = 0x8;

    return (uint64_t)high_offset << 48 | (uint64_t)type << 40 |
           (uint64_t)code_segment << 16 | (uint64_t)low_offset;
}

static inline void lidt(uint64_t *p, int size)
{
    volatile unsigned short pd[3];

    pd[0] = size - 1;
    pd[1] = (unsigned int)p;
    pd[2] = (unsigned int)p >> 16;

    asm volatile("lidt (%0)" ::"r"(pd));
}

void idtinit(void)
{
    for (int i = 0; i < 256; i++)
        idt[i] = get_idt_entry(vectors[i], 0);
    lidt(idt, sizeof(idt));
}
