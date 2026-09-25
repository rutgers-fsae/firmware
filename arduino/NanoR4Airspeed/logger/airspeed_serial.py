"""Serial acquisition stays off the GUI thread; every sample goes to the run CSV."""
import json
import queue
import threading
import time
from collections import deque
from pathlib import Path
import serial
from serial.tools import list_ports
from airspeed_core import (DeviceClock, RunWriter, ZeroCalibration, QUANTUM_PA,
                           calculate, parse_line, stamp, utc, validate_rate)


class SerialWorker(threading.Thread):
    def __init__(self, directory, commands, events, batches, calibration=None, port_override=None):
        super().__init__(daemon=True)
        self.directory = Path(directory)
        self.commands, self.events, self.batches = commands, events, batches
        self.conn = None
        self.port_override = port_override  # Used only by the explicit simulator test.
        self.board_serial = None
        self.buffer = b''
        self.clock = DeviceClock()
        self.target_rate, self.applied_rate, self.version = 20, None, None
        self.cal = calibration
        self.cal_id = ''
        self.zero = None
        self.writer = None
        self.pending = []
        self.recent = deque(maxlen=10000)
        self.last_sample = 0
        self.last_valid = 0
        self.last_flush = 0
        self.last_notice = ''
        self.next_connect = 0
        self.running = True
        self.last_row = None
        self.total_missing = 0
        self.skip_base = None
        self.skipped_since_rate = 0

    def emit(self, kind, **data):
        self.events.put({'kind': kind, **data})

    def log(self, row):
        row.setdefault('received_utc', utc())
        row.setdefault('density_kg_m3', 1.225)
        if self.writer:
            self.writer.append(row)
        self.pending.append(row)
        self.last_row = row

    def notice(self, message, state='connection_event'):
        if message != self.last_notice:
            self.last_notice = message
            self.emit('notice', message=message)
            self.log({'reading_state': state, 'source_line': message})

    def clear_cal(self):
        self.cal, self.cal_id, self.zero = None, '', None
        self.emit('calibration', calibration=None, message='Zero in still air before calculating speed.')

    def disconnect(self, reason):
        if self.conn:
            self.conn.close()
        self.conn = None
        self.buffer = b''
        self.version = self.applied_rate = None
        self.last_valid = 0
        self.recent.clear()
        self.clear_cal()
        self.next_connect = time.monotonic() + 3
        self.emit('connection', connected=False, port='', firmware=None)
        self.notice(reason, 'usb_unavailable')

    def connect(self):
        if self.port_override:
            port, identity = self.port_override, 'SIMULATOR'
        else:
            ports = [p for p in list_ports.comports() if p.vid == 0x2341 and p.pid == 0x0074]
            if self.board_serial:
                ports = [p for p in ports if p.serial_number == self.board_serial]
            if len(ports) != 1:
                self.notice('Connect one Nano R4. Saved runs are available below.', 'waiting_for_board')
                return
            port, identity = ports[0].device, ports[0].serial_number
        self.conn = serial.Serial(port, 115200, timeout=0.02, write_timeout=0.5, exclusive=True)
        self.conn.reset_input_buffer()
        self.buffer = b''
        self.clock = DeviceClock()
        self.board_serial = identity
        self.last_sample = time.monotonic()
        if self.cal and self.cal.get('board_serial') != identity:
            self.clear_cal()
        self.conn.write(b'INFO\n')
        self.emit('connection', connected=True, port=port, firmware=None)
        self.notice('Connected; checking Arduino firmware…')

    def command(self, data):
        kind = data['kind']
        if kind == 'shutdown':
            self.running = False
        elif kind == 'rate':
            self.target_rate = validate_rate(data['rate'])
            if self.conn and self.version == 2:
                self.conn.write(f'RATE,{self.target_rate}\n'.encode())
                self.emit('rate_pending', rate=self.target_rate)
            elif self.conn:
                self.emit('error', message='Upload the updated Arduino sketch to change the sampling rate.')
        elif kind == 'start':
            if self.writer:
                raise ValueError('A recording is already running.')
            self.writer = RunWriter(self.directory, data['title'], self.target_rate)
            self.pending = []
            self.emit('recording', active=True, path=str(self.writer.path), title=self.writer.title)
        elif kind == 'stop':
            if self.writer:
                path, title, rows = str(self.writer.path), self.writer.title, self.writer.rows
                self.writer.close()
                self.writer = None
                self.emit('recording', active=False, path=path, title=title, rows=rows)
        elif kind == 'zero':
            if not self.conn or time.monotonic() - self.last_valid > 3:
                raise ValueError('Wait for normal sensor readings before zeroing.')
            self.cal, self.cal_id = None, ''
            self.conn.reset_input_buffer()
            self.buffer = b''
            self.zero = ZeroCalibration()
            self.log({'reading_state': 'zero_started', 'source_line': 'Operator started still-air zero calibration'})
            self.emit('calibration', calibration=None, message='Keep still: collecting at least 20 fresh readings over at least 5 seconds.')

    def handle_line(self, line):
        received, now = utc(), time.monotonic()
        try:
            packet = parse_line(line)
        except (ValueError, OverflowError):
            packet = {'kind': 'unknown'}
        kind = packet['kind']
        if kind == 'hello':
            self.version = packet['version']
            self.applied_rate = packet['rate']
            self.emit('firmware', version=self.version, rate=self.applied_rate)
            if self.version == 2:
                self.conn.write(f'RATE,{self.target_rate}\n'.encode())
            return
        if kind == 'rate':
            self.applied_rate = packet['rate']
            self.recent.clear()
            self.skip_base = None
            self.skipped_since_rate = 0
            self.emit('rate_applied', rate=self.applied_rate)
            self.log({'reading_state': 'rate_changed', 'requested_rate_hz': self.applied_rate, 'source_line': line})
            return
        if kind == 'command_error':
            self.emit('error', message=line)
            return
        if kind == 'recovery':
            self.last_valid = 0
            self.log({'reading_state': 'i2c_recovery', 'source_line': line})
            return
        if kind not in ('sample', 'failure'):
            self.log({'reading_state': 'unrecognized_data', 'source_line': line})
            return
        if packet.get('legacy') and self.version != 1:
            self.version, self.applied_rate = 1, 1
            self.emit('firmware', version=1, rate=1)
        elif not packet.get('legacy') and self.version is None:
            self.version = 2
            self.conn.write(b'INFO\n')
        self.last_sample = now
        timing, reset = self.clock.update(packet, received)
        if reset:
            self.clear_cal()
            self.recent.clear()
            self.log({'reading_state': 'device_restarted', 'source_line': 'Device counter restarted; zero calibration cleared'})
        self.total_missing += timing.get('missing_serial_samples', 0)
        if 'skipped' in packet:
            if self.skip_base is None or packet['skipped'] < self.skip_base:
                self.skip_base = packet['skipped']
            self.skipped_since_rate = packet['skipped'] - self.skip_base
        base = {'received_utc': received, 'source_line': line, 'requested_rate_hz': packet.get('rate', ''), **timing}
        fresh = kind == 'sample' and packet['status'] == 0 and 1638.3 <= packet['raw'] <= 14744.7
        rate_time = timing.get('device_elapsed_s', now)
        self.recent.append((rate_time, fresh))
        while self.recent and rate_time - self.recent[0][0] > 2:
            self.recent.popleft()
        if kind == 'failure':
            self.last_valid = 0
            self.log(dict(base, reading_state='read_failed'))
            return
        raw, status = packet['raw'], packet['status']
        if fresh:
            self.last_valid = now
        if self.zero:
            try:
                calibration = self.zero.add(raw, status, now)
                if calibration:
                    calibration.update(board_serial=self.board_serial, density_kg_m3=1.225)
                    self.cal = calibration
                    self.cal_id = 'zero-calibration-' + stamp()
                    (self.directory / (self.cal_id + '.json')).write_text(json.dumps(calibration, indent=2) + '\n')
                    self.zero = None
                    self.emit('calibration', calibration=calibration, message='Zero complete')
            except ValueError as exc:
                self.zero = None
                self.emit('calibration', calibration=None, message=str(exc))
        pa, dp, speed, state = calculate(raw, status, self.cal)
        def number(value):
            return round(value, 4) if value is not None else ''
        row = dict(base, raw_counts=raw, sensor_status=status, pressure_pa=number(pa),
                   corrected_pressure_pa=number(dp), airspeed_m_s=number(speed),
                   airspeed_km_h=number(speed * 3.6 if speed is not None else None),
                   reading_state=state, calibration_id=self.cal_id)
        if self.cal:
            row.update(zero_offset_pa=self.cal['zero_offset_pa'],
                       zero_noise_band_pa=max(3 * self.cal['noise_std_pa'], 2 * QUANTUM_PA))
        self.log(row)

    def send_update(self):
        if self.pending:
            batch = self.pending
            self.pending = []
            # Only preview batches can be displaced; the writer has saved every row.
            if self.batches.full():
                try:
                    self.batches.get_nowait()
                except queue.Empty:
                    pass
            self.batches.put_nowait(batch)
        attempted = fresh = 0.0
        if len(self.recent) > 1:
            span = self.recent[-1][0] - self.recent[0][0]
            if span > 0:
                attempted = (len(self.recent) - 1) / span
                fresh = sum(x[1] for x in list(self.recent)[1:]) / span
        self.emit('snapshot', connected=bool(self.conn), version=self.version,
                  rate=self.applied_rate, attempted_hz=attempted, fresh_hz=fresh,
                  rows=self.writer.rows if self.writer else None,
                  missing=self.total_missing, last=self.last_row,
                  skipped=self.skipped_since_rate,
                  stale=time.monotonic() - self.last_sample > 5,
                  zero_count=len(self.zero.samples) if self.zero else None,
                  zero_elapsed=time.monotonic() - self.zero.started if self.zero else None)

    def run(self):
        try:
            while self.running:
                try:
                    while not self.commands.empty():
                        data = self.commands.get_nowait()
                        try:
                            self.command(data)
                        except (ValueError, OSError) as exc:
                            self.emit('error', message=str(exc))
                    if not self.running:
                        break
                    now = time.monotonic()
                    if self.conn is None and now >= self.next_connect:
                        self.next_connect = now + 3
                        self.connect()
                    if self.conn:
                        chunk = self.conn.read(max(1, min(self.conn.in_waiting, 65536)))
                        self.buffer += chunk
                        while b'\n' in self.buffer:
                            line, self.buffer = self.buffer.split(b'\n', 1)
                            if line:
                                self.handle_line(line.decode('ascii', errors='replace').strip())
                        if len(self.buffer) > 131072:
                            self.buffer = b''
                            self.notice('Malformed serial stream; incomplete packet discarded', 'framing_error')
                    else:
                        time.sleep(0.03)
                    if self.zero and now - self.zero.started > 45:
                        self.zero = None
                        self.emit('calibration', calibration=None, message='Zero timed out; wait for valid sensor readings and retry.')
                    if self.writer:
                        self.writer.flush_if_due()
                    if now - self.last_flush >= 0.1:
                        self.send_update()
                        self.last_flush = now
                except (serial.SerialException, OSError) as exc:
                    self.disconnect('USB unavailable: close Serial Monitor and check the cable. ' + str(exc))
        except Exception as exc:
            self.emit('fatal', message='Logging stopped: ' + str(exc))
        finally:
            if self.writer:
                self.writer.close()
            if self.conn:
                self.conn.close()
            self.send_update()
            self.emit('stopped')
