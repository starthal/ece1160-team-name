BOARD:=disc

ifeq ($(BOARD),disc)
	OBJDIR:=disc-obj
	CHECK:=true
else
	ifeq ($(BOARD),kick)
		OBJDIR:=kick-obj
		CHECK:=true
	else
		ifeq ($(BOARD),prod)
			OBJDIR:=prod-obj
			CHECK:=true
		else
			OBJDIR:=NO_BOARD
			CHECK:=echo "No Board"; false
		endif
	endif
endif

TARGET:=main

ifeq ($(USER),josh)
    TOOLCHAIN_PATH:=/home/josh/CodeSourcery
else
    TOOLCHAIN_PATH:=/opt/sourcery/bin
endif

TOOLCHAIN_PREFIX:=arm-none-eabi

OPTLVL:=fast # Optimization level, can be [0, 1, 2, 3, s].

#PROJECT_NAME:=$(notdir $(lastword $(CURDIR)))
TOP:=$(shell readlink -f "..")
STMLIB:=$(TOP)/lib
STD_PERIPH:=$(STMLIB)/periph
STARTUP:=$(STMLIB)/startup
LINKER_SCRIPT:=$(STMLIB)/ld/stm32_flash.ld
LIBC:=/usr/arm-linux-gnueabi/lib/libc.a
#LIBC:=/opt/sourcery/arm-none-eabi/lib/libc.a

INCLUDE=-I$(CURDIR)
INCLUDE+=-I$(STMLIB)/core
INCLUDE+=-I$(STMLIB)/stm32f4/inc
INCLUDE+=-I$(STD_PERIPH)/inc
#INCLUDE+=-I$(STMLIB)/STM32_USB_OTG_Driver/inc
#INCLUDE+=-I$(STMLIB)/STM32_USB_Device_Library/Class/cdc/inc
#INCLUDE+=-I$(STMLIB)/STM32_USB_Device_Library/Core/inc

# vpath is used so object files are written to the current directory instead
# of the same directory as their source files
vpath %.c $(STD_PERIPH)/src \
	$(STMLIB)/stm32f4/src \
	$(STMLIB)/STM32_USB_OTG_Driver/src \
	$(STMLIB)/STM32_USB_Device_Library/Class/cdc/src \
	$(STMLIB)/STM32_USB_Device_Library/Core/src \
	$(OBJDIR)
vpath %.s $(STARTUP) \
	$(OBJDIR)

ASRC=startup_stm32f4xx.s

# Project Source Files
SRC+=main.c
SRC+=stm32f4xx_it.c
SRC+=system_stm32f4xx.c
SRC+=lcd.c
#SRC+=usart.c
#SRC+=rf.c
#SRC+=bluetooth.c
#SRC+=zigbee.c
#SRC+=w_utils.c
#SRC+=host.c
#SRC+=client.c
#SRC+=usb_bsp.c
#SRC+=usbd_desc.c
#SRC+=usbd_usr.c
#SRC+=vcp.c

#Newlib stubs for printf, etc
#SRC+=newlib_stubs.c

# Standard Peripheral Source Files
SRC+=stm32f4xx_syscfg.c
SRC+=stm32f4xx_gpio.c
SRC+=stm32f4xx_rcc.c
SRC+=stm32f4xx_usart.c
SRC+=misc.c
#SRC+=stm32f4xx_flash.c
#SRC+=stm32f4xx_adc.c
#SRC+=stm32f4xx_dac.c
#SRC+=stm32f4xx_dma.c
#SRC+=stm32f4xx_exti.c
SRC+=stm32f4xx_i2c.c
#SRC+=stm32f4xx_spi.c
#SRC+=stm32f4xx_tim.c
#SRC+=drv_glcd.c
#SRC+=glcd_ll.c
#SRC+=Terminal_9_12x6.c

# USB Source Files
#SRC+=usb_dcd_int.c
#SRC+=usb_core.c
#SRC+=usb_dcd.c
#SRC+=usbd_cdc_core.c
#SRC+=usbd_req.c
#SRC+=usbd_core.c
#SRC+=usbd_ioreq.c

