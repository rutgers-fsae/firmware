"""Measurement math, serial protocol, and run files (no GUI or hardware access)."""
import csv
import json
import math
import re
import statistics
import time
import unicodedata
from datetime import datetime, timedelta, timezone
from pathlib import Path

QUANTUM_PA = 2 * 6894.757 / (0.8 * 16383)
MAX_RATE = 1000
FIELDS = ['received_utc', 'elapsed_s', 'raw_counts', 'sensor_status', 'pressure_pa',
          'zero_offset_pa', 'corrected_pressure_pa', 'density_kg_m3',
          'zero_noise_band_pa', 'airspeed_m_s', 'airspeed_km_h', 'reading_state',
          'calibration_id', 'source_line', 'run_title', 'sample_utc',
          'device_sequence', 'device_micros', 'device_elapsed_s',
          'requested_rate_hz', 'missing_serial_samples', 'device_skipped_slots']
LEGACY = re.compile(r'^Received 2 bytes; raw = (\d+); status = ([0-3])\b')


def utc():
    return datetime.now(timezone.utc).isoformat(timespec='milliseconds')


def stamp():
    return datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')


def slugify(title):
    ascii_title = unicodedata.normalize('NFKD', title).encode('ascii', 'ignore').decode()
    return re.sub(r'[^a-z0-9]+', '-', ascii_title.lower()).strip('-')[:70] or 'untitled'


def validate_rate(value):
    text = str(value).strip()
    if not text.isdigit() or not 1 <= int(text) <= MAX_RATE:
        raise ValueError('Choose a whole number from 1 to 1,000 reads per second.')
    return int(text)


def pressure(raw):
    return ((raw - 0.1 * 16383) / (0.8 * 16383) * 2 - 1) * 6894.757


def calculate(raw, status, calibration, density=1.225):
    if status != 0:
        return None, None, None, 'stale' if status == 2 else 'sensor_error'
    if not 1638.3 <= raw <= 14744.7:
        return None, None, None, 'out_of_range'
    pa = pressure(raw)
    if calibration is None:
        return pa, None, None, 'needs_zero'
    dp = pa - calibration['zero_offset_pa']
    band = max(3 * calibration['noise_std_pa'], 2 * QUANTUM_PA)
    if abs(dp) <= band:
        return pa, dp, 0.0, 'within_zero_noise'
    if dp < 0:
        return pa, dp, None, 'negative_pressure_check_tubing'
    return pa, dp, math.sqrt(2 * dp / density), 'valid'


def parse_line(line):
    """Accept the new compact protocol and previous human-readable sketch."""
    parts = line.split(',')
    if parts[0] in ('D', 'F'):
        expected = 7 if parts[0] == 'D' else 6
        if len(parts) != expected:
            raise ValueError('Incomplete data packet')
        nums = [int(x) for x in parts[1:]]
        if any(x < 0 for x in nums) or any(x > 0xFFFFFFFF for x in nums[:2]):
            raise ValueError('Invalid unsigned sample data')
        seq, micros = nums[:2]
        if parts[0] == 'D':
            raw, status, rate, skipped = nums[2:]
            if raw > 16383 or status > 3:
                raise ValueError('Invalid pressure or status bits')
            result = {'kind': 'sample', 'raw': raw, 'status': status}
        else:
            received, rate, skipped = nums[2:]
            result = {'kind': 'failure', 'received': received}
        validate_rate(rate)
        return dict(result, sequence=seq, micros=micros, rate=rate, skipped=skipped)
    if parts[0] == 'HELLO' and len(parts) == 4:
        return {'kind': 'hello', 'version': int(parts[1]), 'maximum': int(parts[2]), 'rate': validate_rate(parts[3])}
    if parts[0] == 'RATE_OK' and len(parts) == 2:
        return {'kind': 'rate', 'rate': validate_rate(parts[1])}
    if parts[0] in ('RATE_ERROR', 'COMMAND_ERROR'):
        return {'kind': 'command_error'}
    if line.startswith('I2C_RECOVERY '):
        return {'kind': 'recovery'}
    match = LEGACY.match(line)
    if match:
        raw, status = map(int, match.groups())
        return {'kind': 'sample', 'raw': raw, 'status': status, 'legacy': True, 'rate': 1}
    if line.startswith('READ FAILED'):
        return {'kind': 'failure', 'legacy': True, 'rate': 1}
    return {'kind': 'unknown'}


class DeviceClock:
    """Unwrap the 32-bit micros counter; detect restarts and sequence gaps."""
    def __init__(self):
        self.last_us = self.last_seq = self.anchor = None
        self.elapsed_us = 0

    def update(self, packet, received):
        if 'micros' not in packet:
            return {'sample_utc': received, 'missing_serial_samples': 0}, False
        now_us, seq = packet['micros'], packet['sequence']
        reset = False
        missing = 0
        if self.last_us is None:
            self.anchor = datetime.fromisoformat(received)
        else:
            delta = (now_us - self.last_us) & 0xFFFFFFFF
            seq_delta = (seq - self.last_seq) & 0xFFFFFFFF
            if delta > 0x7FFFFFFF or seq_delta > 0x7FFFFFFF:
                reset = True
                self.elapsed_us = 0
                self.anchor = datetime.fromisoformat(received)
            else:
                self.elapsed_us += delta
                missing = max(0, seq_delta - 1)
        self.last_us, self.last_seq = now_us, seq
        sample = self.anchor + timedelta(microseconds=self.elapsed_us)
        return {'sample_utc': sample.isoformat(timespec='microseconds'),
                'device_sequence': seq, 'device_micros': now_us,
                'device_elapsed_s': self.elapsed_us / 1e6,
                'missing_serial_samples': missing,
                'device_skipped_slots': packet.get('skipped', '')}, reset


