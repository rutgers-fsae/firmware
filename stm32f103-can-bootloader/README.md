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
- `0x620 + node`: data (one sequence byte, up to 5 image bytes, then a little-endian CRC-16)
- `0x680 + node`: response

Protocol v4 protects each data frame with CRC-16/CCITT-FALSE (polynomial `0x1021`, initial value `0xFFFF`, no reflection or final XOR), covering the sequence byte and image bytes. The sequence byte is the low 8 bits of the frame number. The final CRC-32 still verifies the entire image before validity metadata is written.

Data uses selective repeat over 32-frame windows, because the Ewert CANdapter loses or damages a few percent of frames when sent in bursts. The bootloader buffers the open window in RAM; frames may arrive in any order and more than once, and damaged, duplicate or out-of-window frames are dropped without a reply. The host sends a window followed by command `0x08` (window status). The reply carries the number of frames already programmed (sequence field) and a bit mask of the frames held for the open window (detail field). The host resends only the missing frames and asks again. When the window is complete the bootloader programs it and the next status reports the advanced frame count with an empty mask. `BL_WINDOW_FRAMES` and `BL_DATA_BYTES_PER_FRAME` in `bootloader_protocol.h` must match `WINDOW` and `CHUNK` in `can_flash.py`.

INFO and discovery encode the protocol version in the high byte of their detail field and the maximum image size in the lower 24 bits. The uploader refuses any other protocol version before erasing the application. Upgrade the bootloader through ST-LINK before using this uploader with an older board.

Metadata is serialized as explicit 16-bit words so optimized builds do not read an aliased, uninitialized structure when programming flash.

Commands provide info, begin/erase, set expected CRC-32, finish/verify, abort, start application, and reset. After a successful END, the bootloader reports COMPLETE and starts the application when the host sends START, or by itself 1.5 seconds after the last command. SET_CRC and END can be repeated after a lost reply. Any command or data frame addressed to the node restarts the 1.5 second window, so a host that is talking to the bootloader is never cut off by the application starting. BEGIN erases the metadata page first, so an interrupted erase never leaves an image marked valid. An incomplete image is never marked bootable. After power loss the protected bootloader remains available for retransmission; this MCU does not have enough guaranteed flash for two copies of all current applications.

## Clock configuration

The default `HSE` build uses the boards' 12 MHz external crystal. It runs the system clock at 72 MHz and APB1 at 36 MHz. The optional `HSI` build uses the STM32F103's internal 8 MHz oscillator and runs both the system and APB1 clocks at 36 MHz. In either build, CAN uses 18 time quanta with a prescaler of 4 and a 4-quanta synchronization jump width, producing 500 kbit/s on PA11/PA12. If the crystal fails to start, the `HSE` build falls back to the `HSI` configuration so the board stays reachable over CAN.

This choice applies only while the bootloader is running. After the bootloader starts an application, that application configures its own clock as before. The HSI is less accurate and stable than the external crystal, so the HSI build requires CAN bench testing across expected voltage and temperature conditions before vehicle use.

## Build

From this directory, run `make NODE=1` for a node-1 external-crystal build, or `make NODE=1 CLOCK=HSI` for its internal-oscillator build. Replace `1` with the board's assigned ID; omitting it or using a value outside 1–7 stops the build. Each clock and node combination uses a separate output directory, so switching settings does not reuse incompatible files. The Makefile uses the STM32F1 HAL already committed under `rfr26-tempSensor` and expects `arm-none-eabi-gcc` on `PATH`.

The binaries are written below `build/HSE/nodeN/` or `build/HSI/nodeN/`. Initially install the selected binary with ST-LINK at `0x08000000`.

`make -f tempsensor-app.mk` builds `rfr26-tempSensor` to run behind the bootloader (linked at `0x08004000`) into `build/tempsensor-app/`, without modifying the application project.

With GCC 15.3.1 the HSE build occupies 6,700 bytes of flash and 1,300 bytes of RAM, fitting comfortably inside the reserved 16 KiB bootloader region.

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
```

The Ewert backend uses its native `T` standard-frame encoding, continuously drains USB receive data and filters unrelated CAN traffic in the host. Filtering does not reduce traffic on the vehicle bus. Each 32-frame window is written to the adapter in one burst. Measured on a bus carrying about 310 frames/s of other traffic, the adapter transmits about 1,650 frames/s but rejects, drops or damages roughly 4% of burst frames; selective repeat recovers those. Transfers on that bus still stall intermittently: the adapter stops answering for about a second, the uploader gives up with "no progress", and the upload has to be restarted. Treat the Ewert path as experimental until that is resolved. `scripts/candapter_tx_rate.py` repeats that measurement.

The uploader sends RESET and then polls INFO for up to `--wait` seconds (default 10) before erasing anything. If the application has no reset handler, start the uploader and power-cycle the board while it is polling. Interrupted transfers restart from the beginning.

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
