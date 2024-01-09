#include <stdio.h>
#include <stdint.h>

#define STS_IG32 0xE // 32-bit Interrupt Gate
#define STS_TG32 0xF // 32-bit Trap Gate

uint64_t get_idt_entry(uint32_t offset, int is_trap) {
    uint16_t low_offset = offset & 0xffff;
    uint16_t high_offset = (offset >> 16) & 0xffff;
    uint8_t zero = 0;
    uint8_t type = (is_trap ? STS_TG32 : STS_IG32) | 1 << 7;
    uint16_t code_segment = 0x8;

    return (uint64_t)high_offset << 48 |
     (uint64_t)type << 40 |
    (uint64_t)code_segment << 16 | 
    (uint64_t)low_offset;

}

printuint(uint64_t u) {
    int low = u & 0xffffffff;
    int high = u >> 32;
    printf("%d %d\n", high, low);
}

// Set up a normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears FL_IF, trap gate leaves FL_IF alone
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction.
#define SETGATE(gate, istrap, sel, off, d)                  \
    {                                                       \
        (gate).off_15_0 = (unsigned int)(off)&0xffff; \
        (gate).cs = (sel);                                  \
        (gate).args = 0;                                    \
        (gate).rsv1 = 0;                                    \
        (gate).type = (istrap) ? STS_TG32 : STS_IG32;       \
        (gate).s = 0;                                       \
        (gate).dpl = (d);                                   \
        (gate).p = 1;                                       \
        (gate).off_31_16 = (unsigned int)(off) >> 16; \
    }

// Gate descriptors for interrupts and traps
struct gatedesc {
    unsigned int off_15_0  : 16; // low 16 bits of offset in segment
    unsigned int cs        : 16; // code segment selector
    unsigned int args      : 5;  // # args, 0 for interrupt/trap gates
    unsigned int rsv1      : 3;  // reserved(should be zero I guess)
    unsigned int type      : 4;  // type(STS_{IG32,TG32})
    unsigned int s         : 1;  // must be 0 (system)
    unsigned int dpl       : 2;  // descriptor(meaning new) privilege level
    unsigned int p         : 1;  // Present
    unsigned int off_31_16 : 16; // high bits of offset in segment
};

void pgui(uint64_t value) {
    struct gatedesc gate;
    gate.off_15_0  = (unsigned int)(value & 0xFFFF);
    gate.cs        = (unsigned int)((value >> 16) & 0xFFFF);
    gate.args      = (unsigned int)((value >> 32) & 0x1F);
    gate.rsv1      = (unsigned int)((value >> 37) & 0x7);
    gate.type      = (unsigned int)((value >> 40) & 0xF);
    gate.s         = (unsigned int)((value >> 44) & 0x1);
    gate.dpl       = (unsigned int)((value >> 45) & 0x3);
    gate.p         = (unsigned int)((value >> 47) & 0x1);
    gate.off_31_16 = (unsigned int)((value >> 48) & 0xFFFF);

    // Print the values
    printf("off_15_0  : %d\n", gate.off_15_0);
    printf("cs        : %d\n", gate.cs);
    printf("args      : %d\n", gate.args);
    printf("rsv1      : %d\n", gate.rsv1);
    printf("type      : %d\n", gate.type);
    printf("s         : %d\n", gate.s);
    printf("dpl       : %d\n", gate.dpl);
    printf("p         : %d\n", gate.p);
    printf("off_31_16 : %d\n", gate.off_31_16);
}

void pgg(struct gatedesc gate) {
    printf("off_15_0  : %d\n", gate.off_15_0);
    printf("cs        : %d\n", gate.cs);
    printf("args      : %d\n", gate.args);
    printf("rsv1      : %d\n", gate.rsv1);
    printf("type      : %d\n", gate.type);
    printf("s         : %d\n", gate.s);
    printf("dpl       : %d\n", gate.dpl);
    printf("p         : %d\n", gate.p);
    printf("off_31_16 : %d\n", gate.off_31_16);
}

uint64_t printgate(struct gatedesc myGateDesc)
{
        uint64_t combinedValue = ((uint64_t)myGateDesc.off_15_0) |
                             ((uint64_t)myGateDesc.cs << 16) |
                             ((uint64_t)myGateDesc.args << 32) |
                             ((uint64_t)myGateDesc.rsv1 << 37) |
                             ((uint64_t)myGateDesc.type << 40) |
                             ((uint64_t)myGateDesc.s << 44) |
                             ((uint64_t)myGateDesc.dpl << 45) |
                             ((uint64_t)myGateDesc.p << 47) |
                             ((uint64_t)myGateDesc.off_31_16 << 48);
        return combinedValue;
}

struct trapframe {
    // registers as pushed by pusha
    unsigned int edi;
    unsigned int esi;
    unsigned int ebp;
    unsigned int oesp; // useless & ignored
    unsigned int ebx;
    unsigned int edx;
    unsigned int ecx;
    unsigned int eax;

    // rest of trap frame
    unsigned short gs;
    unsigned short padding1;
    unsigned short fs;
    unsigned short padding2;
    unsigned short es;
    unsigned short padding3;
    unsigned short ds;
    unsigned short padding4;
    unsigned int trapno;

    // below here defined by x86 hardware
    unsigned int err;
    unsigned int eip;
    unsigned short cs;
    unsigned short padding5;
    unsigned int eflags;

    // below here only when crossing rings, such as from user to kernel
    unsigned int esp;
    unsigned short ss;
    unsigned short padding6;
};

void interrupt_dispatch(struct trapframe *tf) __attribute((used));
void idtinit(void);

uint64_t idt[256];
extern unsigned int vectors[]; // in vectors.S: array of 256 entry pointers

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
    uint64_t x, y;
    for (int i = 0; i < 256; i++) {
        // SETGATE(idt[i], 0, 1 << 3, vectors[i], 0);
        // x = printgate(idt[i]);
         idt[i] = get_idt_entry(vectors[i], 0);
    //      if (x != y) {
    //                 printuint( x);
    //                                     printuint( y);

    //    printf("------------\n");
    //      }
    }
    //                 printuint( x);
    //                                     printuint( y);

    //    printf("------------\n");
    //    pgg(idt[255]);
                            // printf("------------\n");

    //    pgui(x);
    //                  printf("------------\n");

    //    pgui(y);


    lidt(idt, sizeof(idt));
}

void interrupt_dispatch(struct trapframe *tf)
{
    printf("inside int dispatch %d %d\n", tf->trapno, 100);
}
