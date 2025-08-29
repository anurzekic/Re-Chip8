#pragma once

#include <array>
#include <cstdint>

class Debugger {
public:
    Debugger() = default;
    ~Debugger() = default;
    Debugger(const Debugger&) = default;

    void showRegisters(std::array<uint8_t, 16>& registers);
    void showTimers();
    void showProgrammCounter();
};