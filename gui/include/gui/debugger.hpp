#pragma once

class Debugger {
public:
    Debugger() = default;
    ~Debugger() = default;
    Debugger(const Debugger&) = default;

    void showRegisters();
    void showTimers();
    void showProgrammCounter();
};