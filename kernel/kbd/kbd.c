#include <kernel/types.h>
#include <kernel/cpu.h>
#include <kernel/kbd.h>
#include <lib/stdio.h>

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

extern void inl(u16 port);
extern void outl(u16 port, u32 data);


// TODO: Figure out what's happening here
void kbd_init(void)
{

    // Disable cursor
    ioport_out(0x3D4, 0x0A);
    ioport_out(0x3D5, 0x20);

    // TODO
    // https://wiki.osdev.org/8259_PIC
    // ICW1: Init commands
    ioport_out(PIC1_COMMAND_PORT, 0x11);
    ioport_out(PIC2_COMMAND_PORT, 0x11);

    // ICW2: Vector offset (start PIC1 at 0x20, PIC2 at 0x28)
    ioport_out(PIC1_DATA_PORT, 0x21);
    ioport_out(PIC2_DATA_PORT, 0x28);

    // ICW3: ???
    ioport_out(PIC1_DATA_PORT, 0x0);
    ioport_out(PIC2_DATA_PORT, 0x0);

    // ICW4: ???
    ioport_out(PIC1_DATA_PORT, 0x1);
    ioport_out(PIC2_DATA_PORT, 0x1);

    // Mask interrupts
    //ioport_out(PIC1_DATA_PORT, 0xff);
    //ioport_out(PIC2_DATA_PORT, 0xff);

    // Enable IRQ1 for kbd
    ioport_out(PIC1_DATA_PORT, 0xFD);
    enable_interrupts();
}

static u8 normalmap[256] = {
    0,    0x1B, '1', '2',  '3',  '4', '5',  '6', // 0x00
    '7',  '8',  '9', '0',  '-',  '=', '\b', '\t',
    'q',  'w',  'e', 'r',  't',  'y', 'u',  'i', // 0x10
    'o',  'p',  '[', ']',  '\n', 0,   'a',  's',
    'd',  'f',  'g', 'h',  'j',  'k', 'l',  ';', // 0x20
    '\'', '`',  0,   '\\', 'z',  'x', 'c',  'v',
    'b',  'n',  'm', ',',  '.',  '/', 0,    '*', // 0x30
    0,    ' ',  0,   0,    0,    0,   0,    0,
    0,    0,    0,   0,    0,    0,   0,    '7', // 0x40
    '8',  '9',  '-', '4',  '5',  '6', '+',  '1',
    '2',  '3',  '0', '.',  0,    0,   0,    0,   // 0x50
};

void handle_keyboard_interrupt()
{
    // Write end of interrupt (EOI)
    ioport_out(PIC1_COMMAND_PORT, 0x20);

    unsigned char status = ioport_in(KEYBOARD_STATUS_PORT);
    // Lowest bit of status will be set if buffer not empty
    // (thanks mkeykernel)
    if (status & 0x1) {
        u8 keycode = ioport_in(KEYBOARD_DATA_PORT);

        if (keycode < 0 || keycode >= 128)
            return;

        putc(normalmap[keycode]);
    }
}
