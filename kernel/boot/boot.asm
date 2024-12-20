; Declare constants for the multiboot header.
MAGIC    equ 0xE85250D6

; Declare a header as in the Multiboot Standard.
section .mb2_hdr
align 4

mb2_hdr_begin:
    dd MAGIC
    dd 0
    dd (mb2_hdr_end - mb2_hdr_begin)
    dd -(MAGIC + (mb2_hdr_end - mb2_hdr_begin))

; framebuffer tag: type = 5
mb2_framebuffer_req:
    dw 5
    dw 1
    dd (mb2_framebuffer_end - mb2_framebuffer_req)
    ; preferred width, height, bpp.
    ; leave as zero to indicate "don't care"
    dd 0
    dd 0
    dd 0
mb2_framebuffer_end:

; the end tag: type = 0, size = 8
dd 0
dd 8
mb2_hdr_end:

section .mb2_text
global _start
_start:
	mov (boot_page_table1 - 0xC0000000), edi
	mov 0, esi
	; Map 1023 pages. The 1024th will be the VGA text buffer.
	mov 1023, ecx

s1:
	; Only map the kernel.
	cmp _kernel_start, esi
	jmp s2
	cmp (_kernel_end - 0xC0000000), esi
	jge s3

	; Map physical address as "present, writable". Note that this maps
	; .text and .rodata as writable. Mind security and map them as non-writable.
	mov esi, edx
	or 0x003, edx
	mov edx, (edi)

s2:
	; Size of page is 4096 bytes.
	add 4096, esi
	; Size of entries in boot_page_table1 is 4 bytes.
	add 4, edi
	; Loop to the next entry if we haven't finished.
	loop s1

s3:
	; Map VGA video memory to 0xC03FF000 as "present, writable".
	mov (0x000B8000 | 0x003), boot_page_table1 - 0xC0000000 + 1023 * 4

	; The page table is used at both page directory entry 0 (virtually from 0x0
	; to 0x3FFFFF) (thus identity mapping the kernel) and page directory entry
	; 768 (virtually from 0xC0000000 to 0xC03FFFFF) (thus mapping it in the
	; higher half). The kernel is identity mapped because enabling paging does
	; not change the next instruction, which continues to be physical. The CPU
	; would instead page fault if there was no identity mapping.

	; Map the page table to both virtual addresses 0x00000000 and 0xC0000000.
	mov (boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 0
	mov (boot_page_table1 - 0xC0000000 + 0x003), boot_page_directory - 0xC0000000 + 768 * 4

	; Set cr3 to the address of the boot_page_directory.
	mov (boot_page_directory - 0xC0000000), ecx
	mov ecx, cr3

	; Enable paging and the write-protect bit.
	mov cr0, ecx
	or 0x80010000, ecx
	mov ecx, cr0

	; Jump to higher half with an absolute jump.
	lea s4, ecx
	jmp ecx

section .text

s4:
	; At this point, paging is fully set up and enabled.

	; Unmap the identity mapping as it is now unnecessary.
	mov 0, boot_page_directory + 0

	; Reload crc3 to force a TLB flush so the changes to take effect.
	mov cr3, ecx
	mov ecx, cr3

	; Set up the stack.
	mov stack_top, esp

	; Enter the high-level kernel.
	call kernel_main

	; Infinite loop if the system has nothing more to do.
	cli
ss1:	hlt
	jmp ss1

