include ../../py/mkenv.mk

# ----- Local definitions
MCUX = $(HOME)/work/SDK_2.2_FRDM-K64F
BOARD = frdmk64f
DEVICE = MK64F12
DEFS += -DCPU_MK64FN1M0VLL12 -DFRDM_K64F -DFREEDOM
LINKERSCRIPT = $(wildcard $(MCUX)/devices/$(DEVICE)/gcc/*_flash.ld)
CPUFLAGS = -mcpu=cortex-m4 -mthumb
FPUFLAGS = -mfloat-abi=hard -mfpu=fpv4-sp-d16
#DEPFLAGS = -MMD -MP
ABIFLAGS = -fno-common -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mapcs
# -----

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include $(TOP)/py/py.mk

CROSS_COMPILE = arm-none-eabi-

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)
INC += -I$(MCUX)/devices/$(DEVICE)/drivers
INC += -I$(MCUX)/devices/$(DEVICE)
INC += -I$(MCUX)/CMSIS/Include

FSL_MOD += \
	fsl_clock.o \
	fsl_sim.o \
	fsl_uart.o \

FSL_O = $(addprefix $(BUILD)/, $(FSL_MOD))

DFU = $(TOP)/tools/dfu.py
PYDFU = $(TOP)/tools/pydfu.py
CFLAGS_CORTEX_M4 = -mthumb -mtune=cortex-m4 -mabi=aapcs-linux -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -fsingle-precision-constant -Wdouble-promotion
#CFLAGS = $(DEFS) $(INC) -Wall -Werror -std=c99 -nostdlib $(CFLAGS_CORTEX_M4) $(COPT)
CFLAGS = $(DEFS) $(INC) -Wall -Werror -std=c99 $(PLATDEFS) $(CPUFLAGS) $(FPUFLAGS) $(DEPFLAGS) $(ABIFLAGS) $(COPT)
LDFLAGS += $(CPUFLAGS) $(FPUFLAGS) $(ABIFLAGS)
LDFLAGS += -flto
LDFLAGS += --specs=nano.specs --specs=nosys.specs
LDFLAGS += -T$(LINKERSCRIPT)
LDFLAGS += $(addprefix -Xlinker ,--gc-sections -static -z muldefs)

# Tune for Debugging or Optimization
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -ggdb
else
CFLAGS += -Os -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections
endif

LIBS = -lm -lc

# Files using QSTR
SRC_MOD += modmachine.c modk64f.c mreg.c

SRC_C = \
	main.c \
	modk64f.c \
	modmachine.c \
	mreg.c \
	uart_core.c \
	lib/utils/stdout_helpers.c \
	lib/utils/pyexec.c \
	lib/libc/string0.c \
	lib/mp-readline/readline.c \
	$(BUILD)/_frozen_mpy.c \
	system_$(DEVICE).c

SRC_S = \
	startup_$(DEVICE).S \

OBJ = $(PY_O) \
	$(FSL_O) \
	$(addprefix $(BUILD)/, $(SRC_C:.c=.o)) \
	$(addprefix $(BUILD)/, $(SRC_S:.S=.o))


all: $(BUILD)/firmware.bin

$(BUILD)/_frozen_mpy.c: frozentest.mpy $(BUILD)/genhdr/qstrdefs.generated.h
	$(ECHO) "MISC freezing bytecode"
	$(Q)$(TOP)/tools/mpy-tool.py -f -q $(BUILD)/genhdr/qstrdefs.preprocessed.h -mlongint-impl=none $< > $@

$(BUILD)/firmware.elf: $(OBJ)
	$(ECHO) "LINK $@"
#	$(Q)$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	$(Q)$(SIZE) $@

$(BUILD)/firmware.bin: $(BUILD)/firmware.elf
	$(Q)$(OBJCOPY) -O binary $^ $(BUILD)/firmware.bin

include $(TOP)/py/mkrules.mk

vpath %.S $(MCUX)/devices/$(DEVICE)/gcc $(MCUX)/devices/$(DEVICE)
$(BUILD)/%.o: %.S
	$(ECHO) "CC $<"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

vpath %.c $(MCUX)/devices/$(DEVICE)/gcc $(MCUX)/devices/$(DEVICE) $(MCUX)/devices/$(DEVICE)/drivers
$(BUILD)/%.o: %.c
	$(call compile_c)
