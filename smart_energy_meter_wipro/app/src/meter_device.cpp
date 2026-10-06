#include "meter_device.hpp"
#include "../../driver/smartmeter_ioctl.h"

#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cerrno>

MeterDevice::MeterDevice(const std::string& path) : path_(path) {}

MeterDevice::~MeterDevice() {
    if (fd_ >= 0) ::close(fd_);
}

bool MeterDevice::openDevice() {
    if (fd_ >= 0) return true;
    fd_ = ::open(path_.c_str(), O_RDWR | O_NONBLOCK);
    return fd_ >= 0;
}

bool MeterDevice::available() const { return fd_ >= 0; }

bool MeterDevice::reset() {
    if (fd_ < 0) return false;
    return ::ioctl(fd_, SMARTMETER_RESET) == 0;
}

bool MeterDevice::getCount(unsigned long& count) {
    if (fd_ < 0) return false;
    return ::ioctl(fd_, SMARTMETER_GET_COUNT, &count) == 0;
}

bool MeterDevice::addPulses(unsigned long pulses) {
    if (fd_ < 0) return false;
    std::string text = std::to_string(pulses);
    ssize_t n = ::write(fd_, text.c_str(), text.size());
    return n == static_cast<ssize_t>(text.size());
}

int MeterDevice::fd() const { return fd_; }
