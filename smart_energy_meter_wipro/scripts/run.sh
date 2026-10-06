#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

make app

if [[ -e /dev/smartmeter ]]; then
  if [[ ! -r /dev/smartmeter || ! -w /dev/smartmeter ]]; then
    echo "Granting user access to /dev/smartmeter..."
    sudo chmod 666 /dev/smartmeter
  fi
  echo "Starting Smart Energy Meter in LINUX DRIVER mode..."
  exec ./build/smartmeter_app
else
  echo "WARNING: /dev/smartmeter is unavailable."
  echo "Starting in SIMULATOR mode."
  exec ./build/smartmeter_app --simulate
fi
