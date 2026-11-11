#!/usr/bin/env bash
set -euo pipefail
if [[ ${1:-} == --help ]]; then
    echo "Usage: sudo bash deploy/install-rpi.sh [--no-start]"
    exit 0
fi
[[ $# -eq 0 || ( $# -eq 1 && $1 == --no-start ) ]] || { echo "Unknown argument" >&2; exit 1; }
[[ $EUID -eq 0 && $(uname -s) == Linux ]] || { echo "Run with sudo on Raspberry Pi OS" >&2; exit 1; }
SOURCE_DIR="$(cd "$(dirname "$0")/.." && pwd)"
apt-get update
apt-get install -y python3-venv i2c-tools
raspi-config nonint do_i2c 0
getent group i2c >/dev/null || groupadd --system i2c
getent group vectornav >/dev/null || groupadd --system vectornav
id airspeed >/dev/null 2>&1 || useradd --system --user-group --home /opt/airspeed --shell /usr/sbin/nologin airspeed
usermod --append --groups i2c airspeed
systemctl stop airspeed-logger.service 2>/dev/null || true
install -d /opt/airspeed/logger /opt/airspeed/deploy
install -d -o airspeed -g airspeed /var/lib/airspeed /var/lib/airspeed/logs
if [[ "$SOURCE_DIR" != /opt/airspeed ]]; then
    install -m 0644 "$SOURCE_DIR"/logger/airspeed_{core,pi}.py /opt/airspeed/logger/
    install -m 0644 "$SOURCE_DIR"/deploy/requirements-pi.txt /opt/airspeed/deploy/
fi
python3 -m venv /opt/airspeed/venv
/opt/airspeed/venv/bin/python -m pip install -r /opt/airspeed/deploy/requirements-pi.txt
install -m 0644 "$SOURCE_DIR/deploy/airspeed-logger.service" /etc/systemd/system/
if [[ ! -f /etc/default/airspeed-logger ]]; then
    install -m 0644 "$SOURCE_DIR/deploy/airspeed-logger.default" /etc/default/airspeed-logger
fi
systemctl daemon-reload
systemctl enable airspeed-logger.service
if [[ ${1:-} != --no-start && -e /dev/i2c-1 ]]; then
    systemctl restart airspeed-logger.service
else
    echo "Installed and enabled. Reboot if I2C was just enabled; then start airspeed-logger."
fi
echo "Settings: /etc/default/airspeed-logger"
echo "Logs: journalctl -u airspeed-logger -f"
