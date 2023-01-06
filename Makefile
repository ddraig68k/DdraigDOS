# Makefile for Y Ddraig ROM
CPU = 68000
TARGET = DdraigDOS

ARCH?=$(CPU)
TUNE?=$(CPU)
EXTRA_CFLAGS?=
DEFINES:=$(DEFINES)

ifeq ($(OS),Windows_NT)
	CROSSDIR = C:/dev/lang/m68k-elf
else
	CROSSDIR = /opt/m68k-elf
endif
BIN = $(CROSSDIR)/bin

BINARY=$(TARGET).bin
MAP=$(TARGET).map
SRECORD=$(TARGET).s68
BINARY_EVEN=$(TARGET)_even.bin
BINARY_ODD=$(TARGET)_odd.bin

CC = $(BIN)/m68k-elf-gcc
AS = $(BIN)/vasmm68k_mot
LD = $(BIN)/m68k-elf-ld
OBJCOPY = $(BIN)/m68k-elf-objcopy
GCC_VERSION = $(shell $(CC) -dumpversion)

CFLAGS=-std=c11 -nostartfiles -Wall -Wpedantic  \
       -I. -mcpu=$(CPU) -march=$(ARCH) -mtune=$(TUNE) -Os   \
       -mstrict-align -fomit-frame-pointer -ffreestanding -Werror $(DEFINES)

LDFLAGS=-T ./ddraig68k_rom.ld -Map=$(MAP) -print-memory-usage
ASFLAGS=-Felf -m$(CPU) -Lnf -nowarn=2028 -quiet $(DEFINES)
LIBS = -L$(CROSSDIR)/m68k-elf/lib/m$(CPU) -lm -L$(CROSSDIR)/lib/gcc/m68k-elf/$(GCC_VERSION)/m$(CPU) -lgcc

CINC_FILES = $(wildcard *.h)

ROMADDR = 0xF80000
OBJECTS= startup.o main.o

include system/include.mk
include drivers/include.mk
include libraries/include.mk
include libraries/fatfs/include.mk
include shell/include.mk
include stdlib/include.mk

%.o : %.c
	$(CC) -c $(CFLAGS) $(EXTRA_CFLAGS) -o $@ $<

%.o : %.S
	$(AS) $(ASFLAGS) -o $@ $<

%.o : %.asm
	$(AS) $(ASFLAGS) -o $@ $<

$(BINARY) : $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@ $(LIBS)

$(BINARY_EVEN): $(BINARY)
	$(OBJCOPY) -b 0 -i 2 --interleave-width=1 -I binary -O binary $(BINARY) $(BINARY_EVEN)

$(BINARY_ODD): $(BINARY)
	$(OBJCOPY) -b 1 -i 2 --interleave-width=1 -I binary -O binary $(BINARY) $(BINARY_ODD)

.PHONY: all clean dump tools

all: $(BINARY_EVEN) $(BINARY_ODD)

clean:
ifeq ($(OS),Windows_NT)
	cmd /c del /q /f *.lst
	cmd /c del /s /q /f *.o
	cmd /c del /q /f *.map
	cmd /c del /q /f *.hex
	cmd /c del /q /f *.bin
	cmd /c del /q /f *.elf
else
	rm -f *.lst *.LST
	find . -name '*.o' -delete
	rm -f *.a
	rm -f *.map
	rm -f *.hex
	rm -f *.bin *.BIN
	rm -f *.elf
endif
