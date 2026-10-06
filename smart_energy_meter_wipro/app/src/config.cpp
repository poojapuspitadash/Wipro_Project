#include "config.hpp"
#include <fstream>
#include <algorithm>
#include <cctype>

static std::string trim(std::string s) {
    auto notspace = [](unsigned char c){ return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notspace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notspace).base(), s.end());
    return s;
}

AppConfig loadConfig(const std::string& path) {
    AppConfig c;
    std::ifstream in(path);
    if (!in) return c;

    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        std::string k = trim(line.substr(0, pos));
        std::string v = trim(line.substr(pos + 1));
        try {
            if (k == "PULSES_PER_KWH") c.pulses_per_kwh = std::stod(v);
            else if (k == "ALERT_POWER_W") c.alert_power_w = std::stod(v);
            else if (k == "SAMPLE_INTERVAL_MS") c.sample_interval_ms = std::stoi(v);
            else if (k == "LOG_FILE") c.log_file = v;
            else if (k == "WEB_PORT") c.web_port = std::stoi(v);
        } catch (...) {}
    }
    return c;
}
