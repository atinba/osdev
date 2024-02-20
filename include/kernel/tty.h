#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

void tty_init(void);
void tty_putc(char);
void tty_write(const char *, size_t);

#endif
