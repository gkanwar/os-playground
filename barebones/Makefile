BIN=bin
CROSSBIN=$(HOME)/opt/cross/bin
AS=$(CROSSBIN)/i686-elf-as
CC=$(CROSSBIN)/i686-elf-gcc
LD=$(CROSSBIN)/i686-elf-ld

.SUFFIXES: # remove default rules
all: kernel

$(BIN):
	mkdir -p $(BIN)
$(BIN)/bootstrap.o: bootstrap.s
	$(AS) -o $(BIN)/bootstrap.o bootstrap.s
$(BIN)/kernel.o: kernel.c
	$(CC) -c kernel.c -o $(BIN)/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
kernel: $(BIN) $(BIN)/bootstrap.o $(BIN)/kernel.o kernel.ld
	$(CC) -T kernel.ld -o $(BIN)/kernel.bin -ffreestanding \
	-O2 -nostdlib $(BIN)/bootstrap.o $(BIN)/kernel.o -lgcc
	./check_multiboot.sh $(BIN)/kernel.bin
grub-image: kernel
	mkdir -p $(BIN)/iso/boot/grub
	cp $(BIN)/kernel.bin $(BIN)/iso/boot/
	cp grub.cfg $(BIN)/iso/boot/grub/
	grub-mkrescue -o $(BIN)/tejos.iso $(BIN)/iso
