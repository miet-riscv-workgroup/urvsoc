# and don't touch the rest unless you know what you're doing.
CROSS_COMPILE ?= /opt/riscv32i/bin/riscv32-unknown-elf-

CC =		$(CROSS_COMPILE)gcc
LD =		$(CROSS_COMPILE)ld
OBJDUMP =	$(CROSS_COMPILE)objdump
OBJCOPY =	$(CROSS_COMPILE)objcopy
SIZE =		$(CROSS_COMPILE)size

ARCH = rv32i
OBJS = ../common/crt_irq.o main.o ../common/uart.o ../common/spi/wb_spi.o ../common/printf.o ../common/vsprintf-xint.o ../common/irq_ext.o ../common/irq/irq_c.o

LDS = ../common/user.ld
LDS_SIM = ../common/user_sim.ld
OUTPUT=demo
OUTPUT_SIM=$(OUTPUT)_sim

$(OUTPUT): $(LDS) $(OBJS) setup sim

setup :	
	${CC} -flto -O2 -Wl,--gc-sections -march=$(ARCH) -o $(OUTPUT).elf -nostartfiles $(OBJS)  -lm -T $(LDS) -lc
	${OBJCOPY} -O binary $(OUTPUT).elf $(OUTPUT).bin
	${OBJDUMP} -D $(OUTPUT).elf > disasm.S
	python3 ../gendatinit_sim.py  $(OUTPUT).bin ../$(OUTPUT).dat	
	$(SIZE) $(OUTPUT).elf

sim: 
	${CC} -flto -O2 -Wl,--gc-sections -march=$(ARCH) -o $(OUTPUT_SIM).elf -nostartfiles $(OBJS)  -lm -T $(LDS_SIM) -lc
	${OBJCOPY} -O binary $(OUTPUT_SIM).elf $(OUTPUT_SIM).bin
	${OBJDUMP} -D $(OUTPUT_SIM).elf > disasm_sim.S
	python3 ../gendatinit.py  $(OUTPUT_SIM).bin ../$(OUTPUT_SIM).dat	

clean:
	rm -f $(OUTPUT).elf $(OUTPUT).bin $(OBJS)
	rm -f $(OUTPUT_SIM).elf $(OUTPUT_SIM).bin $(OBJS)

boot:	
	python3 ../uart-bootloader.py  $(OUTPUT).bin	

%.o:	%.S
	${CC} -c -march=$(ARCH) $^ -o $@