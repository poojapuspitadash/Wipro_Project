#!/usr/bin/env bash
set -euo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"
make app
if [[ -e /dev/smartmeter ]]; then
  echo "Using Linux driver: /dev/smartmeter"
  ./build/smartmeter_app --port 8080
else
  echo "Driver device not available; starting simulator demo."
  ./build/smartmeter_app --simulate --port 8080
fi
