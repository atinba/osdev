section .data

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

GDT_CODE    equ 0x00CF9A000000FFFF
GDT_DATA    equ 0x00CF92000000FFFF
    ;GDT         dq 0, GDT_CODE, GDT_DATA
    ;GDT_END     equ $

    ;GDTR:
    ;    dw GDT_END - GDT - 1
    ;    dd GDT

CODE_SEG equ gdt_code - gdt
DATA_SEG equ gdt_data - gdt

section .text
global load_gdt
load_gdt:
    lgdt [gdt_desc]
	jmp CODE_SEG:.setcs       ; Set CS to our 32-bit flat code selector
	.setcs:
	mov ax, DATA_SEG          ; Setup the segment registers with our flat data selector
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
    ret
