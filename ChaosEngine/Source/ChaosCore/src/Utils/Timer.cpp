#include "ChaosCore/Utils/Timer.h"
#include "ChaosDebug.h"

namespace Chaos::Utils
{


    uint64_t Timer::s_frameCount = 0;

    Timer::FrameTime Timer::s_elapsedTime(0.f);
    Timer::FrameTime Timer::s_deltaTime(0.f);
    Timer::FrameTime Timer::s_accumulator(0.f);

    Timer::clock::time_point Timer::s_currentTime = clock::now();
    Timer::clock::time_point Timer::s_lastTime = clock::now();
    Timer::FrameTime Timer::s_timer(0.f);

    void Timer::Frame()
    {
        s_frameCount++;
        const auto newTime = clock::now();
        s_deltaTime = newTime - s_currentTime;
        if (s_deltaTime > MAX_DELTA_TIME)
            s_deltaTime = MAX_DELTA_TIME;
        s_currentTime = newTime;
        s_accumulator += s_deltaTime;
    }

    void Timer::FixedUpdate()
    {
        s_elapsedTime += FIXED_DELTA_TIME;
        s_accumulator -= FIXED_DELTA_TIME;
    }

    void Timer::Start()
    {
        s_lastTime = clock::now();
    }

    Timer::FrameTime& Timer::Stop()
    {
        s_timer = clock::now() - s_lastTime;
        return s_timer;
    }

    Timer::FrameTime& Timer::CurrentTimer()
    {
        s_timer = clock::now() - s_lastTime;

        return s_timer;
    }
}