#include "alert.hpp"
#include "config.hpp"
#include "energy.hpp"
#include "http_server.hpp"
#include "meter_device.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <csignal>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

static std::atomic<bool> running{true};
static void onSignal(int) { running = false; }

class MeterService {
public:
    MeterService(AppConfig cfg, bool force_simulate, std::string device)
        : cfg_(std::move(cfg)), force_simulate_(force_simulate),
          device_path_(std::move(device)), device_(device_path_),
          calc_(cfg_.pulses_per_kwh), alerts_(cfg_.alert_power_w) {}

    bool start() {
        std::filesystem::path logPath(cfg_.log_file);
        if (logPath.has_parent_path()) std::filesystem::create_directories(logPath.parent_path());
        log_.open(cfg_.log_file, std::ios::app);
        if (!log_) {
            std::cerr << "Warning: cannot open log file " << cfg_.log_file << "\n";
        } else if (log_.tellp() == std::streampos(0)) {
            log_ << "timestamp,pulses,energy_wh,energy_kwh,power_w,alert,mode\n";
        }

        if (force_simulate_) {
            simulate_ = true;
            return true;
        }

        if (!device_.openDevice()) {
            std::cerr << "Cannot open " << device_path_
                      << ". Switching to simulator mode.\n";
            simulate_ = true;
            return true;
        }

        simulate_ = false;
        return true;
    }

    void loop() {
        using clock = std::chrono::steady_clock;
        auto previous_time = clock::now();
        unsigned long previous_pulses = readPulses();

        // Start simulator with a realistic visible reading. In driver mode the
        // counter starts at whatever the kernel driver reports.
        if (simulate_) {
            std::lock_guard<std::mutex> lock(mu_);
            sim_pulses_ = 590;
            previous_pulses = sim_pulses_;
            snapshot_.pulses = sim_pulses_;
            snapshot_.energy_kwh = static_cast<double>(sim_pulses_) / cfg_.pulses_per_kwh;
            snapshot_.energy_wh = snapshot_.energy_kwh * 1000.0;
        }

        while (running.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(cfg_.sample_interval_ms));
            auto now = clock::now();
            double dt = std::chrono::duration<double>(now - previous_time).count();
            unsigned long pulses = readPulses();

            // In simulator mode, generate approximately one pulse per second so
            // the dashboard has a live power signal. Driver mode changes only when
            // the user injects pulses through the dashboard or device.
            if (simulate_) {
                std::lock_guard<std::mutex> lock(mu_);
                sim_pulses_ += 1;
                pulses = sim_pulses_;
            }

            unsigned long delta = pulses >= previous_pulses ? pulses - previous_pulses : 0;
            EnergySnapshot snapshot = calc_.calculate(pulses, dt);
            double power = dt > 0.001
                ? (static_cast<double>(delta) / cfg_.pulses_per_kwh) * 3600000.0 / dt
                : 0.0;

            {
                std::lock_guard<std::mutex> lock(mu_);
                snapshot_ = snapshot;
                snapshot_.power_w = power;
                alert_ = alerts_.isHighPower(power);
                history_power_.push_back(power);
                history_energy_.push_back(snapshot.energy_kwh);
                if (history_power_.size() > 60) history_power_.erase(history_power_.begin());
                if (history_energy_.size() > 60) history_energy_.erase(history_energy_.begin());
            }

            if (log_) {
                log_ << timestamp() << "," << pulses << ","
                     << std::fixed << std::setprecision(3)
                     << snapshot.energy_wh << "," << snapshot.energy_kwh << ","
                     << power << "," << (alert_ ? "HIGH" : "NORMAL") << ","
                     << (simulate_ ? "SIMULATOR" : "LINUX_DRIVER") << "\n";
                log_.flush();
            }

            previous_pulses = pulses;
            previous_time = now;
        }
    }

    unsigned long readPulses() {
        if (!simulate_) {
            unsigned long count = 0;
            if (device_.getCount(count)) return count;
            return 0;
        }
        std::lock_guard<std::mutex> lock(mu_);
        return sim_pulses_;
    }

    bool reset() {
        bool ok = false;
        if (!simulate_) ok = device_.reset();
        else {
            std::lock_guard<std::mutex> lock(mu_);
            sim_pulses_ = 0;
            snapshot_ = {};
            history_power_.clear();
            history_energy_.clear();
            alert_ = false;
            ok = true;
        }
        return ok;
    }

    bool addPulses(unsigned long n) {
        if (!simulate_) return device_.addPulses(n);
        std::lock_guard<std::mutex> lock(mu_);
        sim_pulses_ += n;
        return true;
    }

    std::string statusJson() {
        std::lock_guard<std::mutex> lock(mu_);
        std::ostringstream o;
        o << std::fixed << std::setprecision(3);
        o << "{\"mode\":\"" << (simulate_ ? "SIMULATOR" : "LINUX_DRIVER")
          << "\",\"system_online\":true"
          << ",\"driver_connected\":" << (!simulate_ ? "true" : "false")
          << ",\"device\":\"" << device_path_ << "\""
          << ",\"pulses\":" << snapshot_.pulses
          << ",\"energy_wh\":" << snapshot_.energy_wh
          << ",\"energy_kwh\":" << snapshot_.energy_kwh
          << ",\"power_w\":" << snapshot_.power_w
          << ",\"alert\":" << (alert_ ? "true" : "false")
          << ",\"threshold_w\":" << alerts_.threshold()
          << ",\"sample_interval_ms\":" << cfg_.sample_interval_ms
          << ",\"pulses_per_kwh\":" << cfg_.pulses_per_kwh
          << ",\"history_power\":[";
        for (size_t i = 0; i < history_power_.size(); ++i) {
            if (i) o << ',';
            o << history_power_[i];
        }
        o << "],\"history_energy\":[";
        for (size_t i = 0; i < history_energy_.size(); ++i) {
            if (i) o << ',';
            o << history_energy_[i];
        }
        o << "]}";
        return o.str();
    }

    std::string exportCsv() const {
        std::ifstream in(cfg_.log_file, std::ios::binary);
        if (!in) return "timestamp,pulses,energy_wh,energy_kwh,power_w,alert,mode\n";
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    std::string devicePath() const { return device_path_; }

private:
    static std::string timestamp() {
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm tm{};
        localtime_r(&t, &tm);
        char b[32];
        std::strftime(b, sizeof(b), "%Y-%m-%d %H:%M:%S", &tm);
        return b;
    }

    AppConfig cfg_;
    bool force_simulate_{false};
    bool simulate_{true};
    std::string device_path_;
    MeterDevice device_;
    EnergyCalculator calc_;
    AlertManager alerts_;
    std::ofstream log_;
    mutable std::mutex mu_;
    unsigned long sim_pulses_{0};
    EnergySnapshot snapshot_{};
    bool alert_{false};
    std::vector<double> history_power_;
    std::vector<double> history_energy_;
};

