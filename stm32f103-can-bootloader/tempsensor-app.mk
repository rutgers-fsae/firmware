# Build rfr26-tempSensor to run behind the CAN bootloader (linked at 0x08004000, 47 KiB).
# Usage, from this directory: make -f tempsensor-app.mk
#
# The application sources are not modified. The linker script is derived from the
# application's own script, and the bootloader sets the vector-table offset before it
# jumps, so the application needs no VECT_TAB_OFFSET of its own.
APP := ../rfr26-tempSensor
BUILD := build/tempsensor-app
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size

# Same flags as the STM32CubeIDE Release configuration.
ARCH := -mcpu=cortex-m3 -mthumb -mfloat-abi=soft --specs=nano.specs
CFLAGS := $(ARCH) -std=gnu11 -Os -ffunction-sections -fdata-sections -Wall \
  -DUSE_HAL_DRIVER -DSTM32F103xB -DHSE_VALUE=12000000U -I$(APP)/Core/Inc \
  -I$(APP)/Drivers/STM32F1xx_HAL_Driver/Inc -I$(APP)/Drivers/STM32F1xx_HAL_Driver/Inc/Legacy \
  -I$(APP)/Drivers/CMSIS/Device/ST/STM32F1xx/Include -I$(APP)/Drivers/CMSIS/Include
LDFLAGS := $(ARCH) --specs=nosys.specs -T$(BUILD)/app.ld -Wl,--gc-sections -static \
  -Wl,-Map=$(BUILD)/rfr26-tempSensor.map -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group

SOURCES := $(wildcard $(APP)/Core/Src/*.c) $(wildcard $(APP)/Drivers/STM32F1xx_HAL_Driver/Src/*.c)
OBJECTS := $(patsubst $(APP)/%.c,$(BUILD)/%.o,$(SOURCES)) $(BUILD)/startup.o

.PHONY: all clean
all: $(BUILD)/rfr26-tempSensor.bin

# Fails if the application's memory line ever changes shape, rather than linking at the wrong address.
$(BUILD)/app.ld: $(APP)/STM32F103T8UX_FLASH.ld
	@mkdir -p $(dir $@)
	sed -E 's/ORIGIN = 0x0?8000000, *LENGTH = 64K/ORIGIN = 0x08004000, LENGTH = 47K/' $< > $@
	grep -q 'ORIGIN = 0x08004000, LENGTH = 47K' $@

$(BUILD)/rfr26-tempSensor.elf: $(OBJECTS) $(BUILD)/app.ld
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SIZE) $@

$(BUILD)/rfr26-tempSensor.bin: $(BUILD)/rfr26-tempSensor.elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: $(APP)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/startup.o: $(APP)/Core/Startup/startup_stm32f103t8ux.s
	@mkdir -p $(dir $@)
	$(CC) $(ARCH) -x assembler-with-cpp -c $< -o $@

clean:
	rm -rf $(BUILD)
