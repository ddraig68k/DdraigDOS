CPU = 68000
TARGET = ddriag

CDEFS = -DCPU=$(CPU)
ADEFS = --defsym CPU=$(CPU)

ifeq ($(OS),Windows_NT)
	CROSSDIR = C:/dev/lang/m68k-elf
	RM = cmd /c del /q /f
else
	CROSSDIR = /opt/m68k-elf
	RM = rm
endif
BIN = $(CROSSDIR)/bin

WARNDIS = -Wno-unused-but-set-variable -Wno-unused-function

CC = $(BIN)/m68k-elf-gcc
GCC_VERSION = $(shell $(CC) -dumpversion)
XCOPT = -Os -I. -std=c99 -m$(CPU) -Wall -fomit-frame-pointer $(WARNDIS) $(CDEFS)
#XCOPT = -Os -I../includes -std=c99 -m$(CPU) -Wall -Werror -fomit-frame-pointer $(WARNDIS) $(CDEFS)
COPT = -I. -m$(CPU) $(CDEFS)
AS = $(BIN)/m68k-elf-as
AOPT = -m$(CPU) -alhms $(ADEFS)
LD = $(BIN)/m68k-elf-ld
LOPT =  -print-memory-usage
LIB  = $(BIN)/m68k-elf-ar
LIBS = -L$(CROSSDIR)/m68k-elf/lib/m$(CPU) -lm -Tddraig.ld -L$(CROSSDIR)/lib/gcc/m68k-elf/$(GCC_VERSION)/m$(CPU) -lgcc
OBJCOPY = $(BIN)/m68k-elf-objcopy
VASM = $(BIN)/vasmm68k_mot

ASM_FILES = $(wildcard *.asm)
C_FILES = $(wildcard *.c)
BIN_FILES = $(patsubst %.asm, %.s68, $(ASM_FILES))
CBIN_FILES = $(patsubst %.c, %.s68, $(C_FILES))
INC_FILES = $(wildcard ../includes/*.inc)
CINC_FILES = $(wildcard ../includes/*.h)

.SUFFIXES: .c .s .o. .elf .hex .bin

.s.o:
	$(AS) $(AOPT) -a=$*.lst -o $*.o $*.s

all: $(BIN_FILES) $(CBIN_FILES)

%.s68: %.elf $(BIN_FILES)
	$(OBJCOPY) -S $< -O srec $(patsubst %.elf, %.s68, $@)

%.elf: %.c $(CINC_FILES)
	$(CC) -S $(COPT) -fverbose-asm -o $(patsubst %.elf, %.s, $@) $<
	$(AS) $(AOPT) -a=$*.lst -o $(patsubst %.elf, %.o, $@) $(patsubst %.elf, %.s, $@)
#	$(CC) -c $(COPT) -o $(patsubst %.elf, %.o, $@) $<
	$(LD) $(LOPT) -o $@ $(patsubst %.c, %.o, $<) $(LIBS)

#	@srec_cat $@ -offset -0x002000 -o $(patsubst %.s68, %.bin, $@) -binary
# Build the code and create a binary file
%.s68: %.asm $(INC_FILES)
	$(VASM) -I../includes -o $(patsubst %.asm, %.s68, $@) -L $(patsubst %.s68, %.lst, $@) -s28 -Fsrec $<

#	@srec_cat $@ -offset -0x002000 -o $(patsubst %.s68, %.bin, $@) -binary


clean:
	@$(RM) *.s68
	@$(RM) *.elf
	@$(RM) *.lst
	@$(RM) *.o
