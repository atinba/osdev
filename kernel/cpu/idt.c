#include <stdint.h>
#include <kernel/cpu.h>

#include "idt.h"

extern uint32_t isr_table[];
uint64_t idt[NUM_IDT_ENTRIES];

/* IDT Entry Structure:
 * 16: low 16 bits of offset in segment
 * 16: code segment selector
 *  5: #args, 0 for interrupt/trap gates
 *  3: reserved(should be zero)
 *  4: type(STS_{IG32,TG32})
 *  1: must be 0 (system)
 *  2: descriptor(meaning new) privilege level (highest priv i.e. 0)
 *  1: Present
 * 16: high bits of offset in segment
 */
void set_idt_entry(int idt_index, uint32_t offset, int is_trap)
{
    uint16_t low_offset = (uint16_t)(offset & 0xffff);
    uint16_t high_offset = (uint16_t)(offset >> 16);
    uint8_t type = (is_trap ? STS_TG32 : STS_IG32) | 1 << 7;
    uint16_t code_segment = 0x8;

    idt[idt_index] = (uint64_t)high_offset << 48 | (uint64_t)type << 40 |
                     (uint64_t)code_segment << 16 | (uint64_t)low_offset;
}

void idt_init(void)
{
    for (int i = 0; i < 256; i++)
        set_idt_entry(i, isr_table[i], 0);

    // Load IDT
    uint32_t idt_addr = (uint32_t)&idt;
    uint64_t idtr = (sizeof(idt) - 1) | (uint64_t)idt_addr << 16;
    asm volatile("lidt (%0)" ::"r"(&idtr));
}
