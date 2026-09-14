#!/usr/bin/env bash
# fetch_rapidcheck.sh — vendor RapidCheck at a pinned commit.
#
# RapidCheck is the C++ property-based testing library prescribed by the design.
# It is not committed to this repo (see .gitignore); this script fetches it into
# ./rapidcheck at a known-good pinned commit so the host test build is
# reproducible. Safe to re-run: it is a no-op if the pinned commit is already
# checked out.
#
# Usage:  ./fetch_rapidcheck.sh
#
# Feature: pitot-airspeed-sensor — host test harness (task 4).
set -euo pipefail
cd "$(dirname "$0")"

REPO="https://github.com/emil-e/rapidcheck.git"
# Pinned to the RapidCheck HEAD at the time the harness was set up.
PIN="6e8dadfdafa3a74eabb52ead87f8787f72eccd0b"
DIR="rapidcheck"

if [ -d "$DIR/.git" ] && [ "$(git -C "$DIR" rev-parse HEAD 2>/dev/null || true)" = "$PIN" ]; then
  echo "RapidCheck already at pinned commit $PIN."
  exit 0
fi

rm -rf "$DIR"
echo "Fetching RapidCheck @ $PIN ..."
git clone "$REPO" "$DIR"
git -C "$DIR" checkout --quiet "$PIN"
echo "RapidCheck vendored at $DIR ($PIN)."
