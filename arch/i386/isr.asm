section .text
    extern interrupt_dispatch
    global interrupt_stub

interrupt_stub:
    push ds
    push es
    push fs
    push gs
    pushad
    mov edi, esp
    call interrupt_dispatch
    mov esp, eax

    popad
    pop fs
    pop es
    pop ds

    add esp, 8

    iret
