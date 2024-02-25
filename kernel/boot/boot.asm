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

; The kernel entry point.
section .text
    global _start
    section .text
_start:
    extern load_gdt
    call load_gdt

    cli
    mov eax, stack_top
    mov esp, eax


    extern kmain
    call kmain
; Hang if kernel_main unexpectedly returns.
.hang:
    hlt
    jmp .hang


