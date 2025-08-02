#pragma once

#include <chrono>
#include <thread>
#include <type_traits>

template<std::intmax_t FPS>
class Timer {
public:
    // Define the duration type as 1/FPS seconds
    using frame_duration_t = std::chrono::duration<double, std::ratio<1, FPS>>;

    // Constructor initializes to current time
    Timer() : tp{std::chrono::steady_clock::now()} {}

    // Duration between frames (1/FPS seconds)
    static constexpr frame_duration_t time_between_frames{1};

    void sleep() {
        // Advance the timer to the next frame
        tp += time_between_frames;

        // Sleep most of the way there
        std::this_thread::sleep_until(tp - std::chrono::microseconds(100));

        // Busy wait the last ~100 microseconds
        while(std::chrono::steady_clock::now() < tp) {}
    }

private:
    std::chrono::time_point<std::chrono::steady_clock, frame_duration_t> tp;
};
