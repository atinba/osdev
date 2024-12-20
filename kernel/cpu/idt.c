#include "idt.h"

extern u32 isr_table[];
u64 idt[NUM_IDT_ENTRIES];

extern void keyboard_handler(void);

/* IDT Entry Structure:
 * 16: low 16 bits of offset in segment
 * 16: code segment selector
 *  5: #args, 0 for interrupt/trap gates
 *  3::reserved(should be zero)
 *  4: type(STS_{IG32,TG32})
 *  1: must be 0 (system)
 *  2: descriptor(meaning new) privilege level (highest priv i.e. 0)
 *  1: Present
 * 16: high bits of offset in segment
 */
void set_idt_entry(int idt_index, u32 offset, bool is_trap)
{
    u16 low_offset = (u16)(offset & 0xffff);
    u16 high_offset = (u16)(offset >> 16);
    u8 type = (is_trap ? STS_TG32 : STS_IG32) | 1 << 7;
    u16 code_segment = 0x8;

    idt[idt_index] = (u64)high_offset << 48 | (u64)type << 40 |
                     (u64)code_segment << 16 | (u64)low_offset;
}

void idt_init(void)
{
    for (int i = 0; i < 256; i++)
        set_idt_entry(i, isr_table[i], 0);

    unsigned int kb_handler_offset = (unsigned long)keyboard_handler;
    set_idt_entry(33, kb_handler_offset, 0);

    // Load IDT
    u32 idt_addr = (u32)&idt;
    u64 idtr = (sizeof(idt) - 1) | (u64)idt_addr << 16;
    asm volatile("lidt (%0)" ::"r"(&idtr));
}
