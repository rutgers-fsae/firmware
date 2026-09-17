# STM32F103T8U6 Classical-CAN bootloader

Prototype implementation for issue #19. It targets the two existing STM32F103T8U6 boards and reserves capacity for node IDs 1 through 7.

## Memory layout

- `0x08000000–0x08003FFF`: 16 KiB bootloader
- `0x08004000–0x0800FBFF`: application (maximum 47 KiB)
- `0x0800FC00–0x0800FFFF`: validity metadata

The current temperature-board binaries (about 17–26 KiB) fit. The application linker script must change its FLASH origin to `0x08004000` and length to `47K`, and its vector-table offset must be `0x4000`.

## Protocol

All frames use 11-bit Classical CAN IDs. A unique node ID from 1 through 7 is required when compiling each board; there is deliberately no default.

- `0x5E0`: discovery request
- `0x600 + node`: commands
- `0x620 + node`: data (`uint16` sequence followed by up to 6 image bytes)
- `0x680 + node`: response

Commands provide info, begin/erase, set expected CRC-32, finish/verify, abort, start application, and reset. After a successful END, the bootloader reports COMPLETE and remains active until the host sends START and receives its acknowledgement. Data acknowledgements are idempotent: if an ACK is lost, the host can safely resend the last frame without programming it twice. An incomplete image is never marked bootable. After power loss the protected bootloader remains available for retransmission; this MCU does not have enough guaranteed flash for two copies of all current applications.

## Clock configuration

The default `HSE` build uses the boards' 12 MHz external crystal. It runs the system clock at 72 MHz and APB1 at 36 MHz. The optional `HSI` build uses the STM32F103's internal 8 MHz oscillator and runs both the system and APB1 clocks at 36 MHz. In either build, CAN uses 18 time quanta with a prescaler of 4, producing 500 kbit/s on PA11/PA12.

This choice applies only while the bootloader is running. After the bootloader starts an application, that application configures its own clock as before. The HSI is less accurate and stable than the external crystal, so the HSI build requires CAN bench testing across expected voltage and temperature conditions before vehicle use.

## Build

From this directory, run `make NODE=1` for a node-1 external-crystal build, or `make NODE=1 CLOCK=HSI` for its internal-oscillator build. Replace `1` with the board's assigned ID; omitting it or using a value outside 1–7 stops the build. Each clock and node combination uses a separate output directory, so switching settings does not reuse incompatible files. The Makefile uses the STM32F1 HAL already committed under `rfr26-tempSensor` and expects `arm-none-eabi-gcc` on `PATH`.

The binaries are written below `build/HSE/nodeN/` or `build/HSI/nodeN/`. Initially install the selected binary with ST-LINK at `0x08000000`.

With GCC 16.2.0 the HSE build occupies 6,368 bytes of flash and 1,120 bytes of RAM, fitting comfortably inside the reserved 16 KiB bootloader region.

## Security assumption

CRC-32 detects transfer corruption but does not prove who created the firmware. This prototype therefore treats the vehicle CAN network as trusted; it must gain cryptographic firmware-signature verification before use on a network where an untrusted participant could transmit update commands.

## Host utility

Install `python-can`, then select the adapter interface supported by python-can:

```sh
python3 ../scripts/can_flash.py firmware.bin --node 1 --interface socketcan --channel can0 --bitrate 500000
python3 ../scripts/can_flash.py --node 1 --interface socketcan --channel can0 --bitrate 500000 --reset
```

Do not test on the complete vehicle first. Validate on two powered bench boards with SWD attached for recovery.

## Application integration

The included temperature-board application is relocated to `0x08004000`. Its normal CAN receive path must recognize command ID `0x600 + node` with command byte `0x05` and call `NVIC_SystemReset()`. After reset, the host has 1.5 seconds to start an update before the bootloader launches the valid application. A permanent node-ID allocation is still required before adding that handler because the repository does not currently define unique update IDs for the two boards.
