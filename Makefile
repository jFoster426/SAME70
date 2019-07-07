.PHONY: all directory clean size

CC = arm-none-eabi-gcc
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size

CFLAGS += -W -Wall --std=gnu99 -O0
CFLAGS += -fno-diagnostics-show-caret
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -mcpu=cortex-m7 -mthumb
CFLAGS += -mfloat-abi=hard -mfpu=fpv5-d16
CFLAGS += -MD -MP -MT build/$(*F).o -MF build/$(@F).d

LDFLAGS += -mcpu=cortex-m7 -mthumb
LDFLAGS += -mfloat-abi=hard -mfpu=fpv5-d16
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--script=same70q21.ld

INCLUDES += -I include
INCLUDES += -I lib

DEFINES += -D DONT_USE_CMSIS_INIT
DEFINES += -D F_CPU=300000000
DEFINES += -D F_PCLK=150000000
DEFINES += -D __DEBUG__


CFLAGS += $(INCLUDES) $(DEFINES)

all: clean build/program.elf build/program.hex build/program.bin size build/objdump.asm

build/objdump.asm: build/program.elf
	@arm-none-eabi-objdump -D build/program.elf > build/objdump.asm

build/program.elf: startup_same70.o build/gpio.o build/uart.o build/spi.o build/nrf.o main.o
	@$(CC) $(LDFLAGS) build/startup_same70.o build/gpio.o build/uart.o build/spi.o build/nrf.o build/main.o -o build/program.elf

build/program.hex: build/program.elf
	@$(OBJCOPY) -O ihex build/program.elf build/program.hex

build/program.bin: build/program.elf
	@$(OBJCOPY) -O binary build/program.elf build/program.bin

startup_same70.o:
	@$(CC) $(CFLAGS) -c startup_same70.c -o build/startup_same70.o

main.o: main.c
	@$(CC) $(CFLAGS) -c main.c -o build/main.o

build/gpio.o: lib/gpio/gpio.c
	@$(CC) $(CFLAGS) -c lib/gpio/gpio.c -o build/gpio.o

build/uart.o: lib/uart/uart.c
	@$(CC) $(CFLAGS) -c lib/uart/uart.c -o build/uart.o

build/spi.o: lib/spi/spi.c
	@$(CC) $(CFLAGS) -c lib/spi/spi.c -o build/spi.o

build/nrf.o: lib/nrf/nrf.c
	@$(CC) $(CFLAGS) -c lib/nrf/nrf.c -o build/nrf.o

size: build/program.elf
	@echo.
	@$(SIZE) -t $^

clean:
	@del /q build\\*.*
