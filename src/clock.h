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
        std::chrono::_V2::steady_clock::time_point current_tick;
        std::chrono::_V2::steady_clock::time_point last_tick;
        std::chrono::_V2::steady_clock::time_point init_tick;

};