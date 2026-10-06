#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
#include "../driver/smartmeter_ioctl.h"

int main()
{
    int fd = open("/dev/smartmeter", O_RDWR);

    if (fd < 0) {
        perror("open");
        return 1;
    }

    unsigned long count = 0;

    // SET_COUNT
    count = 100;
    if (ioctl(fd, SMARTMETER_SET_COUNT, &count) < 0) {
        perror("SMARTMETER_SET_COUNT");
        close(fd);
        return 1;
    }

    // GET_COUNT
    count = 0;
    if (ioctl(fd, SMARTMETER_GET_COUNT, &count) < 0) {
        perror("SMARTMETER_GET_COUNT");
        close(fd);
        return 1;
    }

    std::cout << "GET_COUNT = " << count << std::endl;

    if (count != 100) {
        std::cerr << "SET/GET test failed" << std::endl;
        close(fd);
        return 1;
    }

    // RESET
    if (ioctl(fd, SMARTMETER_RESET) < 0) {
        perror("SMARTMETER_RESET");
        close(fd);
        return 1;
    }

    // GET_COUNT after reset
    count = 999;
    if (ioctl(fd, SMARTMETER_GET_COUNT, &count) < 0) {
        perror("SMARTMETER_GET_COUNT");
        close(fd);
        return 1;
    }

    std::cout << "After RESET = " << count << std::endl;

    close(fd);

    if (count != 0) {
        std::cerr << "RESET test failed" << std::endl;
        return 1;
    }

    std::cout << "All IOCTL tests passed." << std::endl;

    return 0;
}
