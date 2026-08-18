#ifndef LOGGER_SPEEDTEST_H
#define LOGGER_SPEEDTEST_H

#include <chrono>

namespace mymc :: log
{
class SpeedTest
{
public:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

public:
    SpeedTest()
    {
        start();
    }

    // 开始计时
    void start()
    {
        m_time_start = Clock::now();
    }

    // 返回经过时间（秒）
    [[nodiscard]] double elapsed() const
    {
        const auto end_time = Clock::now();
        return std::chrono::duration<double>(end_time - m_time_start).count();
    }

    // 返回经过时间（毫秒）
    [[nodiscard]] double elapsed_ms() const
    {
        const auto end_time = Clock::now();
        return std::chrono::duration<double, std::milli>(end_time - m_time_start).count();
    }

    // 返回经过时间（微秒）
    [[nodiscard]] double elapsed_us() const
    {
        const auto end_time = Clock::now();
        return std::chrono::duration<double, std::micro>(end_time - m_time_start).count();
    }

    // 重新开始计时并返回上一次耗时
    double restart()
    {
        const auto end_time = Clock::now();
        const double cost = std::chrono::duration<double>(end_time - m_time_start).count();
        m_time_start = end_time;
        return cost;
    }

private:
    TimePoint m_time_start{};
};
} // namespace mymc

#endif // LOGGER_SPEEDTEST_H
