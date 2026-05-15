#include "metrics.hpp"

Timer::Timer() {
    start_ = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
    end_ = std::chrono::high_resolution_clock::now();
}

double Timer::elapsedSeconds() const {
    return std::chrono::duration<double>(end_ - start_).count();
}
