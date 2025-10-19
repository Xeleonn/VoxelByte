#include "globals.h"

Clock::Clock()
{
    current_tick = std::chrono::steady_clock::now();
    last_tick = current_tick;
    init_tick = current_tick;
    delta_time = 0;
    time = 0;
    VB::inst().GetLogger()->Print("Clock obj constructed");
}

void Clock::Update()
{
    last_tick = current_tick;
    current_tick = std::chrono::steady_clock::now();

    std::chrono::duration<double> c_delta_time = current_tick - last_tick;
    delta_time = c_delta_time.count();

    std::chrono::duration<double> c_time = current_tick - init_tick;
    time = c_time.count();
}

double Clock::GetTime()
{
    return time;
}

double Clock::GetDeltaTime()
{
    return delta_time;
}

float Clock::GetFPS()
{
    return static_cast<float>(1 / GetDeltaTime());
}