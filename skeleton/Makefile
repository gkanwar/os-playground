BIN=bin
SRC=src
INCLUDE=include
KERNEL_OUT=$(BIN)/kernel.bin

# make sure output dirs exist
$(shell mkdir -p $(BIN))
$(shell mkdir -p $(BIN)/kernel)
$(shell mkdir -p $(BIN)/system)

CROSSBIN=$(HOME)/opt/cross/bin
AS=$(CROSSBIN)/i686-elf-as
CC=$(CROSSBIN)/i686-elf-gcc
CXX=$(CROSSBIN)/i686-elf-g++
LD=$(CROSSBIN)/i686-elf-ld

CFLAGS=-ffreestanding -O2 -Wall -Wextra
CXXFLAGS=-std=c++14 -ffreestanding -O2 -Wall -Wextra
ASFLAGS=-x assembler-with-cpp


.SUFFIXES: # remove default rules
.PHONY: all clean kernel kernel-binary grub-image
all: kernel
clean:
	rm -rf $(BIN)

KERNEL_OBJS = \
	bootstrap.o \
	kernel.o \
	phys_mem_allocator.o \
	vga.o
SYSTEM_OBJS = \
	stdlib.o
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
OBJS = \
	$(BIN)/kernel/crti.o \
	$(CRTBEGIN_OBJ) \
	$(addprefix $(BIN)/kernel/,$(KERNEL_OBJS)) \
	$(addprefix $(BIN)/system/,$(SYSTEM_OBJS)) \
	$(CRTEND_OBJ) \
	$(BIN)/kernel/crtn.o
# have to get the link order right
LIBS := -nostdlib -lgcc
LINK_LIST := \
	$(BIN)/kernel/crti.o \
	$(CRTBEGIN_OBJ) \
	$(addprefix $(BIN)/kernel/,$(KERNEL_OBJS)) \
	$(addprefix $(BIN)/system/,$(SYSTEM_OBJS)) \
	$(LIBS) \
	$(CRTEND_OBJ) \
	$(BIN)/kernel/crtn.o


# source rules
$(BIN)/kernel/%.o: $(SRC)/kernel/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -isystem $(INCLUDE)/system -iquote $(INCLUDE)/kernel
$(BIN)/kernel/%.o: $(SRC)/kernel/%.s
	$(CXX) $(ASFLAGS) -c $< -o $@ -isystem $(INCLUDE)/system -iquote $(INCLUDE)/kernel
$(BIN)/system/%.o: $(SRC)/system/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ -iquote $(INCLUDE)/system

# build the kernel binary
$(KERNEL_OUT): $(OBJS) kernel.ld
	$(CXX) -T kernel.ld -o $@ $(CXXFLAGS) $(LINK_LIST)
kernel: $(KERNEL_OUT)
# depends on grub install which not all dev envs will have
# ./check_multiboot.sh $(BIN)/kernel.bin

# package the kernel with grub into a bootable .iso
grub-image: kernel grub.cfg
	mkdir -p $(BIN)/iso/boot/grub
	cp $(KERNEL_OUT) $(BIN)/iso/boot/
	cp grub.cfg $(BIN)/iso/boot/grub/
	grub-mkrescue -o $(BIN)/tejos.iso $(BIN)/iso
