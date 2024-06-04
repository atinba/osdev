#include <kernel/types.h>

#include <lib/stdio.h>

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

void interrupt_dispatch(struct trapframe *tf)
{
    printk("inside int dispatch, trap no: %d \n", tf->trapno);
}
