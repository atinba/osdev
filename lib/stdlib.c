#include <lib/stdio.h>
#include <lib/stdlib.h>

__attribute__((__noreturn__)) void abort(void)
{
    // TODO: Add proper kernel panic.
    printk("kernel: panic: abort()\n");
    while (1) {
    }
    __builtin_unreachable();
}
