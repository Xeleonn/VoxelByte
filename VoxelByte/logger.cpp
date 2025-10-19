#include "globals.h"

Logger::Logger()
{
}

std::string Logger::GetCurrentTime()
{
    using namespace std::chrono;

    // Get current time
    auto now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);

    // Break down into calendar time
    std::tm local_tm{};
#ifdef _WIN32
    localtime_s(&local_tm, &now_c);
#else
    localtime_r(&now_c, &local_tm);
#endif

    // Format the base time (HH:MM:SS)
    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%H:%M:%S");

    // Extract centiseconds (2 decimals)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    int centiseconds = ms.count() / 10;

    oss << '.' << std::setw(2) << std::setfill('0') << centiseconds;

    return oss.str();
}

void Logger::Print(std::string message)
{
	std::cout << GetCurrentTime() << " <VoxelByte> " << message << '\n';
}