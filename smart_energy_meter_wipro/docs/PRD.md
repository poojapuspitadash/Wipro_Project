# Project Requirements Document (PRD)

## 1. Project title
Smart Energy Meter – Pulse Counter & Analytics Agent

## 2. Objective
Build a software-only smart energy meter prototype using Linux device-driver
concepts, C++ system programming and a simple dashboard. Simulated pulses represent
the output of a real meter.

## 3. Functional requirements
1. Count meter pulses.
2. Expose a Linux character device `/dev/smartmeter`.
3. Support reset and pulse-count retrieval through IOCTL.
4. Accept simulated pulses.
5. Convert pulses to Wh and kWh.
6. Estimate instantaneous power.
7. Raise an alert when configured power threshold is reached.
8. Expose live values to a dashboard.
9. Log measurements to CSV.
10. Provide reset and pulse-injection controls.

## 4. Non-functional requirements
- Linux compatible.
- C++17 user-space application.
- Thread-safe counter access.
- Clear modular architecture.
- Buildable using Make.
- No mandatory third-party runtime for the dashboard.
- Graceful fallback when the kernel driver cannot be loaded.

## 5. Deliverables
- Driver source and Makefile
- C++ application
- Frontend
- Configuration
- Tests
- UML/architecture documents
- Test report
- Final report and demo guide
- Build/run scripts

## 6. Scope
Included: simulated pulses, Linux driver, C++ application, analytics, alerting,
dashboard and logging.

Excluded: physical smart-meter hardware, production authentication/TLS and
commercial meter certification.

## 7. Timeline
- Day 1: requirements and architecture
- Day 2: driver and C++ integration
- Day 3: dashboard and logging
- Day 4: testing and debugging
- Day 5: final documentation and demo rehearsal
