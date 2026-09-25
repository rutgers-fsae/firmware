import importlib.util
import math
from pathlib import Path
import unittest

import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'logger'))
import airspeed_core as logger


class CalculationTests(unittest.TestCase):
    def test_transfer_function_endpoints(self):
        self.assertAlmostEqual(logger.pressure(1638.3), -6894.757)
        self.assertAlmostEqual(logger.pressure(14744.7), 6894.757)
        self.assertAlmostEqual(logger.pressure(8191.5), 0)

    def test_no_speed_before_zero_or_for_stale_data(self):
        self.assertEqual(logger.calculate(8220, 0, None, 1.225)[2:], (None, 'needs_zero'))
        self.assertEqual(logger.calculate(8220, 2, None, 1.225)[2:], (None, 'stale'))

    def test_known_dynamic_pressure(self):
        calibration = {'zero_offset_pa': logger.pressure(8300) - 100, 'noise_std_pa': 1}
        pa, dp, speed, state = logger.calculate(8300, 0, calibration, 1.225)
        self.assertEqual(dp, 100)
        self.assertAlmostEqual(speed, 12.7775312999988)
        self.assertEqual(state, 'valid')

    def test_negative_pressure_does_not_become_positive_speed(self):
        cal = {'zero_offset_pa': logger.pressure(8000) + 100, 'noise_std_pa': 1}
        self.assertEqual(logger.calculate(8000, 0, cal, 1.225)[2:], (None, 'negative_pressure_check_tubing'))

    def test_noise_is_flagged_and_out_of_range_is_rejected(self):
        cal = {'zero_offset_pa': logger.pressure(8220), 'noise_std_pa': 4}
        self.assertEqual(logger.calculate(8220, 0, cal, 1.225)[2:], (0.0, 'within_zero_noise'))
        self.assertEqual(logger.calculate(16383, 0, cal, 1.225)[2:], (None, 'out_of_range'))


if __name__ == '__main__':
    unittest.main()
