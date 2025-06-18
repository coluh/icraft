
sources := $(shell find src/ -name "*.c")
sources += $(shell find third_party/ -name "*.c")
objects := $(sources:.c=.o)
target := build/icraft.out

CFLAGS := -lSDL2 -lm -lGL -lfreetype
CFLAGS += -I/usr/include/freetype2
CFLAGS += -g -ggdb -rdynamic
CFLAGS += -Werror -Wall
CFLAGS += -pg

.PHONY: all run test clean
all: $(target)

%.o: %.c
	gcc -c $< -o $@ $(CFLAGS)

$(target): $(objects)
	@mkdir -p build/
	gcc $^ -o $@ $(CFLAGS)

run: $(target)
	./$(target)

clean:
	rm -f $(objects) $(target)
