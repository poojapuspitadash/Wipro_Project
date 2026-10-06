#pragma once
#include <atomic>
#include <functional>
#include <string>

class HttpServer {
public:
    using StatusProvider = std::function<std::string()>;
    using ResetHandler = std::function<bool()>;
    using PulseHandler = std::function<bool(unsigned long)>;
    using ExportHandler = std::function<std::string()>;

    HttpServer(int port, StatusProvider status, ResetHandler reset,
               PulseHandler pulse, ExportHandler export_csv);
    void run(std::atomic<bool>& running);
private:
    int port_;
    StatusProvider status_;
    ResetHandler reset_;
    PulseHandler pulse_;
    ExportHandler export_csv_;
};
