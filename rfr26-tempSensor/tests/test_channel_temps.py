"""Run with python3 tests/test_channel_temps.py; requires a host C compiler."""

import subprocess
import tempfile
from pathlib import Path

source = (Path(__file__).resolve().parents[1] / "Core/Src/main.c").read_text()
helper = source.split(
    "static HAL_StatusTypeDef CAN_SendChannelTemp(uint32_t id, uint8_t temp[7]) {", 1
)[1].split("// send the max/min", 1)[0]
helper = (
    "static HAL_StatusTypeDef CAN_SendChannelTemp(uint32_t id, uint8_t temp[7]) {"
    + helper
)
burst = source.split("#if SEND_CHANNEL_TEMPS", 1)[1].split("#endif", 1)[0]
mock = r"""
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
enum { HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT };
typedef int HAL_StatusTypeDef;
enum { CAN_ID_EXT = 4, CAN_RTR_DATA = 0, DISABLE = 0 };
static int hcan;
static struct { uint32_t ExtId, IDE, RTR, DLC, TransmitGlobalTime; } TxHeader;
static uint8_t TxData[8];
static uint32_t TxMailbox, tick, busy, calls, fail_at;
static uint32_t ids[13];
static uint8_t frames[13][8];
static uint32_t HAL_GetTick(void) { return tick++; }
static uint32_t HAL_CAN_GetTxMailboxesFreeLevel(int *can) {
    (void)can;
    if (busy) { busy--; return 0; }
    return 1;
}
static int HAL_CAN_AddTxMessage(int *can, const void *header, uint8_t *data, uint32_t *mailbox) {
    (void)can; (void)header; (void)mailbox;
    assert(calls < 13);
    ids[calls] = TxHeader.ExtId;
    assert(TxHeader.IDE == CAN_ID_EXT && TxHeader.DLC == 8);
    for (int j = 0; j < 8; j++) frames[calls][j] = data[j];
    calls++;
    busy = 2; /* Each successful enqueue temporarily fills the mailboxes. */
    return calls == fail_at ? HAL_ERROR : HAL_OK;
}
"""
checks = r"""
int main(void) {
    uint8_t temps[90];
    for (int i = 0; i < 90; i++) temps[i] = i + 1;
    assert(CAN_SendChannelTemp(1, temps) == HAL_OK); /* Immediately free. */
    busy = 3;
    assert(CAN_SendChannelTemp(8, temps) == HAL_OK); /* Eventually free. */
    uint32_t before = tick;
    busy = 100;
    assert(CAN_SendChannelTemp(15, temps) == HAL_TIMEOUT);
    assert(calls == 2 && tick - before == 11);
    tick = UINT32_MAX - 5;
    busy = 100;
    assert(CAN_SendChannelTemp(15, temps) == HAL_TIMEOUT); /* Tick wrap. */
    assert(calls == 2);
    busy = 0; fail_at = 3;
    assert(CAN_SendChannelTemp(15, temps) == HAL_ERROR);
    calls = 0; busy = 0; fail_at = 0;
    send_burst(temps);
#if SEND_CHANNEL_TEMPS
    assert(calls == 13);
    for (int i = 0; i < 13; i++) {
        assert(ids[i] == (uint32_t)(i * 7 + 1));
        uint8_t checksum = 65;
        for (int j = 0; j < 7; j++) {
            int idx = i * 7 + j;
            assert(frames[i][j] == (idx < 90 ? temps[idx] : 0));
            checksum += frames[i][j];
        }
        assert(frames[i][7] == checksum);
    }
    calls = 0; busy = 0; fail_at = 2;
    send_burst(temps);
    assert(calls == 2); /* Abort burst on enqueue error. */
    calls = 0; busy = 100;
    send_burst(temps);
    assert(calls == 0); /* Abort burst on timeout. */
#else
    assert(calls == 0);
#endif
}
"""
with tempfile.TemporaryDirectory() as directory:
    path = Path(directory)
    test = path / "test.c"
    test.write_text(
        mock + helper + "\nstatic void send_burst(uint8_t temps[90]) {\n"
        "#if SEND_CHANNEL_TEMPS\n"
        + burst
        + "\n#else\n(void)temps;\n#endif\n}\n"
        + checks
    )
    for enabled in ("true", "false"):
        subprocess.run(
            [
                "cc",
                "-std=c11",
                "-Wall",
                "-Wextra",
                "-Werror",
                f"-DSEND_CHANNEL_TEMPS={enabled}",
                str(test),
                "-o",
                str(path / "test"),
            ],
            check=True,
        )
        subprocess.run([str(path / "test")], check=True)
print("PASS: mailbox waits, timeout/wraparound, errors, frame packing, and toggle")
