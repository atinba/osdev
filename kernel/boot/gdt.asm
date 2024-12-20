section .data

; Access bits
PRESENT        equ 1 << 7
NOT_SYS        equ 1 << 4
EXEC           equ 1 << 3
DC             equ 1 << 2
RW             equ 1 << 1
ACCESSED       equ 1 << 0

; Flags bits
GRAN_4K       equ 1 << 7
SZ_32         equ 1 << 6
LONG_MODE     equ 1 << 5

GDT:
    .Null: equ $ - GDT
        dq 0
    .Code: equ $ - GDT
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | EXEC | RW            ; Access
        db GRAN_4K | LONG_MODE | 0xF                ; Flags & Limit (high, bits 16-19)
        db 0                                        ; Base (high, bits 24-31)
    .Data: equ $ - GDT
        dd 0xFFFF                                   ; Limit & Base (low, bits 0-15)
        db 0                                        ; Base (mid, bits 16-23)
        db PRESENT | NOT_SYS | RW                   ; Access
        db GRAN_4K | SZ_32 | 0xF                    ; Flags & Limit (high, bits 16-19)
        db 0
    .TSS: equ $ - GDT
        dd 0x00000068
        dd 0x00CF8900
    .Pointer:
        dw $ - GDT - 1
        dq GDT

    ;GDT         dq 0, GDT_CODE, GDT_DATA
    ;GDT_END     equ $

    ;GDTR:
    ;    dw GDT_END - GDT - 1
    ;    dd GDT

;CODE_SEG equ gdt_code - gdt
;DATA_SEG equ .DATA - GDT

section .text
global load_gdt
load_gdt:
    lgdt [GDT.Pointer]
	;jmp CODE_SEG:.setcs       ; Set CS to our 32-bit flat code selector
	;.setcs:
;	mov rax, DATA_SEG          ; Setup the segment registers with our flat data selector
	mov ds, rax
	mov es, rax
	mov fs, rax
	mov gs, rax
	mov ss, rax
    ret
