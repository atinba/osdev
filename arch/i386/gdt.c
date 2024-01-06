#include <stdio.h>

#define STS_IG32    0xE     // 32-bit Interrupt Gate
#define STS_TG32    0xF     // 32-bit Trap Gate

// Gate descriptors for interrupts and traps
struct gatedesc {
  unsigned int off_15_0 : 16;   // low 16 bits of offset in segment
  unsigned int cs : 16;         // code segment selector
  unsigned int args : 5;        // # args, 0 for interrupt/trap gates
  unsigned int rsv1 : 3;        // reserved(should be zero I guess)
  unsigned int type : 4;        // type(STS_{IG32,TG32})
  unsigned int s : 1;           // must be 0 (system)
  unsigned int dpl : 2;         // descriptor(meaning new) privilege level
  unsigned int p : 1;           // Present
  unsigned int off_31_16 : 16;  // high bits of offset in segment
};

// Set up a normal interrupt/trap gate descriptor.
// - istrap: 1 for a trap (= exception) gate, 0 for an interrupt gate.
//   interrupt gate clears FL_IF, trap gate leaves FL_IF alone
// - sel: Code segment selector for interrupt/trap handler
// - off: Offset in code segment for interrupt/trap handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke
//        this interrupt/trap gate explicitly using an int instruction.
#define SETGATE(gate, istrap, sel, off, d)                \
{                                                         \
  (gate).off_15_0 = (unsigned int)(off) & 0xffff;                \
  (gate).cs = (sel);                                      \
  (gate).args = 0;                                        \
  (gate).rsv1 = 0;                                        \
  (gate).type = (istrap) ? STS_TG32 : STS_IG32;           \
  (gate).s = 0;                                           \
  (gate).dpl = (d);                                       \
  (gate).p = 1;                                           \
  (gate).off_31_16 = (unsigned int)(off) >> 16;                  \
}

struct gatedesc idt[256];

extern unsigned int vectors[];  // in vectors.S: array of 256 entry pointers
struct trapframe {
  // registers as pushed by pusha
  unsigned int edi;
  unsigned int esi;
  unsigned int ebp;
  unsigned int oesp;      // useless & ignored
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

static inline void
lidt(struct gatedesc *p, int size)
{
  volatile unsigned short pd[3];

  pd[0] = size-1;
  pd[1] = (unsigned int)p;
  pd[2] = (unsigned int)p >> 16;

  asm volatile("lidt (%0)" : : "r" (pd));
}

void
idtinit(void)
{
    for (int i = 0; i < 256; i++)
        SETGATE(idt[i], 0, 1 << 3, vectors[i], 0);

    lidt(idt, sizeof(idt));
}

void
interrupt_dispatch(struct trapframe *tf)
{
    printf("inside int dispatch %s\n", tf->trapno);
}
