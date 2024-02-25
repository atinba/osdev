#include <lib/stdio.h>
#include <kernel/cpu.h>
#include <kernel/kbd.h>
#include <kernel/tty.h>

void kmain(void)
{
    tty_init();
    idt_init();
    kbd_init();
    printk("Hello, kernel World!\n");
    //printk("div by 0: %d\n", 1 / 0);
    while (1)
        ;
}
