OS:=myos.bin

# Arch Info
HOST:=i686-elf
HOSTARCH:=i386
ARCHDIR:=arch/$(HOSTARCH)
INCDIR:=include
include $(ARCHDIR)/build.mk

# Build Info
AS:=nasm
AR:=$(HOST)-ar
CC:=$(HOST)-gcc

DEFAULT_FLAGS:=-g -ffreestanding -O0
WARN_FLAGS:= -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes #-fsanitize=address -fno-omit-frame-pointer
EXTRA_FLAGS:=-fstack-protector-all
INC_FLAGS:=-I$(INCDIR)
LIB_FLAGS:=-Llib -nostdlib -lk -lgcc
CFLAGS:=$(DEFAULT_FLAGS) $(INC_FLAGS) $(WARN_FLAGS)

DEBUG_MACRO:=#TODO

CC_CF:=$(CC) $(CFLAGS) -std=gnu11

# Objs
SRCFILES:=$(shell find . -name '*.c' -o -name '*.h' -name '*.asm')
ARCH_OBJS := $(addprefix $(ARCHDIR)/,$(KERNEL_ARCH_OBJS))
KOBJS=$(ARCH_OBJS) kernel/main.o

# TODO: use checkmake/remake to lint makefile
.PHONY: all clean lib test
.SUFFIXES: 

# Rules
all: $(OS)

$(OS): lib $(KOBJS) $(ARCHDIR)/linker.ld
	$(CC_CF) -T $(ARCHDIR)/linker.ld -o $@ $(KOBJS) $(LIB_FLAGS)
	grub-file --is-x86-multiboot $@
	$(RM) *.a *.o */*.o */*/*.o *.d */*.d */*/*.d

%.o: %.c
	$(CC_CF) $(CPPFLAGS) -MD -c $< -o $@

%.o: %.asm
	$(AS) -felf32 $< -o $@

# LIB
LIB_SRC:=$(shell find lib -name '*.c')
LIBK_OBJS:=$(LIB_SRC:.c=.libk.o)
LIB_BIN:=lib/libk.a

lib: $(LIB_BIN)

lib/libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

%.libk.o: %.c
	$(CC_CF) -MD -c $< -o $@

%.libk.o: %.asm
	$(CC_CF) -MD -c $< -o $@

$(ARCHDIR)/vectors.asm: $(ARCHDIR)/vectors.pl
	./$< > $@

clean:
	$(RM) $(OS) $(LIB_BIN)
	$(RM) $(KOBJS) $(KOBJS:.o=.d)
	$(RM) $(LIBK_OBJS) $(LIBK_OBJS:.o=.d)
	$(RM) *.a *.o */*.o */*/*.o *.d */*.d */*/*.d
	$(RM) $(ARCHDIR)/vectors.asm tags
	git clean -iX

qemu: $(OS)
	qemu-system-i386 -kernel $(OS)

todolist:
	rg -n -i TODO .

format:
	echo $(SRCFILES) | xargs clang-format -i

lint:
	cppcheck --enable=all --inconclusive --std=c11 -I$(INCDIR) .
	find . -iname *.h -o -iname *.c | xargs clang-tidy -checks=cert-* -warnings-as-errors=* -- -I$(INCDIR)
	find . -name '*.c' -o -name '*.h' | xargs splint -I$(INCDIR)

valgrind: $(OS)
	valgrind --leak-check=full --show-leak-kinds=all ./$(OS)

re:
	make clean
	make

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