CDEFS=-DUSE_STDPERIPH_DRIVER
CDEFS+=-DSTM32F4XX
CDEFS+=-DUSE_USB_OTG_FS

ifeq ($(BOARD),disc)
	CDEFS+=-DHSE_VALUE=8000000
	CDEFS+=-DBOARD_DISC
else
	ifeq ($(BOARD),kick)
		CDEFS+=-DHSE_VALUE=25000000
		CDEFS+=-DBOARD_KICK
	else
		ifeq ($(BOARD),prod)
			CDEFS+=-DHSE_VALUE=25000000
			CDEFS+=-DBOARD_PROD
		endif
	endif
endif

MCUFLAGS=-mcpu=cortex-m4 -mthumb
#MCUFLAGS=-mcpu=cortex-m4 -mthumb -mlittle-endian -mfpu=fpa -mfloat-abi=hard -mthumb-interwork
#MCUFLAGS=-mcpu=cortex-m4 -mfpu=vfpv4-sp-d16 -mfloat-abi=hard

COMMONFLAGS=-O$(OPTLVL) -g -Wall
CFLAGS=$(COMMONFLAGS) $(MCUFLAGS) $(INCLUDE) $(CDEFS)

LDLIBS=
LDFLAGS=$(COMMONFLAGS) -fno-exceptions -ffunction-sections -fdata-sections \
        -nostartfiles -Wl,--gc-sections,-T$(LINKER_SCRIPT)

#####
#####

#OBJ = $(SRC:%.c=%.o) $(ASRC:%.s=%.o)
OBJ := $(addprefix $(OBJDIR)/,$(SRC:.c=.o)) $(addprefix $(OBJDIR)/,$(ASRC:.s=.o))

CC=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc
LD=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-objcopy
AS=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-as
AR=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-ar
GDB=$(TOOLCHAIN_PATH)/$(TOOLCHAIN_PREFIX)-gdb

all: $(OBJ)
	@$(CHECK)
	$(CC) -o $(TARGET)_$(BOARD).elf $(LDFLAGS) $(OBJ) $(LDLIBS)
	$(OBJCOPY) -O ihex $(TARGET)_$(BOARD).elf $(TARGET)_$(BOARD).hex
	
$(OBJDIR)/%.o: %.c
	@$(CHECK)
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	
$(OBJDIR)/%.o: %.s
	@$(CHECK)
	$(COMPILE.c) $(OUTPUT_OPTION) $<

.PHONY: clean

clean:
	$(CHECK)
	rm -f $(OBJ)
	rm -f $(TARGET)_$(BOARD).elf
	rm -f $(TARGET)_$(BOARD).hex
	rm -f $(TARGET)_$(BOARD).bin
	rm -f $(OBJDIR)/*.o

# ************************************************
# FLASH PROGRAMMING
#
# Alternate make target for flash programming only
# ************************************************

# specify OpenOCD
OPENOCD = openocd

# specify OpenOCD configuration file
OPENOCD_CFG_PREFIX = ../openocd

# program the AT91SAM7S256 internal flash memory
program: all
	$(CHECK)
	@echo "Flashing the $(BOARD) board..."
	$(OPENOCD) -f $(OPENOCD_CFG_PREFIX)_$(BOARD).cfg -f $(OPENOCD_CFG_PREFIX)_write.cfg
	@echo "Done."

#debug: Program but don't shut down openOCD
debug: all
	$(CHECK)
	@echo "[DEBUG MODE] Flashing the $(BOARD) board..."
	@echo "[DEBUG MODE] Connect with gdb:"
	@echo "	              tar rem localhost:3333"
	@echo "	              monitor reset halt"
	@echo "               load"
	$(OPENOCD) -f $(OPENOCD_CFG_PREFIX)_$(BOARD).cfg -f $(OPENOCD_CFG_PREFIX)_debug.cfg
