# Makefile — host-side C++ test harness for the Airspeed26 sketch.
#
# Single-run build + test entry point (no watch mode):
#
#     make            # build the test binary (default target)
#     make test       # build, then run the property/unit tests once
#     make clean      # remove build artifacts
#
# The pure helpers are compiled UNCHANGED from ../Airspeed26.ino via
# include_sketch.cpp behind the Arduino stubs in this directory (see
# arduino_stubs.h / Wire.h). RapidCheck (the prescribed C++ property-based
# testing library) is vendored under ./rapidcheck at a pinned commit and built
# from source here — no CMake required.
#
# Feature: pitot-airspeed-sensor — host test harness (task 4).

CXX      ?= c++
CXXSTD   ?= -std=c++14
CXXFLAGS ?= $(CXXSTD) -O1 -g -Wall -Wextra

RC_DIR   := rapidcheck
RC_INC   := -I$(RC_DIR)/include
# This directory is placed FIRST on the include path so the sketch's
# `#include <Wire.h>` resolves to our host shim (Wire.h) instead of a real one.
INC      := -I. $(RC_INC)

BUILD    := build
BIN      := $(BUILD)/airspeed26_tests

# RapidCheck sources (compiled once into object files under build/rc).
RC_SRCS  := $(shell find $(RC_DIR)/src -name '*.cpp' 2>/dev/null)
RC_OBJS  := $(patsubst $(RC_DIR)/src/%.cpp,$(BUILD)/rc/%.o,$(RC_SRCS))

# Our translation units: the sketch inclusion unit + the test entry point.
APP_SRCS := include_sketch.cpp test_main.cpp
APP_OBJS := $(patsubst %.cpp,$(BUILD)/%.o,$(APP_SRCS))

.PHONY: all test clean deps

# Default target: ensure the property-testing library is vendored, then build.
# deps is resolved first; because the RapidCheck source list is expanded when
# this Makefile is parsed, the build itself runs in a recursive $(MAKE) after
# the sources exist on disk.
all: deps
	@$(MAKE) --no-print-directory $(BIN)

# Ensure the vendored RapidCheck sources are present (fetched at a pinned
# commit). Runs automatically before compiling anything that needs them.
deps:
	@test -f $(RC_DIR)/include/rapidcheck.h || ./fetch_rapidcheck.sh

# Run the tests exactly once (single-run, no watch).
test: deps
	@$(MAKE) --no-print-directory $(BIN)
	@echo "── Running Airspeed26 host tests ──"
	./$(BIN)

$(BIN): $(APP_OBJS) $(RC_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $^ -o $@

# App/test objects. Note: include_sketch.cpp needs our shim include path first
# so <Wire.h> resolves locally; test_main.cpp needs the RapidCheck headers.
$(BUILD)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INC) -c $< -o $@

# RapidCheck objects — built from the vendored sources.
$(BUILD)/rc/%.o: $(RC_DIR)/src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(RC_INC) -c $< -o $@

clean:
	rm -rf $(BUILD)
