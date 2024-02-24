#include <stddef.h>
#include <stdint.h>

#include <kernel/tty.h>

#define VGA_COLOR  0x0A
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint16_t *)0xB8000

static size_t term_row;
static size_t term_col;
static uint16_t *term_buf;

static inline void set_term_entry(size_t x, size_t y, char c)
{
    const size_t index = (y)*VGA_WIDTH + (x);
    term_buf[index] = (uint16_t)c | (uint16_t)VGA_COLOR << 8;
}

void tty_init(void)
{
    term_row = 0;
    term_col = 0;
    term_buf = VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            set_term_entry(x, y, ' ');
}

// TODO: add scroll
void tty_putc(char c)
{
    if (c == '\n') {
        term_row++;
        term_col = 0;
        if (term_row == VGA_HEIGHT)
            term_row = 0;
        return;
    }

    set_term_entry(term_col, term_row, c);

    if (++term_col == VGA_WIDTH) {
        term_col = 0;
        if (++term_row == VGA_HEIGHT)
            term_row = 0;
    }
}

void tty_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        tty_putc(data[i]);
}
