#!/usr/bin/env bash
# run_tests.sh — one-shot build + run of the Airspeed26 host test harness.
#
# Convenience wrapper around `make test`. Runs the property/unit tests exactly
# once (no watch mode) and exits non-zero if the build or any test fails.
#
# Usage:  ./run_tests.sh
#
# Feature: pitot-airspeed-sensor — host test harness (task 4).
set -euo pipefail
cd "$(dirname "$0")"
make test
