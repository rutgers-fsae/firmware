# PM100DX EEPROM CAN TUI

Read and edit Cascadia Motion PM100DX EEPROM parameters using a CANdapter.
Requires Python 3.14, uv, and a terminal supporting `curses` (macOS/Linux).

```sh
uv sync
uv run main.py --port /dev/cu.usbserial-DN7BORLC
```

The default port is `/dev/cu.usbserial-DN7BORLC`. Bus settings match
`../scripts/precharge_can_sim.py`: serial 115200 baud,
CAN 500 kbit/s, standard identifiers, and inverter base `0xA0`.
`--bitrate` accepts 125, 250, 500, or 1000 kbit/s.

The adapter is polled every 2 ms while idle. Before each parameter request,
incoming traffic is drained for 100 ms, matching the existing CANdapter script's
command interval without leaving the USB FIFO unpolled. `--request-gap` adjusts
this interval in seconds. This completed one full live scan, but intermittent
adapter rejections and missing replies remain under investigation.

Connection overrides:

```sh
uv run main.py --port /dev/cu.usbserial-YOUR_ADAPTER --bitrate 500 --base 0xA0 --mode standard
uv run main.py --mode extended --base 0xA0 --timeout 2 --freshness 1
uv run main.py --help
```

Only one application should own the adapter, and only one CAN node should issue
parameter read/write requests during a session. `c` reconnects using the original
CLI settings. After programming new CAN settings and power cycling, relaunch
with the new settings shown in the batch result.

## Individual reads and writes

These commands open the adapter without scanning all parameters or starting the TUI:

```sh
uv run main.py read 100
uv run main.py write 129 150
uv run main.py write 129 1500 --raw
```

Addresses are from the PDF, not positions in the GUI export. Values use engineering
units by default (the examples write 150 Nm); `--raw` accepts raw decimal or hex
words. Connection options such as `--port` and `--timeout` work with these commands.
`write` applies immediately: it validates the value and related settings, checks
for changes since the initial read, requires fresh inverter-disabled telemetry,
and verifies by readback. Writes are never retried. No full scan is performed.
Running without a command still opens the TUI.

## Keys

Connecting does not read EEPROM automatically. Select a parameter and press `r`
to read it, `i` to edit it, then `w` to review and apply the staged value. A single
edit is verified without a full scan. `R` explicitly reads all parameters; batches
of multiple edits retain the full refresh.

| Key                 | Action                                                   |
| ------------------- | -------------------------------------------------------- |
| `j` / `k`, arrows   | Select next/previous parameter                           |
| `gg` / `G`          | First/last parameter                                     |
| `Ctrl-u` / `Ctrl-d` | Half-page navigation                                     |
| `/`, `n` / `N`      | Search names/aliases/addresses; next/previous match      |
| `Enter` / `i`       | Edit and stage a value; `Ctrl-r` toggles units/raw input |
| `r` / `R`           | Read selected parameter / all parameters                 |
| `u`                 | Unstage selected edit                                    |
| `w`                 | Review changes, then type `WRITE` to apply               |
| `e`                 | Export observed EEPROM values to JSON                    |
| `o`                 | Preview/import JSON or RMS GUI name/value text           |
| `c`                 | Reconnect                                                |
| `Esc`               | Cancel dialog or active operation between transactions   |
| `?`                 | Help                                                     |
| `q`                 | Quit; type `DISCARD` if edits remain staged              |

Engineering values must be exactly representable; raw entry accepts decimal,
hexadecimal (`0xFC28`), and signed decimal where appropriate (`-984`).
Raw words are stored as unsigned 16-bit integers in JSON. For example, gamma
`-98.4 deg` is raw signed `-984`, stored as word `64552` (`0xFC28`).

## Reads, writes, and power cycles

The catalog comes from **CAN Protocol (V6_3).pdf**, section 2.3.4. It excludes
reserved/factory addresses, general commands, and Gen 5/CM-only parameters.
Gen 3-only entries are labeled and probed. Unsupported entries stay visible.
A parameter listed in an RMS GUI export is not necessarily accessible through
this PDF's CAN protocol; no addresses are inferred from file order.

“Current” is the last observed **EEPROM contents**, not a guarantee that the
inverter is already operating with those settings. `NOW` entries are highlighted
in yellow in the PDF and take effect immediately. `CYCLE` entries require an
operator-performed power cycle. Hardware-specific limits beyond those in the
PDF remain the operator's responsibility.

Reading and editing do not write EEPROM. Before a batch, every edited value is
reread and compared with its staging baseline. Every write requires fresh
inverter-disabled telemetry from the Internal States message (`base + 0x0A`,
byte 6 bit 0). Missing, stale, or enabled status blocks writes; there is no
override. If status broadcasts are disabled, restore them using another
supported programming interface before writing with this tool.

Each write is acknowledged and independently read back. A rejection, timeout,
mismatch, conflict, or cancellation stops the batch. Completed writes remain
stored; unverified edits remain staged. Writes are never automatically retried
or rolled back. Reads retry the same parameter up to twice when a reply is missing
or the adapter rejects the transmit request.
After a transaction exhausts its attempts, reconnect before further requests
so delayed replies cannot be accepted by a later transaction.

Motor-type changes are written first and followed by a full reread because they
can reset flux/gamma. If another staged parameter changed as a side effect,
restage it against its new baseline. Communication changes are written last.
Disabling CAN termination, status broadcasts, or parameter mailboxes can prevent
later communication; the batch stops rather than assuming success.
The application does not send motor commands or perform power cycles.

## Files

Exports are version-1 JSON snapshots containing capture time, connection
settings, observed firmware, and per-parameter address, alias, raw word, units,
decoded display value, read time, and status. Only observed values are exported;
staged edits are excluded. Missing/stale/unsupported rows are explicit and make
`complete` false. Replacement is atomic; overwrites require confirmation.

Import accepts this JSON format and the decimal name/value text used by
`../PM100DX_EEPROM`. Names are matched through explicit aliases. Only successful
source values with successful live baselines can become staged differences.
Unknown, duplicate, invalid, and unsupported records are reported and omitted.
Reviewing an import never writes EEPROM or changes connection settings. Apply
staged imports with `w`.

## Verification

```sh
uv run python -m unittest -v
```

Offline checks use a fake CANdapter; they cover framing, scaling, fragmented
replies, write interlocks, conflicts, partial batches, and import/export.

Bench acceptance requires an attached adapter and inverter: read and export a
snapshot, change an operator-selected parameter with the motor disabled, verify
readback, then check persistence after an operator-performed power cycle.
