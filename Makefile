TARGET = kernel.elf
ISO = build/os.iso
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Darwin)
    # macOS
    CC = x86_64-elf-gcc
    LD = x86_64-elf-ld
    MKISO = xorriso -as mkisofs
else
    # Linux (assumes native GCC + genisoimage)
    CC = gcc
    LD = ld
    MKISO = genisoimage
endif

AS = nasm

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -I$(INCLUDE_DIR)
ASFLAGS = -f elf32
LDFLAGS = -T link.ld -m elf_i386

SOURCES_C := $(wildcard $(SRC_DIR)/kernel/*.c $(SRC_DIR)/drivers/*.c $(SRC_DIR)/lib/*.c)
SOURCES_S := $(wildcard $(SRC_DIR)/boot/*.s)

OBJECTS := $(patsubst %.c, %.o, $(SOURCES_C)) $(patsubst %.s, %.o, $(SOURCES_S))

all: $(ISO)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<

$(ISO): $(BUILD_DIR)/$(TARGET)
	@mkdir -p $(BUILD_DIR)/iso/boot/grub
	cp $(BUILD_DIR)/$(TARGET) $(BUILD_DIR)/iso/boot/kernel.elf
	cp grub/stage2_eltorito $(BUILD_DIR)/iso/boot/grub/
	cp grub/menu.lst $(BUILD_DIR)/iso/boot/grub/
	$(MKISO) -R -b boot/grub/stage2_eltorito \
		-no-emul-boot -boot-load-size 4 -A os \
		-input-charset utf8 -quiet -boot-info-table \
		-o $(ISO) $(BUILD_DIR)/iso

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)

clean:
	find . -name '*.o' -delete
	rm -rf $(BUILD_DIR)

.PHONY: all clean run