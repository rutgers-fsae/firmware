import sys, csv, tempfile, unittest
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1] / 'logger'))
from airspeed_core import *

class Features(unittest.TestCase):
    def test_rate_limits(self):
        for rate in [1,20,500,1000]: self.assertEqual(validate_rate(rate),rate)
        for value in [0,1001,-1,'20.5','', 'twenty']:
            with self.assertRaises(ValueError): validate_rate(value)
    def test_protocol_new_old_and_failure(self):
        self.assertEqual(parse_line('D,4,200000,8220,0,500,2')['rate'],500)
        self.assertEqual(parse_line('F,5,202000,0,500,2')['kind'],'failure')
        self.assertTrue(parse_line('Received 2 bytes; raw = 8220; status = 0 (normal)')['legacy'])
        for line in ['D,1,1,20000,0,20,0','D,1,1,8220,9,20,0','D,1,1,8220,0,0,0','D,1,1']:
            with self.assertRaises(ValueError): parse_line(line)
    def test_microsecond_rollover_reset_and_gap(self):
        clock=DeviceClock()
        clock.update({'micros':0xfffffff0,'sequence':1},'2026-09-25T02:00:00+00:00')
        row,reset=clock.update({'micros':0x20,'sequence':3},'2026-09-25T02:00:01+00:00')
        self.assertFalse(reset); self.assertEqual(row['device_elapsed_s'],48/1e6)
        self.assertEqual(row['missing_serial_samples'],1)
        _,reset=clock.update({'micros':1,'sequence':1},'2026-09-25T02:00:02+00:00')
        self.assertTrue(reset)
    def test_zero_requires_time_not_just_twenty_fast_samples(self):
        zero=ZeroCalibration(now=0)
        for i in range(100): self.assertIsNone(zero.add(8220,0,now=i/1000))
        self.assertIsNotNone(zero.add(8220,0,now=5.1))
        zero=ZeroCalibration(now=0)
        for i in range(19): self.assertIsNone(zero.add(8220,0,now=i))
        self.assertIsNotNone(zero.add(8220,0,now=19))
    def test_named_runs_no_overwrite_utc_preserved_and_history(self):
        with tempfile.TemporaryDirectory() as directory:
            a=RunWriter(directory,'../Fan Test / A',100)
            sample={'received_utc':'2026-09-25T02:00:00.123+00:00','raw_counts':8220,'reading_state':'needs_zero'}
            for _ in range(1000): a.append(sample)
            a.close()
            b=RunWriter(directory,'../Fan Test / A',100); b.close()
            self.assertNotEqual(a.path,b.path)
            self.assertEqual(a.path.parent,Path(directory))
            self.assertIn('fan-test-a',a.path.name)
            rows=load_run(a.path)
            self.assertEqual(len(rows),1000)
            self.assertEqual(rows[0]['received_utc'],sample['received_utc'])
            self.assertEqual(rows[0]['run_title'],'../Fan Test / A')
            self.assertEqual(len(list_runs(directory)),2)
    def test_legacy_csv_still_loads(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / 'airspeed-legacy.csv'
            path.write_text('received_utc,raw_counts,sensor_status,pressure_pa,corrected_pressure_pa,airspeed_m_s,reading_state\n2026-01-01T12:00:00+00:00,8220,0,29.9854,0,0,within_zero_noise\n')
            self.assertEqual(len(load_run(path)), 1)
            self.assertEqual(list_runs(directory)[0]['path'], path)
    def test_twelve_hour_time(self):
        for iso in ['2026-09-25T00:01:00+00:00','2026-09-25T15:01:00+00:00']:
            value=format_local(iso,True)
            self.assertRegex(value,r'^\d{1,2}:\d\d:\d\d\.\d{3} (AM|PM)$')
            self.assertTrue(1<=int(value.split(':')[0])<=12)
    def test_plot_preserves_peak_and_missing_gap(self):
        rows=[]
        for i in range(10000):
            rows.append({'_plot_time':i,'airspeed_m_s':100 if i==523 else 1})
        rows[5000]['airspeed_m_s']=''
        segments=plot_segments(rows,'airspeed_m_s',100)
        self.assertEqual(len(segments),2)
        self.assertEqual(max(p[1] for s in segments for p in s),100)
        self.assertLess(sum(map(len,segments)),150)

if __name__=='__main__': unittest.main()
