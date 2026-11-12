"""Run the real C data receiver from the bootloader against the Python flasher.

The data-frame, CRC and metadata routines are cut out of main.c and compiled with the
host C compiler, so these tests need `cc`.
"""

import binascii
import ctypes
import struct
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(Path(__file__).resolve().parent))
from test_can_flash import Target, flasher, module  # noqa: E402

SOURCE = (REPO / "stm32f103-can-bootloader/src/main.c").read_text()
HEADER = REPO / "stm32f103-can-bootloader/include/bootloader_protocol.h"


def cut(start, end):
    begin = SOURCE.index(start)
    return SOURCE[begin : SOURCE.index(end, begin)]


HARNESS = f'''#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "{HEADER}"
enum {{RX_IDLE, RX_WAITING_CRC, RX_DATA, RX_COMPLETE}};
static uint32_t rx_state, image_size, bytes_written, expected_crc, window_mask;
static uint16_t expected_sequence;
static uint8_t window_data[BL_WINDOW_FRAMES * BL_DATA_BYTES_PER_FRAME];
#define BL_METADATA_MAGIC 0x52465242UL
#define BL_METADATA_ADDRESS 0x0800FC00UL
#define FLASH_TYPEPROGRAM_HALFWORD 1
#define HAL_OK 0
static uint16_t metadata_words[8];
static unsigned char image[48128];
static int reply_count; static uint8_t reply_status; static uint16_t reply_sequence; static uint32_t reply_detail;
static void HAL_FLASH_Unlock(void) {{}}
static void HAL_FLASH_Lock(void) {{}}
static int HAL_FLASH_Program(int type, uint32_t address, uint16_t word) {{
    (void)type; metadata_words[(address - BL_METADATA_ADDRESS) / 2] = word; return 0;
}}
static uint32_t HAL_FLASH_GetError(void) {{ return 0; }}
static bool write_bytes(const uint8_t *data, uint8_t n) {{ memcpy(image + bytes_written, data, n); return true; }}
static void send_status(bl_status_t s, uint8_t cmd, uint16_t seq, uint32_t detail) {{
    (void)cmd; ++reply_count; reply_status = s; reply_sequence = seq; reply_detail = detail;
}}
{cut("static uint16_t packet_crc16(", chr(10) + "static bool write_bytes(")}
{cut("static bool metadata_write(void) {", chr(10) + "static bool application_is_valid")}
void test_metadata(uint16_t *out) {{
    image_size = 3968; expected_crc = 0xE5900042; metadata_write();
    for (int i = 0; i < 8; ++i) out[i] = metadata_words[i];
}}
void reset_receiver(uint32_t size) {{
    memset(image, 255, sizeof(image)); image_size = size; bytes_written = 0; expected_sequence = 0;
    rx_state = RX_DATA; window_mask = 0; reply_count = 0;
}}
void receive(const uint8_t *data, uint8_t n) {{ process_data(data, n); }}
uint32_t mask(void) {{ return window_mask; }}
int replies(void) {{ return reply_count; }}
uint8_t status(void) {{ return reply_status; }}
uint16_t sequence(void) {{ return reply_sequence; }}
uint32_t detail(void) {{ return reply_detail; }}
uint32_t written(void) {{ return bytes_written; }}
uint16_t frames(void) {{ return expected_sequence; }}
void copy_image(uint8_t *out, uint32_t n) {{ memcpy(out, image, n); }}
'''

BUILD = tempfile.TemporaryDirectory()
(Path(BUILD.name) / "receiver.c").write_text(HARNESS)
subprocess.run(
    [
        "cc",
        "-shared",
        "-fPIC",
        "-O2",
        "-Wall",
        "-Wextra",
        "-Werror",
        str(Path(BUILD.name) / "receiver.c"),
        "-o",
        str(Path(BUILD.name) / "receiver.so"),
    ],
    check=True,
)
lib = ctypes.CDLL(str(Path(BUILD.name) / "receiver.so"))
lib.receive.argtypes = [ctypes.c_char_p, ctypes.c_uint8]
lib.written.restype = lib.detail.restype = lib.mask.restype = ctypes.c_uint32
lib.sequence.restype = lib.frames.restype = ctypes.c_uint16


def packet(sequence, data):
    payload = bytes([sequence & 0xFF]) + data
    return payload + struct.pack("<H", binascii.crc_hqx(payload, 0xFFFF))


def receive(data):
    lib.receive(data, len(data))


