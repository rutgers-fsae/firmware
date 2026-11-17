"""Run with python3 tests/test_channel_temps.py; requires a host C compiler."""

import subprocess
import tempfile
from pathlib import Path

root = Path(__file__).resolve().parents[1]
source = (root / "Core/Src/main.c").read_text()
helpers = source.split("static HAL_StatusTypeDef CAN_SendFrame(void) {", 1)[1].split(
    "// send the max/min", 1
)[0]
helpers = "static HAL_StatusTypeDef CAN_SendFrame(void) {" + helpers
mock = r"""
#include <assert.h>
#include <stdint.h>
enum { HAL_OK, HAL_ERROR, HAL_BUSY, HAL_TIMEOUT };
typedef int HAL_StatusTypeDef;
enum { CAN_ID_EXT=4, CAN_RTR_DATA=0, DISABLE=0, CAN_TX_TIMEOUT_MS=100,
       CAN_TX_MAILBOX0=1, CAN_TX_MAILBOX1=2, CAN_TX_MAILBOX2=4,
       CAN_TSR_TXOK0=1, CAN_TSR_TXOK1=256, CAN_TSR_TXOK2=65536 };
static struct { uint32_t TSR; } registers;
static struct { typeof(registers) *Instance; } hcan = { &registers };
static struct { uint32_t ExtId, IDE, RTR, DLC, TransmitGlobalTime; } TxHeader;
static uint8_t TxData[8], frames[13][8];
static uint32_t TxMailbox, tick, busy, pending, calls, aborted, mailbox=1;
static int enqueue_error, delivery_error, abort_error;
static uint32_t HAL_GetTick(void) { return tick++; }
static uint32_t HAL_CAN_GetTxMailboxesFreeLevel(void *can) {
    (void)can; if (busy) { busy--; return 0; } return 1;
}
static int HAL_CAN_AddTxMessage(void *can, void *header, uint8_t *data, uint32_t *mb) {
    (void)can; (void)header;
    if (enqueue_error) return HAL_ERROR;
    assert(calls < 13); *mb=mailbox; registers.TSR=0;
    for (int j=0; j<8; j++) frames[calls][j]=data[j];
    calls++; return HAL_OK;
}
static uint32_t HAL_CAN_IsTxMessagePending(void *can, uint32_t mb) {
    (void)can; (void)mb;
    if (pending) { pending--; return 1; }
    if (!delivery_error) registers.TSR = mailbox==1 ? 1 : mailbox==2 ? 256 : 65536;
    return 0;
}
static int HAL_CAN_AbortTxRequest(void *can, uint32_t mb) {
    (void)can; (void)mb; aborted++; pending=0;
    return abort_error ? HAL_ERROR : HAL_OK;
}
"""
checks = r"""
int main(void) {
    uint8_t temps[7]={1,2,3,4,5,6,7};
    busy=3; pending=3;
    assert(CAN_SendChannelTemp(1,temps)==HAL_OK);
    assert(calls==1 && TxData[7]==93 && TxHeader.DLC==8);
    calls=0; busy=200;
    assert(CAN_SendChannelTemp(1,temps)==HAL_TIMEOUT && calls==0);
    busy=0; enqueue_error=1;
    assert(CAN_SendChannelTemp(1,temps)==HAL_ERROR);
    enqueue_error=0; pending=200; tick=UINT32_MAX-5;
    assert(CAN_SendChannelTemp(1,temps)==HAL_TIMEOUT && aborted==1);
    pending=200; abort_error=1;
    assert(CAN_SendChannelTemp(1,temps)==HAL_ERROR);
    abort_error=0; delivery_error=1;
    assert(CAN_SendChannelTemp(1,temps)==HAL_ERROR);
    delivery_error=0;
    for (mailbox=1; mailbox<=4; mailbox*=2)
        assert(CAN_SendChannelTemp(1,temps)==HAL_OK);
    calls=0; mailbox=1;
    for (int i=0; i<13; i++) {
        busy=3; pending=3;
        assert(CAN_SendChannelTemp(1U + 7U*i,temps)==HAL_OK);
    }
    assert(calls==13 && TxHeader.ExtId==85U);
}
"""
assert "#define DAQ_BASE_ID 1U" in source
assert "CAN_SendChannelTemp(DAQ_BASE_ID + i, sendTemp)" in source
assert "RCC_PLL_MUL6" in source and "hcan.Init.Prescaler = 2;" in source
assert "hcan.Init.AutoBusOff = ENABLE;" in source
assert "hcan.Init.AutoRetransmission = ENABLE;" in source
assert 12000000 * 6 // 2 // 2 // 2 // (1 + 13 + 4) == 500000
with tempfile.TemporaryDirectory() as directory:
    path = Path(directory)
    (path / "test.c").write_text(mock + helpers + checks)
    subprocess.run(
        [
            "cc",
            "-std=gnu11",
            "-Wall",
            "-Wextra",
            "-Werror",
            str(path / "test.c"),
            "-o",
            str(path / "test"),
        ],
        check=True,
    )
    subprocess.run([str(path / "test")], check=True)
print(
    "PASS: mailbox waits, delivery, timeout/wraparound, abort/errors, 13 frames, timing"
)
