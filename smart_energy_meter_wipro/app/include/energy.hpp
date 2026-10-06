#pragma once
#include <cstdint>

struct EnergySnapshot {
    unsigned long pulses{0};
    double energy_wh{0.0};
    double energy_kwh{0.0};
    double power_w{0.0};
};

class EnergyCalculator {
public:
    explicit EnergyCalculator(double pulses_per_kwh = 1000.0);
    EnergySnapshot calculate(unsigned long pulses, double elapsed_seconds) const;
private:
    double pulses_per_kwh_;
};
