BIN=bin
SRC=src
INCLUDE=include
KERNEL_OUT=$(BIN)/kernel.bin

# make sure output dirs exist
$(shell mkdir -p $(BIN))

CROSSBIN=$(HOME)/opt/cross/bin
AS=$(CROSSBIN)/i686-elf-as
CC=$(CROSSBIN)/i686-elf-gcc
CXX=$(CROSSBIN)/i686-elf-g++
LD=$(CROSSBIN)/i686-elf-ld

CXX_FLAGS=-std=c++14 -ffreestanding -O2 -Wall -Wextra


.SUFFIXES: # remove default rules
.PHONY: all kernel kernel-binary grub-image
all: kernel

CPP_BARE_SRCS = \
	kernel/kernel.cpp \
	kernel/vga.cpp \
	system/stdlib.cpp
CPP_OBJS = $(addprefix $(BIN)/,$(CPP_BARE_SRCS:.cpp=.o))
CPP_SRCS = $(addprefix $(SRC)/,$(CPP_BARE_SRCS))

# source rules
$(BIN)/kernel/%.o: $(SRC)/kernel/%.cpp
	$(CXX) -c $< -o $@ $(CXX_FLAGS) -isystem $(INCLUDE)/system -iquote $(INCLUDE)/kernel
$(BIN)/system/%.o: $(SRC)/system/%.cpp
	$(CXX) -c $< -o $@ $(CXX_FLAGS) -iquote $(INCLUDE)/system

# special files
$(BIN)/kernel/bootstrap.o: $(SRC)/kernel/bootstrap.s
	$(AS) -o $@ $<

# build the kernel binary
$(KERNEL_OUT): $(BIN)/kernel/bootstrap.o $(CPP_OBJS)
	$(CXX) $^ -T kernel.ld -o $@ $(CXX_FLAGS) -nostdlib -lgcc
kernel: $(KERNEL_OUT) kernel.ld
# depends on grub install which not all dev envs will have
# ./check_multiboot.sh $(BIN)/kernel.bin

# package the kernel with grub into a bootable .iso
grub-image: kernel grub.cfg
	mkdir -p $(BIN)/iso/boot/grub
	cp $(KERNEL_OUT) $(BIN)/iso/boot/
	cp grub.cfg $(BIN)/iso/boot/grub/
	grub-mkrescue -o $(BIN)/tejos.iso $(BIN)/iso
