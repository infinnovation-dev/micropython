include ../py/mkenv.mk

CROSS = 1

# Path to clone of mbedmicro/mbed git repo
MBED_REPO = $(HOME)/mbed/mbed
MBED_HAL = $(MBED_REPO)/libraries/mbed/targets/hal
# Targets to consider when generating mbedpins.h
PINS_TARGETS = \
	K64F \
	KL25Z \
	LPC1768 \
	NUCLEO_F401RE \

# PINS_PATH_$(TARGET) is the relative path within the mbed tree
# where the relevant PinNames.h file can be found.
# Often related to extra_labels in workspace_tools/targets.py
PINS_PATH_KL25Z = TARGET_Freescale/TARGET_KLXX/TARGET_KL25Z
PINS_PATH_K64F = TARGET_Freescale/TARGET_KPSDK_MCUS/TARGET_MCU_K64F/TARGET_FRDM
PINS_PATH_LPC1768 = TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768
PINS_PATH_NUCLEO_F401RE = TARGET_STM/TARGET_STM32F4/TARGET_NUCLEO_F401RE

mbedpins.h:	makembedpins.py
	python makembedpins.py -d $(foreach t,$(PINS_TARGETS),$(t):$(MBED_HAL)/$(PINS_PATH_$(t))/PinNames.h) > $@

qstrdefscond.h:	makeqstrcond.py mbedpins.h
	python makeqstrcond.py \
		../py/qstrdefs.h \
		../py/*.c \
		../extmod/machine_mem.c \
		../lib/utils/pyexec.c \
		../lib/utils/pyhelp.c \
		../lib/mp-readline/readline.c \
		qstrdefsport.h \
		main.cpp \
		help.c \
		modmachine.c \
		modmbed.c \
		modpins.c \
		mbedpins.h \
		> $@

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h

# include py core make definitions
include ../py/py.mk
