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
        std::chrono::_V2::system_clock::time_point current_tick;
        std::chrono::_V2::system_clock::time_point last_tick;
        double seconds_since_start;
};