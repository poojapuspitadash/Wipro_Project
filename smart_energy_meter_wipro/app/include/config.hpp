#pragma once
#include <string>
#include <unordered_map>

struct AppConfig {
    double pulses_per_kwh{1000.0};
    double alert_power_w{3000.0};
    int sample_interval_ms{1000};
    std::string log_file{"logs/meter.csv"};
    int web_port{8080};
};

AppConfig loadConfig(const std::string& path);
