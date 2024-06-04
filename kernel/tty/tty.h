#ifndef _TTY_TTY_H
#define _TTY_TTY_H

#include <kernel/types.h>
#include <stddef.h>

#define VGA_COLOR  0x0A
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_SIZE   VGA_WIDTH *VGA_HEIGHT
#define VGA_MEMORY (u16 *)0xB8000

static size_t term_index;
static u16 *term_buf;

static inline void set_term_entry(size_t index, char c)
{
    term_buf[index] = (u16)c | (u16)VGA_COLOR << 8;
}

static inline void backspace(void)
{
    if (term_index > 0) {
        term_index--;
        set_term_entry(term_index, ' ');
    }
}
#endif
