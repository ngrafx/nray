#pragma once

#include <chrono>

class Timer {
  public:
    Timer() {}

    // Starts the timer
    void Start() {_start = std::chrono::system_clock::now();}
    // Stops the timer
    void Stop() {_end = std::chrono::system_clock::now();}

    // Print the timerDuration;
    void Print();

  private:
    std::chrono::time_point<std::chrono::system_clock> _start;
    std::chrono::time_point<std::chrono::system_clock> _end;
};