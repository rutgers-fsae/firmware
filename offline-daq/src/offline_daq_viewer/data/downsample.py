from __future__ import annotations

from dataclasses import dataclass

import numpy as np


@dataclass(frozen=True)
class DownsampleResult:
    x: np.ndarray
    y: np.ndarray
    original_points: int
    plotted_points: int


def downsample_min_max(
    x: np.ndarray,
    y: np.ndarray,
    max_points: int = 50_000,
) -> DownsampleResult:
    """Reduce large data to min/max buckets while preserving extrema."""
    if max_points < 3:
        raise ValueError("max_points must be at least 3")

    x = np.asarray(x)
    y = np.asarray(y)

    if x.shape != y.shape:
        raise ValueError("x and y must have the same shape")

    original_points = int(x.size)
    if original_points <= max_points:
        return DownsampleResult(x=x, y=y, original_points=original_points, plotted_points=original_points)

    bucket_count = max(1, max_points // 2)
    edges = np.linspace(0, original_points, bucket_count + 1, dtype=np.int64)
    sampled_x: list[float] = []
    sampled_y: list[float] = []

    for start, stop in zip(edges[:-1], edges[1:]):
        if stop <= start:
            continue
        segment_y = y[start:stop]
        if segment_y.size == 0:
            continue

        min_offset = int(np.nanargmin(segment_y))
        max_offset = int(np.nanargmax(segment_y))
        indexes = [start + min_offset, start + max_offset]
        indexes.sort()

        for index in indexes:
            sampled_x.append(x[index])
            sampled_y.append(y[index])

    downsampled_x = np.asarray(sampled_x)
    downsampled_y = np.asarray(sampled_y)
    return DownsampleResult(
        x=downsampled_x,
        y=downsampled_y,
        original_points=original_points,
        plotted_points=int(downsampled_x.size),
    )

