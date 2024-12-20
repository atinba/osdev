#include <lib/stdio.h>
#include <kernel/tty.h>

void kmain(void)
{
    tty_init();
    tty_putc('a');
    // idt_init();
    //    kbd_init();
    // printk("Hello, kernel World!\n");
    //printk("div by 0: %d\n", 1 / 0);
    while (1)
        ;
}
