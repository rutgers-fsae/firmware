PROJECT := stm32f103-can-bootloader
BUILD := build
CUBE := ../rfr26-tempSensor
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

CFLAGS := -mcpu=cortex-m3 -mthumb -std=gnu11 -Os -ffunction-sections -fdata-sections -Wall -Wextra -Werror \
  -DSTM32F103xB -DUSE_HAL_DRIVER -DHSE_VALUE=12000000U -Iinclude -I$(CUBE)/Core/Inc \
  -I$(CUBE)/Drivers/STM32F1xx_HAL_Driver/Inc -I$(CUBE)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
  -I$(CUBE)/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I$(CUBE)/Drivers/CMSIS/Include
LDFLAGS := -mcpu=cortex-m3 -mthumb -nostdlib -TSTM32F103T8UX_BOOTLOADER.ld -Wl,--gc-sections -Wl,-Map=$(BUILD)/$(PROJECT).map -lgcc
SOURCES := src/main.c $(CUBE)/Core/Src/system_stm32f1xx.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
  $(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
VPATH := src:$(CUBE)/Core/Src:$(CUBE)/Drivers/STM32F1xx_HAL_Driver/Src
OBJECTS := $(addprefix $(BUILD)/,$(notdir $(SOURCES:.c=.o))) $(BUILD)/startup.o

.PHONY: all clean
all: $(BUILD)/$(PROJECT).bin

$(BUILD)/$(PROJECT).elf: $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD)/$(PROJECT).bin: $(BUILD)/$(PROJECT).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup.o: $(CUBE)/Core/Startup/startup_stm32f103t8ux.s
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD)
