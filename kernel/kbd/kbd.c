#include <stdint.h>
#include "stdio.h"

#include "kernel/kbd.h"

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT    0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT    0xA1
// IO Ports for Keyboard
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

extern void enable_interrupts(void);
extern char ioport_in(unsigned short port);
extern void ioport_out(unsigned short port, unsigned char data);

extern void inl(uint16_t port);
extern void outl(uint16_t port, uint32_t data);

// Right control, right alt seem to send
// keycode 224, then the left control/alt keycode
// Arrow keys also send two interrupts, one 224 and then their actual code
// Same for numpad enter
// 197: Num Lock
// 157: Pause|Break (followed by 197?)
// Clicking on screen appears to send keycodes 70, 198
// Is this the MARK command or something like that?
unsigned char keyboard_map[128] = {
    // -------- 0 to 9 --------
    ' ',
    ' ', // 1: escape key
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    // -------- 10 to 19 --------
    '9',
    '0',
    '-',
    '=',
    ' ', // Backspace
    ' ', // Tab
    'q',
    'w',
    'e',
    'r',
    // -------- 20 to 29 --------
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    ' ', // Enter
    ' ', // left Ctrl
    // -------- 30 to 39 --------
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    // -------- 40 to 49 --------
    ' ',
    '`',
    ' ', // left Shift
    ' ',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    // -------- 50 to 59 --------
    'm',
    ',',
    '.',
    '/', // slash, or numpad slash if preceded by keycode 224
    ' ', // right Shift
    '*', // numpad asterisk
    ' ', // left Alt
    ' ', // Spacebar
    ' ',
    ' ', // F1
    // -------- 60 to 69 --------
    ' ', // F2
    ' ', // F3
    ' ', // F4
    ' ', // F5
    ' ', // F6
    ' ', // F7
    ' ', // F8
    ' ', // F9
    ' ', // F10
    ' ',
    // -------- 70 to 79 --------
    ' ', // scroll lock
    '7', // numpad 7, HOME key if preceded by keycode 224
    '8', // numpad 8, up arrow if preceded by keycode 224
    '9', // numpad 9, PAGE UP key if preceded by keycode 224
    '-', // numpad hyphen
    '4', // numpad 4, left arrow if preceded by keycode 224
    '5', // numpad 5
    '6', // numpad 6, right arrow if preceded by keycode 224
    ' ',
    '1', // numpad 1, END key if preceded by keycode 224
    // -------- 80 to 89 --------
    '2', // numpad 2, down arrow if preceded by keycode 224
    '3', // numpad 3, PAGE DOWN key if preceded by keycode 224
    '0', // numpad 0, INSERT key if preceded by keycode 224
    '.', // numpad dot, DELETE key if preceded by keycode 224
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    // -------- 90 to 99 --------
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    // -------- 100 to 109 --------
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    // -------- 110 to 119 --------
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    // -------- 120-127 --------
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
    ' ',
};

// TODO: Figure out what's happening here
void kbd_init()
{
    // Disable cursor
    ioport_out(0x3D4, 0x0A);
    ioport_out(0x3D5, 0x20);

    // https://wiki.osdev.org/8259_PIC
    // ICW1: Init commands
    ioport_out(PIC1_COMMAND_PORT, 0x11);
    ioport_out(PIC2_COMMAND_PORT, 0x11);

    // ICW2: Vector offset (start PIC1 at 0x20, PIC2 at 0x28)
    ioport_out(PIC1_DATA_PORT, 0x20);
    ioport_out(PIC2_DATA_PORT, 0x28);

    // ICW3: ???
    ioport_out(PIC1_DATA_PORT, 0x0);
    ioport_out(PIC2_DATA_PORT, 0x0);

    // ICW4: ???
    ioport_out(PIC1_DATA_PORT, 0x1);
    ioport_out(PIC2_DATA_PORT, 0x1);

    // Mask interrupts
    ioport_out(PIC1_DATA_PORT, 0xff);
    ioport_out(PIC2_DATA_PORT, 0xff);

    // Enable IRQ1 for kbd
    ioport_out(PIC1_DATA_PORT, 0xFD);
    enable_interrupts();
}

void handle_keyboard_interrupt()
{
    // Write end of interrupt (EOI)
    ioport_out(PIC1_COMMAND_PORT, 0x20);

    unsigned char status = ioport_in(KEYBOARD_STATUS_PORT);
    // Lowest bit of status will be set if buffer not empty
    // (thanks mkeykernel)
    if (status & 0x1) {
        char keycode = ioport_in(KEYBOARD_DATA_PORT);
        if (keycode < 0 || keycode >= 128)
            return;
        if (keycode == 28) {
            putc('\n');
        } else if (keycode == 14) {
            //backspace();
        } else if (keycode == 1) {
            //vga_exit();
        } else {
            putc(keyboard_map[keycode]);
        }
    }
}
