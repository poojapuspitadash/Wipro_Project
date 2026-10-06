# Final Project Report — Smart Energy Meter

## 1. Abstract

The Smart Energy Meter is a software-only energy monitoring prototype developed for Linux Device Drivers, System Programming and C++ Programming training. Simulated meter pulses represent the input of a physical energy meter. A Linux character driver maintains the pulse counter and exposes `/dev/smartmeter`. A C++ user-space application accesses the device through POSIX file operations and ioctl commands, calculates energy and instantaneous power, evaluates a high-consumption threshold, records measurements and serves a browser dashboard.

## 2. Objectives

- Implement a Linux character device driver.
- Count and inject simulated meter pulses.
- Provide RESET, GET_COUNT and SET_COUNT ioctl operations.
- Build a C++ system-programming application.
- Calculate Wh, kWh and instantaneous power.
- Generate high-power alerts at the configured threshold.
- Provide a live dashboard with charts and controls.
- Record readings in CSV format.
- Test the application in simulator and Linux-driver modes.

## 3. Architecture

Pulse Input → Linux Character Driver → `/dev/smartmeter` → C++ Application → Energy/Power Analytics → Alert Manager → HTTP Dashboard.

## 4. Linux driver

The driver uses a character device, a mutex-protected pulse counter and ioctl commands. User space can inject pulse counts with `write()` for the software-only prototype.

## 5. C++ application

The application is divided into `MeterDevice`, `EnergyCalculator`, `AlertManager`, configuration handling and `HttpServer`. It uses `open()`, `write()`, `ioctl()`, file I/O, threads and POSIX signal handling.

## 6. Dashboard

The dashboard presents the requested Smart Energy Monitor flow: system status, pulse count, energy, power, high-power status, live power graph, energy graph, driver/device information, meter controls and CSV export.

## 7. Limitation

The project uses simulated pulses rather than a physical smart meter sensor. A future version can replace the software pulse input with a real sensor/interrupt source while keeping the same user-space analytics and dashboard architecture.
