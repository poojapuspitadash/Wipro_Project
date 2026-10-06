#include "energy.hpp"
#include <algorithm>

EnergyCalculator::EnergyCalculator(double pulses_per_kwh)
    : pulses_per_kwh_(pulses_per_kwh > 0 ? pulses_per_kwh : 1000.0) {}

EnergySnapshot EnergyCalculator::calculate(unsigned long pulses,
                                            double elapsed_seconds) const {
    EnergySnapshot s;
    s.pulses = pulses;
    s.energy_kwh = static_cast<double>(pulses) / pulses_per_kwh_;
    s.energy_wh = s.energy_kwh * 1000.0;

    if (elapsed_seconds > 0.0) {
        // Delta-based instantaneous estimate is supplied by the caller through
        // the pulse count delta; total power is not inferred from lifetime energy.
        s.power_w = 0.0;
    }
    return s;
}
