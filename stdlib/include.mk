C_FILES = $(wildcard stdlib/src/*.c)

OBJECTS := $(OBJECTS) $(C_FILES:.c=.o)
EXTRA_CFLAGS := $(EXTRA_CFLAGS) -Istdlib/include -Istdlib/src/templates
