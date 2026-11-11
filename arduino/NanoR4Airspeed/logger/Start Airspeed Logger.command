#!/bin/zsh
cd -- "${0:A:h}"
if [[ -n "${AIRSPEED_PYTHON:-}" ]]; then
  python_runtime="$AIRSPEED_PYTHON"
elif [[ -x .venv/bin/python3 ]]; then
  python_runtime="./.venv/bin/python3"
else
  python_runtime="python3"
fi
exec "$python_runtime" airspeed_logger.py "$@"
