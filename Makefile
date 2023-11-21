# Config
OS:=myos.bin
DEFAULT_HOST:=i686-elf
HOST?=DEFAULT_HOST
HOSTARCH:=i386

# Dirs
SYSROOT:=sysroot
BOOTDIR:=$(SYSROOT)/boot
SYS_USRDIR:=$(SYSROOT)/usr
SYS_INCDIR:=$(USRDIR)/include
LIBDIR:=$(USRDIR)/lib
INC:=include

# Build Opts
HOST:=i686-elf# TODO
AR:=$(HOST)-ar
AS:=$(HOST)-as
CC:=$(HOST)-gcc

RELEASE_CFLAGS:=-O2
CFLAGS:=-g -ffreestanding
CFLAGS += -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wconversion -Wstrict-prototypes
EXTRA_FLAGS:=-fstack-protector-all
LFLAGS:= -nostdlib -lk -lgcc
CFLAGS+=$(CPPFLAGS) -D__is_libc -Iinclude
LIBK_CFLAGS:=$(CFLAGS)
LIBK_CPPFLAGS:=$(CPPFLAGS) -D__is_libk
# Deps

CPPFLAGS:=$(CPPFLAGS) -D__is_kernel -I../include
LIB_BIN:=libk.a # libc.a
# TODO: phony, suffixes


ARCHDIR=arch/i386# TODO: dont hardcode
include $(ARCHDIR)/build.mk
LIBS:=$(LIBS)-L/home/atin/osdev/ -nostdlib -lk -lgcc
KOBJS := $(addprefix $(ARCHDIR)/,$(KERNEL_ARCH_OBJS))
OBJS=\
$(KOBJS) \
kernel/kernel/kernel.o \

# TODO: figure out order
LL=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS) \

LINK_LIST=\
$(LDFLAGS) \
$(ARCHDIR)/crti.o \
$(ARCHDIR)/crtbegin.o \
$(ARCHDIR)/tty.o \
$(ARCHDIR)/boot.o \
$(LIBS) \
kernel/kernel/kernel.o \
$(ARCHDIR)/crtend.o \
$(ARCHDIR)/crtn.o \

.PHONY: all clean install install-headers install-kernel
# Rules
all: kernel

kernel: $(OS)
$(OS): $(LIB_BIN) $(OBJS) $(ARCHDIR)/linker.ld
	$(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS) $(LINK_LIST)
	grub-file --is-x86-multiboot $(OS)

$(ARCHDIR)/crtbegin.o $(ARCHDIR)/crtend.o:
	OBJ=`$(CC) $(CFLAGS) $(LDFLAGS) -print-file-name=$(@F)` && cp "$$OBJ" $@

%.o: %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.o: %.S
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

# LIBS

FREEOBJS := $(patsubst %.c, %.o, $(wildcard libc/**/*.c))

LIBC_OBJS=\
$(FREEOBJS) \

LIBK_OBJS=$(FREEOBJS:.o=.libk.o)

libc.a: $(LIBC_OBJS)
	$(AR) rcs $@ $(LIBC_OBJS)

libk.a: $(LIBK_OBJS)
	$(AR) rcs $@ $(LIBK_OBJS)

%.o: %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(CFLAGS) $(CPPFLAGS)

%.S: %.c
	$(CC) -MD -c $< -o $@ $(CFLAGS) $(CPPFLAGS)

%.libk.o: %.c
	$(CC) -MD -c $< -o $@ -std=gnu11 $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

%.libk.o: %.S
	$(CC) -MD -c $< -o $@ $(LIBK_CFLAGS) $(LIBK_CPPFLAGS)

clean:
	$(RM) -r $(BUILD_DIR)
	$(RM) -r $(BUILD_DIR)


testkern:
	./clean.sh
	cd libc && make install
	make

-include $(OBJS:.o=.d)
-include $(LIBK_OBJS:.o=.d)
