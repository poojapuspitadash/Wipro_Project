#!/usr/bin/env bash
set -e
echo "Running kernel: $(uname -r)"
KDIR="/lib/modules/$(uname -r)/build"
if [[ -d "$KDIR" ]]; then
  echo "Matching kernel build tree found: $KDIR"
else
  echo "WARNING: matching kernel build tree not found: $KDIR"
  echo "Use simulator mode or install/build a matching kernel environment."
  exit 1
fi
