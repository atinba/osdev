#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <kernel/tty.h>

static void putchar(int);
static void printint(int, int);

// TODO: 64 bit ints
void printf(const char *restrict fmt, ...)
{
    va_list parameters;
    va_start(parameters, fmt);

    int nc;
    const char *str;

    while (*fmt) {
        if (*fmt != '%') {
            putchar(*fmt);
            fmt++;
            continue;
        }

        fmt++;

        switch (*fmt) {
        case 'd':
            nc = va_arg(parameters, int);
            printint(nc, 10);
            break;
        case 'c':
            nc = va_arg(parameters, int);
            putchar(nc);
            break;
        case 's':
            str = va_arg(parameters, const char *);
            while (*str) {
                putchar(*str);
                str++;
            }
            break;
        default:
            printf("Invalid format specifier: %c\n", *fmt);
        }

        fmt++;
    }

    va_end(parameters);
}

static void putchar(int ic)
{
    char c = (char)ic;
    terminal_write(&c, sizeof(c));
}

static void printint(int num, int base)
{
    static const char hex_chars[] = "0123456789ABCDEF";
    char buf[16];
    int i = 0;
    int is_neg = num < 0;

    if (is_neg)
        num *= -1;

    buf[i++] = hex_chars[num % base];
    while ((num /= base) != 0)
        buf[i++] = hex_chars[num % base];

    if (is_neg)
        buf[i++] = '-';

    while (--i >= 0)
        putchar(buf[i]);
}
