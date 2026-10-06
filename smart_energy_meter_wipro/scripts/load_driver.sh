#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

echo "[1/4] Checking kernel environment..."
./scripts/check_kernel.sh

echo "[2/4] Building Linux kernel driver..."
make driver

echo "[3/4] Loading Smart Energy Meter driver..."
if lsmod | grep -q '^smartmeter'; then
  echo "Driver already loaded. Unloading old instance..."
  sudo rmmod smartmeter_driver || true
fi
sudo insmod driver/smartmeter_driver.ko
sleep 1

echo "[4/4] Checking /dev/smartmeter..."
if [[ -e /dev/smartmeter ]]; then
  sudo chmod 666 /dev/smartmeter
  echo "Driver loaded successfully."
  echo "Device: /dev/smartmeter"
  echo "Permissions:"
  ls -l /dev/smartmeter
else
  echo "ERROR: /dev/smartmeter was not created."
  dmesg | tail -30 || true
  exit 2
fi
