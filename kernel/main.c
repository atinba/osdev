#include "stdio.h"
#include "kernel/cpu.h"
#include "kernel/kbd.h"
#include "kernel/tty.h"

void kmain(void)
{
    tty_init();
    idt_init();
    kbd_init();
    printf("Hello, kernel World!\n");
    while (1)
        ;
    //    printf("div by 0: %d\n", 1 / 0);
}
