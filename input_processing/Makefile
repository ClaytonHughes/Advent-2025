CC = gcc
CC_FLAGS = -g -Wall -Wextra -Werror

C_SOURCES := $(wildcard *.c)
EXES := $(patsubst %.c, %, $(C_SOURCES))

.PHONY: clean

all: $(EXES)

%:	%.c
	$(CC) $(CC_FLAGS) -o $@ $<

clean:
	rm $(EXES)