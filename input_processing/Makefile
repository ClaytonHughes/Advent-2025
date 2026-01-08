CC = gcc
CC_FLAGS = -g -Wall -Wextra -Werror $(CFLAGS)

C_SOURCES := $(wildcard *.c)
EXES := $(patsubst %.c, bin/%, $(C_SOURCES))

.PHONY: clean

all: $(EXES)

bin/%:	%.c
	@mkdir -p bin
	$(CC) $(CC_FLAGS) -o $@ $<

clean:
	rm -rf ./bin