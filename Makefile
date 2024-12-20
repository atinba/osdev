OS:=noos.bin

# Arch Info
HOST:=x86_64
INCDIR:=include

# Build Info
AS:=nasm
AR:=$(HOST)-elf-ar
CC:=$(HOST)-elf-gcc
LD:=$(HOST)-elf-ld

QEMU:=qemu-system-$(HOST)
QEMU_FLAGS:=-machine q35 -smp cores=4 -monitor stdio -d int -M smm=off -no-reboot
QEMU_WF:=$(QEMU) $(QEMU_FLAGS)

DEFAULT_FLAGS:=-g -ffreestanding -O0
WARN_FLAGS:= -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes #-fsanitize=address -fno-omit-frame-pointer
EXTRA_FLAGS:=-fstack-protector-all
# TODO: Update
INC_FLAGS:=-I$(INCDIR)
LIB_FLAGS:=-nostdlib -lgcc
CFLAGS:=$(DEFAULT_FLAGS) $(INC_FLAGS) $(WARN_FLAGS) $(LIB_FLAGS)
DEBUG_MACRO:=#TODO

CC_CF:=$(CC) $(CFLAGS) -std=gnu11

# Files
SRC_FILES:=$(shell find . -name '*.c' -o -name '*.asm')
OBJ_FILES = $(addsuffix .o, $(SRC_FILES))
LINKER_FILE:=kernel/boot/linker.ld

# TODO: use checkmake/remake to lint makefile
.PHONY: all clean lib test
.SUFFIXES:

# Rules
all: qemu

os: $(OS)

$(OS): $(OBJ_FILES) $(LINKER_FILE)
	$(CC_CF) -T $(LINKER_FILE) -o $@ $(OBJ_FILES)
	grub-file --is-x86-multiboot $@
	$(RM) *.o */*.o */*/*.o *.d */*.d */*/*.d

%.c.o: %.c
	$(CC_CF) -MD -c $< -o $@

%.asm.o: %.asm
	$(AS) -f elf64 $< -o $@

clean:
	$(RM) $(OS) $(LIB_BIN) noos.iso
	find . -name \*.o -type f -delete
	find . -name \*.d -type f -delete

qemu: $(OS)
	$(QEMU_WF) -kernel $<
	$(RM) $<

debug: $(OS)
	$(QEMU) -kernel $< -s -S &
	gdb \
    -ex "file $<" \
    -ex 'target remote localhost:1234' \
    -ex 'break kmain' \

iso: $(OS)
	mkdir -p isodir/boot/grub
	cp $(OS) isodir/boot/
	echo "menuentry \"noos\" { multiboot /boot/noos.bin}" > isodir/boot/grub/grub.cfg
	grub-mkrescue -o noos.iso isodir
	$(RM) -rf isodir

qemu-iso: iso
	$(QEMU_WF) -cdrom noos.iso -d int --no-reboot

debug-iso: iso
	$(QEMU) -cdrom noos.iso -s -S &
	gdb \
    -ex "file $<" \
    -ex 'target remote localhost:1234' \
    -ex 'break kmain' \

bear:
	bear -- make os

valgrind: $(OS)
	valgrind --leak-check=full --show-leak-kinds=all ./$(OS)

todo:
	rg -n -i TODO .

CFILES:=$(shell find . -name '*.c' -o -name "*.h")

format:
	clang-format -i $(CFILES)

lint:
	cppcheck --enable=all --inconclusive --std=c11 -I$(INCDIR) .
	clang-tidy -checks=cert-* -warnings-as-errors=* $(CFILES) -- -I$(INCDIR)
	splint -I$(INCDIR) $(CFILES)

-include $(OBJFILES:.o=.d)
