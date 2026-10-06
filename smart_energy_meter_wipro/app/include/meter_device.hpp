#pragma once
#include <string>

class MeterDevice {
public:
    explicit MeterDevice(const std::string& path);
    ~MeterDevice();
    bool openDevice();
    bool available() const;
    bool reset();
    bool getCount(unsigned long& count);
    bool addPulses(unsigned long pulses);
    int fd() const;
private:
    std::string path_;
    int fd_{-1};
};
