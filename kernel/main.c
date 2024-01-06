#include <stdio.h>

#include <kernel/tty.h>
#include <arch/int.h>

void kernel_main(void) {
	terminal_initialize();
    idtinit();
	printf("Hello, kernel World!\n");
	printf("div by 0: %d\n", 1 / 0);
}
