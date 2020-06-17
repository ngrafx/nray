#include <iostream>
#include "timer.h"


void Timer::Print() {

    
    long seconds = std::chrono::duration_cast<std::chrono::seconds>(_end - _start).count();
    if (seconds <= 0) {
        long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(_end - _start).count();
        std::cout << milliseconds << "ms ";
        return;
    }

    long minutes = std::chrono::duration_cast<std::chrono::minutes>(_end - _start).count();
    if (minutes <= 0) {
        std::cout << seconds << "s ";
        return;
    }
    
    std::cout << minutes << "min" << seconds - (minutes*60) << "s";
}