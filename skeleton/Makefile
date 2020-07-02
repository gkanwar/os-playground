BIN=bin
SRC=src
INCLUDE=include
KERNEL_SRC=$(SRC)/kernel
SYSTEM_SRC=$(SRC)/system # TODO: rename to clib or some such?

CROSSBIN=$(HOME)/opt/cross/bin
AS=$(CROSSBIN)/i686-elf-as
CC=$(CROSSBIN)/i686-elf-gcc
CXX=$(CROSSBIN)/i686-elf-g++
LD=$(CROSSBIN)/i686-elf-ld

CXX_FLAGS=-std=c++14 -ffreestanding -O2 -Wall -Wextra


.SUFFIXES: # remove default rules
all: kernel

ASM_SRCS = \
	$(KERNEL_SRC)/bootstrap.s
CPP_SRCS = \
	$(KERNEL_SRC)/kernel.cpp \
	$(SYSTEM_SRC)/stdlib.cpp

# source rules
$(KERNEL_BIN)/%.o: $(KERNEL_SRC)/%.cpp
	$(CXX) -c $< -o $@ $(CXX_FLAGS)
$(SYSTEM_BIN)/%.o: $(SYSTEM_SRC)/%.cpp
	$(CXX) -c $< -o $@ $(CXX_FLAGS)

# TODO: rest of targets
$(BIN):
	mkdir -p $(BIN)
$(BIN)/bootstrap.o: bootstrap.s
	$(AS) -o $(BIN)/bootstrap.o bootstrap.s
$(BIN)/kernel.o: kernel.cpp
	$(CXX) -c kernel.cpp -o $(BIN)/kernel.o -std=c++14 -ffreestanding -O2 -Wall -Wextra
kernel: $(BIN) $(KERNEL_BIN)/bootstrap.o $(BIN)/kernel.o kernel.ld
	$(CXX) -T kernel.ld -o $(BIN)/kernel.bin -ffreestanding \
	-O2 -nostdlib $(BIN)/bootstrap.o $(BIN)/kernel.o -lgcc
	./check_multiboot.sh $(BIN)/kernel.bin
grub-image: kernel
	mkdir -p $(BIN)/iso/boot/grub
	cp $(BIN)/kernel.bin $(BIN)/iso/boot/
	cp grub.cfg $(BIN)/iso/boot/grub/
	grub-mkrescue -o $(BIN)/tejos.iso $(BIN)/iso
