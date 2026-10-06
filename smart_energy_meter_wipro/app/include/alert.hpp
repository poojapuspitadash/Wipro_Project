#pragma once
#include <string>

class AlertManager {
public:
    explicit AlertManager(double threshold_w = 3000.0);
    bool isHighPower(double power_w) const;
    std::string message(double power_w) const;
    double threshold() const;
private:
    double threshold_w_;
};
