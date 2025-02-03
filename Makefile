
sources := $(shell find src/ -name "*.c")
sources += $(shell find third_party/ -name "*.c")
objects := $(sources:.c=.o)
target := build/icraft.out

CFLAGS := -lSDL2 -lm -lGL
CFLAGS += -g -ggdb -rdynamic
CFLAGS += -Werror -Wall

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
