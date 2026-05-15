#pragma once

#include <chrono>

class Timer {
public:
    Timer();
    void stop();
    double elapsedSeconds() const;

private:
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::high_resolution_clock::time_point end_;
};