class ZeroCalibration:
    """At least 20 fresh samples spanning at least five seconds."""
    def __init__(self, now=None):
        self.started = time.monotonic() if now is None else now
        self.samples = []

    def add(self, raw, status, now=None):
        now = time.monotonic() if now is None else now
        if now - self.started > 45:
            raise ValueError('Zero timed out. Wait for fresh readings, then try again.')
        if status != 0 or not 1638.3 <= raw <= 14744.7:
            return None
        self.samples.append(pressure(raw))
        if len(self.samples) < 20 or now - self.started < 5:
            return None
        noise = statistics.stdev(self.samples)
        if noise > 10:
            raise ValueError('Zero was too noisy (>10 Pa standard deviation). Keep the probe still and retry.')
        return {'zero_offset_pa': statistics.mean(self.samples), 'noise_std_pa': noise,
                'sample_count': len(self.samples), 'duration_s': now - self.started,
                'created_utc': utc()}


class RunWriter:
    def __init__(self, directory, title, rate):
        self.directory = Path(directory)
        self.title = ' '.join(title.strip().split())[:100] or 'Untitled run'
        self.started = time.monotonic()
        name = f'airspeed-{slugify(self.title)}-{stamp()}'
        self.path = self.directory / (name + '.csv')
        self.file = self.path.open('x', newline='', encoding='utf-8')
        self.writer = csv.DictWriter(self.file, fieldnames=FIELDS)
        self.writer.writeheader()
        self.rows = 0
        self.last_flush = time.monotonic()
        self.metadata_path = self.path.with_suffix('.run.json')
        self.metadata = {'title': self.title, 'started_utc': utc(), 'requested_rate_hz': rate,
                         'csv': self.path.name, 'ended_utc': None}
        self.metadata_path.write_text(json.dumps(self.metadata, indent=2) + '\n')
        self.file.flush()

    def append(self, row):
        data = dict.fromkeys(FIELDS, '')
        data.update(row)
        # Titles are human text, never spreadsheet formulas.
        title = self.title
        if title.startswith(('=', '+', '-', '@')):
            title = "'" + title
        data.update(run_title=title, elapsed_s=round(time.monotonic() - self.started, 6))
        try:
            self.writer.writerow({key: data[key] for key in FIELDS})
        except OSError as exc:
            raise RuntimeError(f'Cannot write recording {self.path.name}: {exc}') from exc
        self.rows += 1
        self.flush_if_due()

    def flush_if_due(self):
        if time.monotonic() - self.last_flush >= 1:
            try:
                self.file.flush()
            except OSError as exc:
                raise RuntimeError(f'Cannot flush recording {self.path.name}: {exc}') from exc
            self.last_flush = time.monotonic()

    def close(self):
        if self.file.closed:
            return
        self.file.flush()
        self.file.close()
        self.metadata.update(ended_utc=utc(), rows=self.rows)
        self.metadata_path.write_text(json.dumps(self.metadata, indent=2) + '\n')


def format_local(value, milliseconds=False):
    if not value:
        return '—'
    moment = datetime.fromisoformat(value).astimezone()
    text = moment.strftime('%I:%M:%S').lstrip('0')
    if milliseconds:
        text += f'.{moment.microsecond // 1000:03d}'
    return text + moment.strftime(' %p')


def row_time(row):
    if '_plot_time' in row:
        return row['_plot_time']
    return datetime.fromisoformat(row.get('sample_utc') or row['received_utc']).timestamp()


def load_run(path):
    with Path(path).open(newline='', encoding='utf-8') as source:
        return list(csv.DictReader(source))


def list_runs(directory):
    runs = []
    for path in Path(directory).glob('airspeed-*.csv'):
        try:
            metadata_path = path.with_suffix('.run.json')
            if metadata_path.exists():
                metadata = json.loads(metadata_path.read_text())
                title, started = metadata['title'], metadata['started_utc']
            else:
                with path.open(newline='') as source:
                    first = next(csv.DictReader(source), None)
                started = first['received_utc'] if first else datetime.fromtimestamp(path.stat().st_mtime, timezone.utc).isoformat()
                title = (first or {}).get('run_title') or 'Untitled run'
            moment = datetime.fromisoformat(started).astimezone()
            label = f"{title} · {moment.strftime('%b %d')} {format_local(started)}"
            runs.append({'path': path, 'title': title, 'started': started, 'label': label})
        except (ValueError, KeyError, OSError, csv.Error):
            continue
    return sorted(runs, key=lambda r: r['started'], reverse=True)


def plot_segments(rows, field, maximum_points=1800):
    """Preserve gaps and bin extrema; logging itself never uses this reduction."""
    segments, current = [], []
    for row in rows:
        try:
            value = row.get(field, '')
            if value in ('', None):
                raise ValueError()
            point = (row_time(row), float(value))
            if not all(math.isfinite(x) for x in point):
                raise ValueError()
            current.append(point)
        except (ValueError, KeyError, TypeError):
            if current:
                segments.append(current)
                current = []
    if current:
        segments.append(current)
    total = sum(map(len, segments))
    step = max(1, math.ceil(total / max(1, maximum_points // 4)))
    if step == 1:
        return segments
    reduced = []
    for segment in segments:
        points = []
        for offset in range(0, len(segment), step):
            chunk = segment[offset:offset + step]
            selected = {0, len(chunk) - 1, min(range(len(chunk)), key=lambda i: chunk[i][1]), max(range(len(chunk)), key=lambda i: chunk[i][1])}
            points.extend(chunk[i] for i in sorted(selected))
        reduced.append(points)
    return reduced
