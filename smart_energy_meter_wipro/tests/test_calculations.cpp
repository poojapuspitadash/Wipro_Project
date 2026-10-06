#include "energy.hpp"
#include "alert.hpp"
#include <cassert>
#include <cmath>
#include <iostream>

int main() {
    EnergyCalculator calc(1000);
    auto s = calc.calculate(1000, 1.0);
    assert(std::abs(s.energy_kwh - 1.0) < 1e-9);
    assert(std::abs(s.energy_wh - 1000.0) < 1e-9);

    AlertManager alerts(3000);
    assert(!alerts.isHighPower(2999.9));
    assert(alerts.isHighPower(3000.0));

    std::cout << "All calculation and alert unit tests passed.\n";
    return 0;
}
