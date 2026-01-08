ASSEMBLER = rgbdev
AS_FLAGS = -Wall -Werror $(AFLAGS)
LINKER = rgblink
LN_FLAGS = $(LFLAGS)
ROMFIX = rgbfix
RF_FLAGS = -p 0xff -v $(RFLAGS)

ASM_SOURCES := $(wildcard day*.asm)
ROMS := $(patsubst roms/%.gb, %, $(ASM_SOURCES))

.PHONY: clean dbg setup

dbg:
	@echo '$(ROMS)'

all: $(ROMS)

obj/%.o: %.asm
	@mkdir -p obj
	$(ASSEMBLER) $(AS_FLAGS) -o $@ -- $< 

obj/%.rom: obj/%.o
	$(LINKER) $(LN_FLAGS) -o $@ -m $@.map -- $<

roms/%.gb: obj/%.rom
	@mkdir -p roms
	$(ROMFIX) $(RF_FLAGS) -o $@ 

setup: rgbdev
rgbdev:
	wget --quiet https://github.com/gbdev/rgbds/releases/latest/download/rgbds-linux-x86_64.tar.xz
	@mkdir -p rgbdev
	@tar xf rgbds-linux-x86_64.tar.xz -C rgbdev
# grumble grumble, ./install.sh expects the files to be in the current directory.
	@cd rgbdev && chmod a+x ./install.sh && sudo ./install.sh
	@rgbasm -V
	@rm -r rgbdev
	@rm rgbds-linux-x86_64.tar.xz


clean:
	rm -rf ./obj ./roms
