# STM32F103T8U6 Classical-CAN bootloader

Prototype implementation for issue #19. It targets the two existing STM32F103T8U6 boards and reserves capacity for node IDs 1 through 7.

## Memory layout

- `0x08000000–0x08003FFF`: 16 KiB bootloader
- `0x08004000–0x0800FBFF`: application (maximum 47 KiB)
- `0x0800FC00–0x0800FFFF`: validity metadata

The current temperature-board binaries (about 17–26 KiB) fit. The application linker script must change its FLASH origin to `0x08004000` and length to `47K`, and its vector-table offset must be `0x4000`.

## Protocol

All frames use 11-bit Classical CAN IDs. `BL_NODE_ID` is compiled separately for each board (1–7).

- `0x5E0`: discovery request
- `0x600 + node`: commands
- `0x620 + node`: data (`uint16` sequence followed by up to 6 image bytes)
- `0x680 + node`: response

Commands provide info, begin/erase, set expected CRC-32, finish/verify, abort, start application, and reset. Data acknowledgements are idempotent: if an ACK is lost, the host can safely resend the last frame without programming it twice. An incomplete image is never marked bootable. After power loss the protected bootloader remains available for retransmission; this MCU does not have enough guaranteed flash for two copies of all current applications.

## Internal-oscillator configuration

This branch's bootloader uses the STM32F103's internal 8 MHz HSI oscillator, so it does not require the external crystal during an update. The PLL divides HSI by 2 and multiplies it by 9 for a 36 MHz system and APB1 clock. CAN uses 18 time quanta with a prescaler of 4, producing 500 kbit/s on PA11/PA12.

The temperature application still switches to the board's 12 MHz external crystal after the bootloader starts it. HSI is less accurate and stable than the external crystal, so this variant requires CAN bench testing across expected voltage and temperature conditions before it is considered safe for vehicle use.

## Build

From this directory, run `make`. The Makefile uses the STM32F1 HAL already committed under `rfr26-tempSensor` and expects `arm-none-eabi-gcc` on `PATH`. The compiler can also be selected explicitly, for example `make CC=/path/arm-none-eabi-gcc OBJCOPY=/path/arm-none-eabi-objcopy SIZE=/path/arm-none-eabi-size`.

The bootloader binary is written to `build/stm32f103-can-bootloader.bin`. Initially install it with ST-LINK at `0x08000000`.

With GCC 16.2.0 the current build occupies 6,308 bytes of flash and 1,120 bytes of RAM, fitting comfortably inside the reserved 16 KiB bootloader region.

## Host utility

Install `python-can`, then select the adapter interface supported by python-can:

```sh
python3 ../scripts/can_flash.py firmware.bin --node 1 --interface socketcan --channel can0 --bitrate 500000
python3 ../scripts/can_flash.py --node 1 --interface socketcan --channel can0 --bitrate 500000 --reset
```

Do not test on the complete vehicle first. Validate on two powered bench boards with SWD attached for recovery.

## Application integration

The included temperature-board application is relocated to `0x08004000`. Its normal CAN receive path must recognize command ID `0x600 + node` with command byte `0x05` and call `NVIC_SystemReset()`. After reset, the host has 1.5 seconds to start an update before the bootloader launches the valid application. A permanent node-ID allocation is still required before adding that handler because the repository does not currently define unique update IDs for the two boards.
