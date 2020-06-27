BIN=bin
CROSSBIN=$(HOME)/opt/cross/bin
AS=$(CROSSBIN)/i686-elf-as
LD=$(CROSSBIN)/i686-elf-ld

all: baby

$(BIN):
	mkdir -p $(BIN)
baby: $(BIN) baby_boot.s
	$(AS) -o $(BIN)/baby_boot.o baby_boot.s
	$(LD) -o $(BIN)/baby_boot.bin --oformat binary --Ttext 0x7c00 $(BIN)/baby_boot.o
