#include "Generator.h"
#include <random>

float Generator::randf(float min, float max)
{
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

std::string Generator::GetCurrentTime()
{
    auto now = std::chrono::system_clock::now();        // 현재 시간
    std::time_t t = std::chrono::system_clock::to_time_t(now);  // time_t 변환

    std::tm localTime;
    localtime_s(&localTime, &t);  // thread-safe 버전

    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%dT%H:%M:%S");  // ISO 8601 포맷
    return oss.str();
}

Signal Generator::GenerateSignal(std::string devName, int myMode)
{
    Signal s;
    s.device_id = devName;
    s.timestamp = GetCurrentTime();

    switch (myMode)
    {
    case 0: // NORMAL
        s.voltage = 220.0f + randf(-2, 2);
        s.current = randf(0.5f, 2.0f);
        s.frequency = 60.0f + randf(-0.02f, 0.02f);
        s.status = "NORMAL";
        break;
    case 1: // HIGH_LOAD
        s.voltage = 214.0f + randf(-2, 2);
        s.current = randf(5.0f, 10.0f);
        s.frequency = 59.85f + randf(-0.05f, 0.0f);
        s.status = "OVERLOAD";
        break;
    case 2: // GEN_FAIL
        s.voltage = randf(0, 20);
        s.current = 0.0f;
        s.frequency = 59.3f;
        s.status = "FAIL";
        break;
    case 3: // FREQ_DRIFT
        s.voltage = 220.0f + randf(-1, 1);
        s.current = randf(1.0f, 2.0f);
        s.frequency = 60.0f + randf(-0.3f, 0.3f);
        s.status = "UNSTABLE";
        break;
    case 4: // MAINTENANCE
        s.voltage = 0.0f;
        s.current = 0.0f;
        s.frequency = 0.0f;
        s.status = "OFFLINE";
        break;
    }

    //s.power = s.voltage * s.current;
    s.power_factor = 0.95f + randf(-0.02f, 0.02f);
    return s;
}
