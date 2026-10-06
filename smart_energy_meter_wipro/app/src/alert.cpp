#include "alert.hpp"
#include <sstream>
#include <iomanip>

AlertManager::AlertManager(double threshold_w)
    : threshold_w_(threshold_w > 0 ? threshold_w : 3000.0) {}

bool AlertManager::isHighPower(double power_w) const {
    return power_w >= threshold_w_;
}

std::string AlertManager::message(double power_w) const {
    std::ostringstream out;
    out << std::fixed << std::setprecision(1);
    if (isHighPower(power_w))
        out << "HIGH POWER ALERT: " << power_w << " W >= " << threshold_w_ << " W";
    else
        out << "Normal consumption: " << power_w << " W";
    return out.str();
}

double AlertManager::threshold() const { return threshold_w_; }
