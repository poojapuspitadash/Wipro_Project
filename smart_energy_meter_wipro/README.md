# Smart Energy Meter — Wipro Training Project

A software-only Smart Energy Meter prototype using **Linux Character Device Driver + C++ System Programming + Web Dashboard**. Simulated pulses are supported when real meter hardware is unavailable.

## Final flow

`Pulse Input → Linux Character Driver → /dev/smartmeter → C++ Application → Energy/Power Analytics → Alert Manager → HTTP Dashboard`

## Dashboard

The dashboard follows the requested Smart Energy Monitor flow:

- System Online indicator
- Pulse Count
- Energy (kWh / Wh)
- Live Power (W)
- Normal / High status with 3000 W threshold
- Live power graph
- Energy consumption graph
- Driver/device/mode/system information
- Add 1 / 10 / 100 / 1000 pulses
- Reset
- Export CSV

## 1. Build

```bash
sudo apt update
sudo apt install -y build-essential gcc g++ make
cd smart_energy_meter_wipro
make
```

## 2. Load the Linux driver

Use a kernel whose headers match `uname -r` (important for WSL kernel modules):

```bash
./scripts/load_driver.sh
```

The script builds the module, loads it, verifies `/dev/smartmeter`, and sets temporary user access with `chmod 666`.

Check:

```bash
ls -l /dev/smartmeter
```

Expected permissions include `crw-rw-rw-`.

## 3. Run the complete project

The easiest method is:

```bash
./scripts/run.sh
```

Or from the project root:

```bash
./build/smartmeter_app
```

Open:

`http://localhost:8080`

The dashboard should show **Driver: Connected** and **Mode: LINUX_DRIVER**.

## 4. Driver-only checks

```bash
echo 10 | sudo tee /dev/smartmeter
cat /dev/smartmeter
```

Or run the C++ ioctl test:

```bash
make test-ioctl
```

It verifies SET_COUNT, GET_COUNT and RESET.

## 5. Simulator mode

If `/dev/smartmeter` is unavailable:

```bash
./build/smartmeter_app --simulate
```

The simulator starts with a visible reading and generates a small live pulse stream so the dashboard graphs are populated.

## 6. Dashboard controls

- **Add Pulses** writes to the real driver in Linux-driver mode and updates the simulator counter in simulator mode.
- **Reset** calls `SMARTMETER_RESET` in driver mode.
- **Export CSV** downloads the application log.

## 7. Configuration

Edit `config/config.conf`:

```text
PULSES_PER_KWH=1000
ALERT_POWER_W=3000
SAMPLE_INTERVAL_MS=1000
LOG_FILE=logs/meter.csv
WEB_PORT=8080
```

## 8. Testing

```bash
make tests
make test-ioctl
```

For the complete system demonstration, verify the chain:

`driver → device node → C++ ioctl → analytics → alert → dashboard`

## Important WSL note

If `insmod` reports `Invalid module format` or `module_layout` mismatch, the module was built against a different kernel tree than the running WSL kernel. Check:

```bash
uname -r
make -C /lib/modules/$(uname -r)/build kernelrelease
```

Those values must match before rebuilding the driver.
