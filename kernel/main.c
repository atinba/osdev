#include <stdio.h>

#include <kernel/tty.h>

extern void gdt_init();
extern void idt_init();

void kmain(void)
{
    tty_init();
    gdt_init();
    idt_init();
    printf("Hello, kernel World!\n");
    // while(1);
    printf("div by 0: %d\n", 1 / 0);
}
