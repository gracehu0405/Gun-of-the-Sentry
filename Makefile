NAME = main

# This is the list of your modules that will be used when
# buliding the console application and libmypi.a library
# To be considered for system bonus, MY_MODULES must name all of your modules
# If you have unresolved issues with a module, you can remove and the
# reference module will be used instead (no system bonus, though)
#
# *** Be sure MY_MODULES is set correctly when you submit! ***
MY_MODULES = ultrasound.o sensormap.o gun.o shell.o graphics.o
#= shoot.o
#timer.o gpio.o strings.o printf.o backtrace.o malloc.o keyboard.o shell.o fb.o gl.o console.o gprof.o

CFLAGS  = -I$(CS107E)/include -g -Wall -Wpointer-arith
CFLAGS += -Og -std=c99 -ffreestanding
CFLAGS += -mapcs-frame -fno-omit-frame-pointer -mpoke-function-name
LDFLAGS = -nostdlib -T memmap  -L$(CS107E)/lib
LDLIBS  = -lpi -lgcc

all : $(NAME).bin $(MY_MODULES)

#lib: libmypi.a

%.bin: %.elf
	arm-none-eabi-objcopy $< -O binary $@

%.elf: %.o $(MY_MODULES) start.o cstart.o
	arm-none-eabi-gcc $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	arm-none-eabi-gcc $(CFLAGS) -c $< -o $@

%.o: %.s
	arm-none-eabi-as $(ASFLAGS) $< -o $@

#libmypi.a: $(MY_MODULES) Makefile
#	rm -f $@
#	arm-none-eabi-ar cDr $@ $(filter %.o,$^)

%.list: %.o
	arm-none-eabi-objdump --no-show-raw-insn -d $< > $@

install: $(NAME).bin
	rpi-install.py -p $<

test: tests/graphics_tests.c
	rpi-install.py -p $<

clean:
	rm -f *.o *.bin *.elf *.list *~ libmypi.a
	#rm -f apps/*.o apps/*.bin apps/*.elf apps/*.list apps/*~
	rm -f tests/*.o tests/*.bin tests/*.elf tests/*.list tests/*~

.PHONY: all clean install test

.PRECIOUS: %.elf %.o %.a

# empty recipe used to disable built-in rules for native build
%:%.c
%:%.o

define CS107E_ERROR_MESSAGE
ERROR - CS107E environment variable is not set.

Please set it to point to the `cs107e.github.io/cs107e` directory using the
command `export CS107E=<replace with path to your cs107e.github.io directory>/cs107e`.

To have this done automatically, add the above command to your shell
environment configuration file (e.g. ~/.bashrc)
endef

ifndef CS107E
$(error $(CS107E_ERROR_MESSAGE))
endif
