; Declare constants for the multiboot header.
MBALIGN  equ 1 << 0
MEMINFO  equ 1 << 1
MBFLAGS  equ MBALIGN | MEMINFO
MAGIC    equ 0x1BADB002
CHECKSUM equ -(MAGIC + MBFLAGS)

; Declare a header as in the Multiboot Standard.
section .multiboot
align 4
    dd MAGIC
    dd MBFLAGS
    dd CHECKSUM

; Reserve a stack for the initial thread.
section .bss
align 16
stack_bottom:
resb 16384 ; 16 KiB
stack_top:

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

; The kernel entry point.
section .text
    global _start
    section .text
_start:
    mov eax, stack_top
    mov esp, eax

    call load_gdt

    extern kmain
    call kmain

; Hang if kernel_main unexpectedly returns.
    cli
.hang:
    hlt
    jmp .hang

load_gdt:
    lgdt [gdt_desc]
    mov ax, ds
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

