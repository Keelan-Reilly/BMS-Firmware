PROJECT := firmware
BUILD_DIR := build

CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

MCUFLAGS := -mcpu=cortex-m4 -mthumb -mfloat-abi=soft
OPTFLAGS := -Og -g3 -ffunction-sections -fdata-sections
WARNFLAGS := -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers
CSTD := -std=gnu99

DEFS := -DUSE_HAL_DRIVER -DSTM32F303xC

INCLUDES := \
	-IMain \
	-IModules/Inc \
	-IDrivers/HWDrivers/Inc \
	-IDrivers/SWDrivers/Inc \
	-ILibraries/Inc \
	-IDevice \
	-ICubeMX/Inc \
	-IDrivers/STM32F3xx_HAL_Driver/Inc \
	-IDrivers/STM32F3xx_HAL_Driver/Inc/Legacy \
	-IDrivers/CMSIS/Include \
	-IDrivers/CMSIS/Device/ST/STM32F3xx/Include

CFLAGS := $(MCUFLAGS) $(OPTFLAGS) $(WARNFLAGS) $(CSTD) $(DEFS) $(INCLUDES) -MMD -MP
ASFLAGS := $(MCUFLAGS) -x assembler-with-cpp $(DEFS) $(INCLUDES)

LDSCRIPT := gcc/STM32F303CC_APP_FLASH.ld
LDFLAGS := $(MCUFLAGS) -T$(LDSCRIPT) --specs=nano.specs --specs=nosys.specs \
	-Wl,-Map,$(BUILD_DIR)/$(PROJECT).map,--cref -Wl,--gc-sections
LDLIBS := -lm

CORE_SRCS := \
	Main/main.c \
	Device/stm32f3xx_it.c \
	Device/stm32f3xx_hal_msp.c \
	CubeMX/Src/system_stm32f3xx.c

HAL_SRCS := \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_adc_ex.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_can.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_cortex.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_crc.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_crc_ex.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_dma.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_flash_ex.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_gpio.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_i2c_ex.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_iwdg.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_pwr_ex.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_rcc_ex.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_spi.c \
	Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_hal_uart.c

HW_SRCS := \
	Drivers/HWDrivers/Src/driverHWADC.c \
	Drivers/HWDrivers/Src/driverHWEEPROM.c \
	Drivers/HWDrivers/Src/driverHWI2C1.c \
	Drivers/HWDrivers/Src/driverHWI2C2.c \
	Drivers/HWDrivers/Src/driverHWPowerState.c \
	Drivers/HWDrivers/Src/driverHWSPI1.c \
	Drivers/HWDrivers/Src/driverHWStatus.c \
	Drivers/HWDrivers/Src/driverHWSwitches.c \
	Drivers/HWDrivers/Src/driverHWUART2.c

SW_SRCS := \
	Drivers/SWDrivers/Src/driverSWADC128D818.c \
	Drivers/SWDrivers/Src/driverSWDCDC.c \
	Drivers/SWDrivers/Src/driverSWEMC2305.c \
	Drivers/SWDrivers/Src/driverSWISL28022.c \
	Drivers/SWDrivers/Src/driverSWLTC6812.c \
	Drivers/SWDrivers/Src/driverSWPCAL6416.c \
	Drivers/SWDrivers/Src/driverSWSHT21.c \
	Drivers/SWDrivers/Src/driverSWSSD1306.c \
	Drivers/SWDrivers/Src/driverSWStorageManager.c \
	Drivers/SWDrivers/Src/driverSWUART2.c

MODULE_SRCS := \
	Modules/Src/modCAN.c \
	Modules/Src/modCommands.c \
	Modules/Src/modConfig.c \
	Modules/Src/modDelay.c \
	Modules/Src/modDisplay.c \
	Modules/Src/modEffect.c \
	Modules/Src/modFlash.c \
	Modules/Src/modHiAmp.c \
	Modules/Src/modOperationalState.c \
	Modules/Src/modPowerElectronics.c \
	Modules/Src/modPowerState.c \
	Modules/Src/modStateOfCharge.c \
	Modules/Src/modTerminal.c \
	Modules/Src/modUART.c

LIB_SRCS := \
	Libraries/Scr/libBuffer.c \
	Libraries/Scr/libCRC.c \
	Libraries/Scr/libFileStream.c \
	Libraries/Scr/libGLCDFont.c \
	Libraries/Scr/libGraphics.c \
	Libraries/Scr/libLogos.c \
	Libraries/Scr/libPacket.c \
	Libraries/Scr/libRingbuffer.c

ASM_SRCS := \
	Drivers/CMSIS/Device/ST/STM32F3xx/Source/Templates/gcc/startup_stm32f303xc.s

SRCS := $(CORE_SRCS) $(HAL_SRCS) $(HW_SRCS) $(SW_SRCS) $(MODULE_SRCS) $(LIB_SRCS)
OBJS := $(addprefix $(BUILD_DIR)/,$(SRCS:.c=.o)) $(addprefix $(BUILD_DIR)/,$(ASM_SRCS:.s=.o))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean size

all: $(BUILD_DIR)/$(PROJECT).elf $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).bin size

$(BUILD_DIR)/$(PROJECT).elf: $(OBJS) $(LDSCRIPT)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $@

$(BUILD_DIR)/$(PROJECT).hex: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $< $@

$(BUILD_DIR)/$(PROJECT).bin: $(BUILD_DIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

size: $(BUILD_DIR)/$(PROJECT).elf
	$(SIZE) $<

$(BUILD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(dir $@)
	$(CC) $(ASFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