int main(int argc, char** argv) {
    std::string configPath = "config/config.conf";
    std::string devicePath = "/dev/smartmeter";
    bool simulate = false;
    int portOverride = -1;

    for (int i = 1; i < argc; ++i) {
        std::string a(argv[i]);
        if (a == "--simulate") simulate = true;
        else if (a == "--device" && i + 1 < argc) devicePath = argv[++i];
        else if (a == "--config" && i + 1 < argc) configPath = argv[++i];
        else if (a == "--port" && i + 1 < argc) {
            try { portOverride = std::stoi(argv[++i]); } catch (...) { return 2; }
        } else if (a == "--help") {
            std::cout << "Usage: smartmeter_app [--simulate] [--device PATH] [--config FILE] [--port PORT]\n";
            return 0;
        }
    }

    AppConfig cfg = loadConfig(configPath);
    if (portOverride > 0 && portOverride < 65536) cfg.web_port = portOverride;

    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    MeterService service(cfg, simulate, devicePath);
    service.start();
    std::thread worker(&MeterService::loop, &service);

    HttpServer server(
        cfg.web_port,
        [&service]{ return service.statusJson(); },
        [&service]{ return service.reset(); },
        [&service](unsigned long n){ return service.addPulses(n); },
        [&service]{ return service.exportCsv(); }
    );

    std::cout << "\nSmart Energy Meter\n";
    std::cout << "Dashboard: http://localhost:" << cfg.web_port << "\n";
    std::cout << "Device: " << service.devicePath() << "\n";
    server.run(running);

    running = false;
    if (worker.joinable()) worker.join();
    std::cout << "Smart Meter stopped.\n";
    return 0;
}
