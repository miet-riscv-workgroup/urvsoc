# and don't touch the rest unless you know what you're doing.
CROSS_COMPILE ?= /opt/riscv32i/bin/riscv32-unknown-elf-

CC =		$(CROSS_COMPILE)gcc
LD =		$(CROSS_COMPILE)ld
OBJDUMP =	$(CROSS_COMPILE)objdump
OBJCOPY =	$(CROSS_COMPILE)objcopy
SIZE =		$(CROSS_COMPILE)size

SYN_DIR=../../syn/spec
SYN_TOP_LEVEL=spec_top

ARCH = rv32i

# keep RV32I: the bootloader also emulates DIV/MULH instuctions

CFLAGS = -g -O2 -march=$(ARCH) -I. -I../common -ffunction-sections -fdata-sections
OBJS = ../common/crtboot.o  boot.o ../common/uart/uart.o ../common/emulate.o ../common/irq.o
LDS = ../common/boot.ld
OUTPUT=uart-bootloader

$(OUTPUT): $(LDS) $(OBJS)
	${CC} -flto -O2 -Wl,--gc-sections -g  -march=$(ARCH) -o $(OUTPUT).elf -nostartfiles $(OBJS)  -lm -T $(LDS) -lc
	${OBJCOPY} -O binary $(OUTPUT).elf $(OUTPUT).bin
	${OBJDUMP} -D $(OUTPUT).elf > disasm.S
	python3 ../gendatinit.py  $(OUTPUT).bin ../$(OUTPUT).dat

	$(SIZE) $(OUTPUT).elf
clean:
	rm -f $(OUTPUT).elf $(OUTPUT).bin $(OBJS)

%.o:	%.S
	${CC} -c -march=$(ARCH) $^ -o $@