class CTarget(Target):
    """Commands in Python, data frames through the compiled bootloader receiver, with faults."""

    def __init__(self, drop_every=0, corrupt_every=0, lose_status_every=0):
        super().__init__()
        self.drop_every, self.corrupt_every = drop_every, corrupt_every
        self.lose_status_every = lose_status_every
        self.data_frames = self.dropped_frames = self.corrupted = 0
        self.statuses = self.lost_statuses = 0

    def begin(self, size):
        lib.reset_receiver(size)

    def accepted_frames(self):
        return lib.frames()

    def window_mask(self):
        return lib.mask()

    def contents(self):
        out = ctypes.create_string_buffer(self.size)
        lib.copy_image(out, self.size)
        return out.raw

    def on_data(self, data):
        self.data_frames += 1
        if self.drop_every and self.data_frames % self.drop_every == 0:
            self.dropped_frames += (
                1  # Like an adapter BELL: the frame never reaches the bus.
            )
            return
        if self.corrupt_every and self.data_frames % self.corrupt_every == 0:
            self.corrupted += 1
            data = bytes([data[0], data[1] ^ 0x40]) + data[2:]
        receive(data)

    def reply(self, status, command, sequence=0, detail=0):
        if command == module.WINDOW_STATUS:
            self.statuses += 1
            if self.lose_status_every and self.statuses % self.lose_status_every == 0:
                self.lost_statuses += 1
                return
        super().reply(status, command, sequence, detail)


class ReceiverTests(unittest.TestCase):
    def test_metadata_serialization_with_optimization(self):
        words = (ctypes.c_uint16 * 8)()
        lib.test_metadata(words)
        self.assertEqual(
            bytes(words), struct.pack("<IIII", 0x52465242, 3968, 0xE5900042, 0xFFFFFFFF)
        )

    def test_every_single_bit_corruption_is_dropped(self):
        for size in range(1, 6):
            good = packet(0, bytes(range(size)))
            for bit in range(len(good) * 8):
                lib.reset_receiver(size)
                bad = bytearray(good)
                bad[bit // 8] ^= 1 << (bit % 8)
                receive(bytes(bad))
                self.assertEqual((lib.mask(), lib.written(), lib.replies()), (0, 0, 0))

    def test_window_is_programmed_only_when_complete_in_any_order(self):
        size = module.SPAN + 7
        image = bytes(range(256))[:size]
        lib.reset_receiver(size)
        slots = list(range(module.WINDOW))
        for slot in reversed(slots[1:]):
            receive(packet(slot, image[slot * 5 : slot * 5 + 5]))
            receive(
                packet(slot, image[slot * 5 : slot * 5 + 5])
            )  # Duplicates are harmless.
        self.assertEqual((lib.written(), lib.frames()), (0, 0))
        self.assertEqual(lib.mask(), 0xFFFFFFFE)
        receive(packet(0, image[:5]))
        self.assertEqual(
            (lib.written(), lib.frames(), lib.mask()), (module.SPAN, module.WINDOW, 0)
        )
        # The final, short window: one full frame and a 2-byte frame.
        receive(packet(module.WINDOW + 1, image[module.SPAN + 5 :]))
        self.assertEqual((lib.written(), lib.mask()), (module.SPAN, 0b10))
        receive(packet(module.WINDOW, image[module.SPAN : module.SPAN + 5]))
        self.assertEqual(
            (lib.written(), lib.frames(), lib.mask()), (size, module.WINDOW + 2, 0)
        )
        out = ctypes.create_string_buffer(size)
        lib.copy_image(out, size)
        self.assertEqual(out.raw, image)
        self.assertEqual(lib.replies(), 0)

    def test_frames_outside_the_open_window_are_ignored(self):
        lib.reset_receiver(1000)
        receive(packet(module.WINDOW, b"abcde"))  # First frame of the next window.
        receive(packet(255, b"abcde"))  # A late duplicate from a previous window.
        self.assertEqual((lib.mask(), lib.written()), (0, 0))

    def test_wrong_payload_length_is_dropped(self):
        lib.reset_receiver(7)
        receive(packet(0, b"abc"))
        receive(packet(1, b"abcde"))
        self.assertEqual((lib.mask(), lib.written()), (0, 0))

    def test_upload_survives_dropped_frames_corruption_and_lost_replies(self):
        image = bytes(range(256)) * 12 + bytes(
            range(7)
        )  # About ten acknowledgement windows.
        bus = CTarget(drop_every=37, corrupt_every=53, lose_status_every=4)
        flasher(bus).flash(image, wait=0.05)
        self.assertEqual(bus.contents(), image)
        self.assertGreater(bus.dropped_frames, 5)
        self.assertGreater(bus.corrupted, 5)
        self.assertGreater(bus.lost_statuses, 2)

    def test_clean_upload_sends_each_frame_once(self):
        image = bytes(range(256)) * 12 + bytes(range(7))
        bus = CTarget()
        flasher(bus).flash(image, wait=0.05)
        self.assertEqual(bus.contents(), image)
        self.assertEqual(bus.data_frames, -(-len(image) // module.CHUNK))

    def test_dead_link_gives_up_instead_of_looping(self):
        bus = CTarget(drop_every=1)
        with self.assertRaisesRegex(module.FlashError, "no progress at byte 0"):
            flasher(bus).flash(bytes(100), wait=0.05)


if __name__ == "__main__":
    unittest.main()
