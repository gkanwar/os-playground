BIN=bin
SRC=src
INCLUDE=include
KERNEL_OUT=$(BIN)/kernel.bin
PROJ_NAME=gullfoss

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
CXXFLAGS=-std=c++14 -ffreestanding -fno-rtti -fno-exceptions -O2 -Wall -Wextra -g
ASFLAGS=-x assembler-with-cpp


.SUFFIXES: # remove default rules
.PHONY: all clean kernel kernel-binary grub-image
all: kernel
clean:
	rm -rf $(BIN)

KERNEL_SRCS = \
	bootstrap.s \
	debug_serial.cpp \
	heap_allocator.cpp \
	kernel.cpp \
	panic.cpp \
	phys_mem_allocator.cpp \
	pre_kernel.cpp \
	vga.cpp \
	virt_mem_allocator.cpp
SYSTEM_SRCS = \
	stdlib.cpp

# identify all obj and dep files
KERNEL_OBJS = $(addprefix $(BIN)/kernel/,$(patsubst %.cpp,%.o,$(KERNEL_SRCS:.s=.o)))
KERNEL_DEPS = $(KERNEL_OBJS:.o=.d)
SYSTEM_OBJS = $(addprefix $(BIN)/system/,$(SYSTEM_SRCS:.cpp=.o))
SYSTEM_DEPS = $(SYSTEM_OBJS:.o=.d)

# list objs to compile and (ordered) link arg list for gcc
CRTBEGIN_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtbegin.o)
CRTEND_OBJ := $(shell $(CC) $(CFLAGS) -print-file-name=crtend.o)
OBJS = \
	$(BIN)/kernel/crti.o \
	$(CRTBEGIN_OBJ) \
	$(KERNEL_OBJS) \
	$(SYSTEM_OBJS) \
	$(CRTEND_OBJ) \
	$(BIN)/kernel/crtn.o
LIBS := -nostdlib -lgcc
LINK_LIST := \
	$(BIN)/kernel/crti.o \
	$(CRTBEGIN_OBJ) \
	$(KERNEL_OBJS) \
	$(SYSTEM_OBJS) \
	$(LIBS) \
	$(CRTEND_OBJ) \
	$(BIN)/kernel/crtn.o


# source rules
$(BIN)/kernel/%.o: $(SRC)/kernel/%.cpp
	$(CXX) $(CXXFLAGS) -MD -MP -c $< -o $@ -isystem $(INCLUDE)/system -iquote $(INCLUDE)/kernel
$(BIN)/kernel/%.o: $(SRC)/kernel/%.s
	$(CXX) $(ASFLAGS) -MD -MP -c $< -o $@ -isystem $(INCLUDE)/system -iquote $(INCLUDE)/kernel
$(BIN)/system/%.o: $(SRC)/system/%.cpp
	$(CXX) $(CXXFLAGS) -MD -MP -c $< -o $@ -iquote $(INCLUDE)/system

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
	grub-mkrescue -o $(BIN)/$(PROJ_NAME).iso $(BIN)/iso

-include $(KERNEL_DEPS) $(SYSTEM_DEPS)
