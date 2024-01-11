#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <kernel/tty.h>

#include <stdint.h>

#define VGA_COLOR  0x0A
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY (uint16_t *)0xB8000
#define SET_TERM_ENTRY(x, y, c)                   \
    do {                                          \
        const size_t index = (y)*VGA_WIDTH + (x); \
        term_buf[index] = vga_entry(c);           \
    } while (0)

static inline uint16_t vga_entry(char c)
{
    return (uint16_t)c | (uint16_t)VGA_COLOR << 8;
}

static size_t term_row;
static size_t term_col;
static uint16_t *term_buf;

void terminal_initialize(void)
{
    term_row = 0;
    term_col = 0;
    term_buf = VGA_MEMORY;
    for (size_t y = 0; y < VGA_HEIGHT; y++)
        for (size_t x = 0; x < VGA_WIDTH; x++)
            SET_TERM_ENTRY(x, y, ' ');
}

// TODO: add scroll
void terminal_putchar(char c)
{
    if (c == '\n') {
        term_row++;
        term_col = 0;
        if (term_row == VGA_HEIGHT)
            term_row = 0;
        return;
    }

    SET_TERM_ENTRY(term_col, term_row, c);

    if (++term_col == VGA_WIDTH) {
        term_col = 0;
        if (++term_row == VGA_HEIGHT)
            term_row = 0;
    }
}

void terminal_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
        terminal_putchar(data[i]);
}
