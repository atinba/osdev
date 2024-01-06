section .text
    extern interrupt_dispatch
    global interrupt_stub

interrupt_stub:
    push ds
    push es
    push fs
    push gs
    pushad ; pushal has been replaced with pushad in NASM

    mov edi, esp
    call interrupt_dispatch
    mov esp, eax

    popad ; popal has been replaced with popad in NASM
    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8

    iret
