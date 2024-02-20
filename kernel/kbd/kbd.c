#include <stdint.h>
#include "stdio.h"

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT    0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT    0xA1
// IO Ports for Keyboard
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

extern void enable_interrupts();
extern char ioport_in(unsigned short port);
extern void ioport_out(unsigned short port, unsigned char data);

extern void inl(uint16_t port);
extern void outl(uint16_t port, uint32_t data);

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
// Right control, right alt seem to send
// keycode 224, then the left control/alt keycode
// Arrow keys also send two interrupts, one 224 and then their actual code
// Same for numpad enter
// 197: Num Lock
// 157: Pause|Break (followed by 197?)
// Clicking on screen appears to send keycodes 70, 198
// Is this the MARK command or something like that?

void kbdinit()
{
    ioport_out(0x3D4, 0x0A);
    ioport_out(0x3D5, 0x20);

    // Program the PICs - Programmable Interrupt Controllers
    // Background:
    // In modern architectures, the PIC is not a separate chip.
    // It is emulated in the CPU for backwards compatability.
    // The APIC (Advanced Programmable Interrupt Controller)
    // is the new version of the PIC that is integrated into the CPU.
    // Default vector offset for PIC is 8
    // This maps IRQ0 to interrupt 8, IRQ1 to interrupt 9, etc.
    // This is a problem. The CPU reserves the first 32 interrupts for
    // CPU exceptions such as divide by 0, etc.
    // In programming the PICs, we move this offset to 0x2 (32) so that
    // we can handle all interrupts coming to the PICs without overlapping
    // with any CPU exceptions.

    // Send ICWs - Initialization Command Words
    // PIC1: IO Port 0x20 (command), 0xA0 (data)
    // PIC2: IO Port 0x21 (command), 0xA1 (data)
    // ICW1: Initialization command
    // Send a fixed value of 0x11 to each PIC to tell it to expect ICW2-4
    // Restart PIC1

    ioport_out(PIC1_COMMAND_PORT, 0x11);
    ioport_out(PIC2_COMMAND_PORT, 0x11);
    // ICW2: Vector Offset (this is what we are fixing)
    // Start PIC1 at 32 (0x20 in hex) (IRQ0=0x20, ..., IRQ7=0x27)
    // Start PIC2 right after, at 40 (0x28 in hex)
    ioport_out(PIC1_DATA_PORT, 0x20);
    ioport_out(PIC2_DATA_PORT, 0x28);
    // ICW3: Cascading (how master/slave PICs are wired/daisy chained)
    // Tell PIC1 there is a slave PIC at IRQ2 (why 4? don't ask me - https://wiki.osdev.org/8259_PIC)
    // Tell PIC2 "its cascade identity" - again, I'm shaky on this concept. More resources in notes
    ioport_out(PIC1_DATA_PORT, 0x0);
    ioport_out(PIC2_DATA_PORT, 0x0);
    // ICW4: "Gives additional information about the environemnt"
    // See notes for some potential values
    // We are using 8086/8088 (MCS-80/85) mode
    // Not sure if that's relevant, but there it is.
    // Other modes appear to be special slave/master configurations (see wiki)
    ioport_out(PIC1_DATA_PORT, 0x1);
    ioport_out(PIC2_DATA_PORT, 0x1);
    // Voila! PICs are initialized

    // Mask all interrupts (why? not entirely sure)
    // 0xff is 16 bits that are all 1.
    // This masks each of the 16 interrupts for that PIC.
    ioport_out(PIC1_DATA_PORT, 0xff);
    ioport_out(PIC2_DATA_PORT, 0xff);

    // 0xFD = 1111 1101 in binary. enables only IRQ1
    // Why IRQ1? Remember, IRQ0 exists, it's 0-based
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
