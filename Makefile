OS:=myos.bin

# Arch Info
HOST:=i686-elf
HOSTARCH:=i386
ARCHDIR:=arch/$(HOSTARCH)
include $(ARCHDIR)/build.mk

# Build Info
AR:=$(HOST)-ar
AS:=$(HOST)-as
CC:=$(HOST)-gcc

RELEASE_FLAGS:=-O2 # Todo: release
DEFAULT_FLAGS:=-g -ffreestanding -O2
WARN_FLAGS:= -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
EXTRA_FLAGS:=-fstack-protector-all
INC_FLAGS:=-Iinclude
LIB_FLAGS:=-Llib -nostdlib -lk -lgcc
CFLAGS:=$(DEFAULT_FLAGS)  $(INC_FLAGS)

KERN_MACRO:=-D__is_kernel
LIBC_MACRO:=-D__is_libc
LIBK_MACRO:=-D__is_libk

CC_CF:=$(CC) $(CFLAGS) -std=gnu11

# Objs
ARCH_OBJS := $(addprefix $(ARCHDIR)/,$(KERNEL_ARCH_OBJS))
KOBJS=$(ARCH_OBJS) kernel/kernel/kernel.o

.PHONY: all clean lib
.SUFFIXES: 

# Rules
all: $(OS)

$(OS): lib $(KOBJS) $(ARCHDIR)/linker.ld
	$(CC_CF) -T $(ARCHDIR)/linker.ld -o $@ $(KOBJS) $(LIB_FLAGS)
	grub-file --is-x86-multiboot $@
	$(RM) *.a *.o */*.o */*/*.o *.d */*.d */*/*.d

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC_CF) -print-file-name=$(@F)` && cp "$$OBJ" $@

%.o: %.c
	$(CC_CF) $(CPPFLAGS) -MD -c $< -o $@

%.o: %.s
	$(CC_CF) $(CPPFLAGS) -MD -c $< -o $@

# LIB
LIB_SRC := $(wildcard lib/**/*.c)
LIBC_OBJS=$(LIB_SRC:.c=.libc.o)
LIBK_OBJS=$(LIB_SRC:.c=.libk.o)
LIB_BIN:=lib/libk.a # libc.a

lib: $(LIB_BIN)

lib/libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

%.libk.o: %.c
	$(CC_CF) -MD -c $< -o $@ $(LIBK_MACRO)

%.libk.o: %.S
	$(CC_CF) -MD -c $< -o $@ $(LIBK_MACRO)

libc.a: $(LIBC_OBJS)
	$(AR) rcs $@ $(LIBC_OBJS)

%.libc.o: %.c
	$(CC_CF) -MD -c $< -o $@ $(LIBC_MACRO)

%.libc.S: %.c
	$(CC_CF) -MD -c $< -o $@ $(LIBC_MACRO)

clean:
	$(RM) $(OS) $(LIB_BIN)
	$(RM) $(KOBJS) $(KOBJS:.o=.d)
	$(RM) $(LIBK_OBJS) $(LIBK_OBJS:.o=.d)
	$(RM) *.a *.o */*.o */*/*.o *.d */*.d */*/*.d
	git clean -iX

qemu: $(OS)
	qemu-system-i386 -kernel $(OS)

todolist:
	rg -n -i TODO .

re:
	make clean
	make

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
