from __future__ import annotations

import numpy as np

from offline_daq_viewer.data.downsample import downsample_min_max


def test_downsample_leaves_small_data_unchanged() -> None:
    x = np.arange(10, dtype=float)
    y = x * 2

    result = downsample_min_max(x, y, max_points=50)

    assert result.original_points == 10
    assert result.plotted_points == 10
    np.testing.assert_array_equal(result.x, x)
    np.testing.assert_array_equal(result.y, y)


def test_downsample_bounds_large_data() -> None:
    x = np.arange(100_000, dtype=float)
    y = np.sin(x / 100)

    result = downsample_min_max(x, y, max_points=10_000)

    assert result.original_points == 100_000
    assert result.plotted_points <= 10_000
    assert result.x.shape == result.y.shape

