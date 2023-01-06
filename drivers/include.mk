OBJECTS := $(OBJECTS) drivers/diskio.o drivers/display.o drivers/expansion.o drivers/keyboard.o

include drivers/audio/include.mk
include drivers/network/include.mk
include drivers/sys/include.mk
include drivers/video/include.mk

