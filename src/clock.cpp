#include "clock.h"

Clock::Clock()
{
    current_tick = std::chrono::high_resolution_clock::now();
    last_tick = current_tick;
    init_tick = current_tick;
}

void Clock::Update()
{
    last_tick = current_tick;
    current_tick = std::chrono::high_resolution_clock::now();
}

double Clock::GetTime()
{
    std::chrono::duration<double> time = current_tick - init_tick;
    return time.count();
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