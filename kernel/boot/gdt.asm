global gdt_init
gdt:
    gdt_null:
        dq 0

    gdt_code:
        dw 0xFFFF
        dw 0
        db 0
        db 0b10011010
        db 0b11001111
        db 0

    gdt_data:
        dw 0xFFFF
        dw 0
        db 0
        db 0b10010010
        db 0b11001111
        db 0

    gdt_end:

gdt_desc:
    dw gdt_end - gdt - 1
    dd gdt


gdt_init:
    lgdt [gdt_desc]
    mov ax, ds
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret