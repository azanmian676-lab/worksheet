NASM = nasm
CC = i686-elf-gcc
LD = i686-elf-ld
QEMU = qemu-system-i386

CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -c
LDFLAGS = -T src/ldscript.ld -melf_i386
NASMFLAGS = -f elf32

ASM_SOURCES = src/start.asm src/hwio.asm
C_SOURCES = src/core.c drivers/screen.c drivers/key.c drivers/term.c

ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)
ASM_OBJECTS := $(ASM_OBJECTS:.s=.o)
C_OBJECTS = $(C_SOURCES:.c=.o)
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

KERNEL = iso/boot/kernel.elf
ISO = os.iso

all: $(ISO)

$(ISO): $(KERNEL)
	@echo "Creating ISO image..."
	mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot \
		-boot-load-size 4 -A os -input-charset utf8 -quiet \
		-boot-info-table -o $(ISO) iso

$(KERNEL): $(OBJECTS)
	@echo "Linking kernel..."
	$(LD) $(LDFLAGS) $(OBJECTS) -o $(KERNEL)

%.o: %.asm
	@echo "Assembling $<..."
	$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.s
	@echo "Assembling $<..."
	$(NASM) $(NASMFLAGS) $< -o $@

%.o: %.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $< -o $@

run: $(ISO)
	@echo "Starting QEMU..."
	$(QEMU) -boot d -cdrom $(ISO) -m 32

run-debug: $(ISO)
	@echo "Starting QEMU with CPU logging..."
	$(QEMU) -nographic -boot d -cdrom $(ISO) -m 32 -d cpu -D logq.txt

run-direct: $(KERNEL)
	@echo "Starting QEMU with kernel directly..."
	$(QEMU) -kernel $(KERNEL) -m 32

clean:
	@echo "Cleaning..."
	rm -f $(OBJECTS) $(KERNEL) $(ISO) logq.txt

.PHONY: all clean run run-debug run-direct

