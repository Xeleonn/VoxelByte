#ifndef CLOCK_H
#define CLOCK_H

#include <chrono>

class Clock
{
public:
    Clock();
    void Update();
    double GetTime();
    double GetDeltaTime();
    float GetFPS();

private:
    double time;
    double delta_time;
    std::chrono::steady_clock::time_point current_tick;
    std::chrono::steady_clock::time_point last_tick;
    std::chrono::steady_clock::time_point init_tick;
};

#endif
