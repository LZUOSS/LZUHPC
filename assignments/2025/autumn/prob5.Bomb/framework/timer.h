#ifndef FRAMEWORK_TIMER_H
#define FRAMEWORK_TIMER_H

#include <chrono>

struct Timer {
    std::chrono::high_resolution_clock::time_point start_time;
    inline void tic() {
        start_time = std::chrono::high_resolution_clock::now();
    }

    inline double toc() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end_time - start_time).count();
    }
};

#endif