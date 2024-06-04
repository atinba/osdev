#include <string.h>
#include <stddef.h>

#include <kernel/tty.h>
#include <kernel/types.h>

#include "tty.h"

void tty_init(void)
{
    term_index = 0;
    term_buf = VGA_MEMORY;
    for (size_t i = 0; i < VGA_SIZE; i++)
        set_term_entry(i, ' ');
}

void tty_putc(char c)
{
    if (c == '\b')
        backspace();
    else if (c == '\n')
        term_index = VGA_WIDTH * (term_index / VGA_WIDTH + 1);
    else
        set_term_entry(term_index++, c);

    if (term_index >= VGA_SIZE) {
        size_t mem_size = (VGA_HEIGHT - 1) * VGA_WIDTH;
        memmove(term_buf, term_buf + VGA_WIDTH, mem_size * sizeof(u16));
        term_index = mem_size;
        for (size_t i = term_index; i < VGA_SIZE; i++)
            set_term_entry(i, ' ');
    }
}
