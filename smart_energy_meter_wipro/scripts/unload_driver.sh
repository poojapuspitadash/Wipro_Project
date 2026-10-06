#!/usr/bin/env bash
set -e
sudo rmmod smartmeter_driver 2>/dev/null || true
echo "Driver unloaded (if it was loaded)."
