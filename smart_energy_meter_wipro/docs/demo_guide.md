# Final Demo Guide

## Target dashboard flow

`SMART ENERGY MONITOR → System Online → Pulse Count → Energy → Power → Status → Live Graphs → System Information → Controls → Export CSV`

## Demo sequence

1. Build the application:
   ```bash
   make
   ```
2. Load the driver:
   ```bash
   ./scripts/load_driver.sh
   ```
3. Start the complete application:
   ```bash
   ./scripts/run.sh
   ```
4. Open `http://localhost:8080`.
5. Confirm **Driver: Connected**, **Device: /dev/smartmeter**, and **Mode: LINUX_DRIVER**.
6. Click **Add 10 Pulses** or **Add 100 Pulses**.
7. Observe pulse count, kWh, instantaneous power, alert status and graphs update.
8. Cross the configured 3000 W threshold to demonstrate the **HIGH** alert.
9. Click **Reset** and verify the meter returns to zero.
10. Click **Export CSV** and show the recorded measurements.

## What to explain

- The Linux character driver owns the pulse counter.
- C++ uses `open()` and `ioctl()` to read/reset the counter and `write()` to inject software pulses.
- Energy is calculated from pulses using the configured pulses-per-kWh ratio.
- Power is estimated from pulse delta over the sampling interval.
- The alert manager compares power with the 3000 W threshold.
- The C++ socket server exposes status and control APIs to the browser.
- The dashboard visualizes live data without requiring a separate web framework.

## Simulator fallback

If the Linux driver cannot be loaded on a particular machine, run:

```bash
./build/smartmeter_app --simulate
```

The same analytics and dashboard remain available, while the System Information panel explicitly shows `SIMULATOR` rather than pretending the driver is connected.
