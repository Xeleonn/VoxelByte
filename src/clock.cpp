#include "clock.h"

Clock::Clock()
{
    current_tick = std::chrono::high_resolution_clock::now();
    last_tick = current_tick;
    seconds_since_start = 0;
}

void Clock::Update()
{
    last_tick = current_tick;
    current_tick = std::chrono::high_resolution_clock::now();
    seconds_since_start += GetDeltaTime();
}

double Clock::GetTime()
{
    return seconds_since_start;
}

double Clock::GetDeltaTime()
{
    std::chrono::duration<double> delta_time = current_tick - last_tick;
    return delta_time.count();
}

float Clock::GetFPS()
{
    return static_cast<float>(1 / GetDeltaTime());
}