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
- `0x620 + node`: data (`uint16` little-endian sequence, up to 4 image bytes, then a little-endian CRC-16)
- `0x680 + node`: response

Protocol v2 protects each data frame with CRC-16/CCITT-FALSE (polynomial `0x1021`, initial value `0xFFFF`, no reflection or final XOR), covering the sequence bytes and image bytes. A bad packet returns status `0x87` without advancing the sequence or writing flash; the host retries the same packet. The final CRC-32 still verifies the entire image before validity metadata is written. INFO and discovery encode the protocol version in the high byte of their detail field and the maximum image size in the lower 24 bits. The uploader refuses a non-v2 bootloader before erasing the application. Upgrade the bootloader through ST-LINK before using this uploader with a v1 board.

Metadata is serialized as explicit 16-bit words so optimized builds do not read an aliased, uninitialized structure when programming flash.

Commands provide info, begin/erase, set expected CRC-32, finish/verify, abort, start application, and reset. After a successful END, the bootloader reports COMPLETE and remains active until the host sends START and receives its acknowledgement. Data acknowledgements are idempotent: if an ACK is lost, the host can safely resend the last frame without programming it twice. An incomplete image is never marked bootable. After power loss the protected bootloader remains available for retransmission; this MCU does not have enough guaranteed flash for two copies of all current applications.

## Clock configuration

The default `HSE` build uses the boards' 12 MHz external crystal. It runs the system clock at 72 MHz and APB1 at 36 MHz. The optional `HSI` build uses the STM32F103's internal 8 MHz oscillator and runs both the system and APB1 clocks at 36 MHz. In either build, CAN uses 18 time quanta with a prescaler of 4, producing 500 kbit/s on PA11/PA12.

This choice applies only while the bootloader is running. After the bootloader starts an application, that application configures its own clock as before. The HSI is less accurate and stable than the external crystal, so the HSI build requires CAN bench testing across expected voltage and temperature conditions before vehicle use.

## Build

From this directory, run `make NODE=1` for a node-1 external-crystal build, or `make NODE=1 CLOCK=HSI` for its internal-oscillator build. Replace `1` with the board's assigned ID; omitting it or using a value outside 1–7 stops the build. Each clock and node combination uses a separate output directory, so switching settings does not reuse incompatible files. The Makefile uses the STM32F1 HAL already committed under `rfr26-tempSensor` and expects `arm-none-eabi-gcc` on `PATH`.

The binaries are written below `build/HSE/nodeN/` or `build/HSI/nodeN/`. Initially install the selected binary with ST-LINK at `0x08000000`.

With GCC 16.2.0 the HSE build occupies 6,504 bytes of flash and 1,120 bytes of RAM, fitting comfortably inside the reserved 16 KiB bootloader region.

## Security assumption

CRC-32 detects transfer corruption but does not prove who created the firmware. This prototype therefore treats the vehicle CAN network as trusted; it must gain cryptographic firmware-signature verification before use on a network where an untrusted participant could transmit update commands.

## Host utility

Install `python-can`, then select the adapter interface supported by python-can:

```sh
python3 ../scripts/can_flash.py firmware.bin --node 1 --interface socketcan --channel can0 --bitrate 500000
python3 ../scripts/can_flash.py --node 1 --interface socketcan --channel can0 --bitrate 500000 --reset
```

For an Ewert CANdapter on macOS, also install `pyserial` and specify the actual serial device:

```sh
python3 -m pip install python-can pyserial
python3 ../scripts/can_flash.py firmware.bin --node 1 --interface ewert --channel /dev/cu.usbserial-YOUR_ADAPTER
python3 ../scripts/can_flash.py firmware.bin --node 1 --interface ewert --channel /dev/cu.usbserial-YOUR_ADAPTER --fast
```

The Ewert backend uses its native `T` standard-frame encoding, continuously drains USB receive data, filters unrelated CAN traffic in the host, and waits for adapter acknowledgements. Filtering does not reduce traffic on the vehicle bus. Default packet spacing is 150 ms; `--fast` selects 10 ms while retaining retries, per-packet CRC, echoed image-size/CRC checks, and final image verification. Fast mode is an unvalidated performance option: measure on the intended adapter and bus load. Interrupted transfers restart from the beginning; recovery does not skip verification. Setup retries restart BEGIN before any image data when the setup acknowledgement is lost.

Run regression checks with:

```sh
python3 -m unittest discover -s ../scripts/tests -p 'test_*can*.py' -v
python3 -m unittest discover -s ../scripts/tests -p test_bootloader_receiver.py -v
```

The receiver tests compile the actual C CRC/data/metadata routines with a host C compiler. The Ewert integration test uses a pseudo-terminal, background traffic, lost/duplicate replies, and an adapter rejection; it needs `pyserial` and a POSIX host.

Do not test on the complete vehicle first. Validate on two powered bench boards with SWD attached for recovery.

## Application integration

This PR contains the bootloader and host flashing tools only; it does not modify application firmware. Applications must be separately linked at `0x08004000` with a `0x4000` vector-table offset before they can run behind this bootloader. An existing binary linked at `0x08000000` cannot simply be flashed at the application address.

For updates while an application is running, the application must separately implement a CAN receive handler for command ID `0x600 + node`, command byte `0x05`, that calls `NVIC_SystemReset()`. Without that integration, reset the board physically and begin the transfer during the bootloader startup window. After reset, the host has 1.5 seconds to start an update before the bootloader launches a valid application. Assign a unique node ID to each application and its matching bootloader; do not reuse node 1 across boards.
