CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread
CPPFLAGS ?= -Iapp/include
LDFLAGS ?= -pthread

APP_SRC := app/src/main.cpp app/src/energy.cpp app/src/alert.cpp app/src/meter_device.cpp app/src/http_server.cpp app/src/config.cpp
APP_BIN := build/smartmeter_app
TEST_BIN := build/test_calculations
IOCTL_TEST_BIN := build/test_ioctl

.PHONY: all clean app driver tests test-ioctl run-simulate

all: app

app: $(APP_BIN)

$(APP_BIN): $(APP_SRC) | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(APP_SRC) -o $@ $(LDFLAGS)

build:
	mkdir -p build

driver:
	$(MAKE) -C driver

tests: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): tests/test_calculations.cpp app/src/energy.cpp app/src/alert.cpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) tests/test_calculations.cpp app/src/energy.cpp app/src/alert.cpp -o $@

test-ioctl: $(IOCTL_TEST_BIN)
	./$(IOCTL_TEST_BIN)

$(IOCTL_TEST_BIN): tests/test_ioctl.cpp | build
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) tests/test_ioctl.cpp -o $@

run-simulate: app
	./$(APP_BIN) --simulate

clean:
	rm -rf build/*.o $(APP_BIN) $(TEST_BIN) $(IOCTL_TEST_BIN)
	-$(MAKE) -C driver clean

install-user: app
	mkdir -p $(HOME)/.local/bin
	cp $(APP_BIN) $(HOME)/.local/bin/smartmeter_app
